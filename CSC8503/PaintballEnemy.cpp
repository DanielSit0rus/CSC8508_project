#include "PaintballEnemy.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include <iostream>
#include <cmath>
#include "PaintballGameObject.h"
#include "StateGameObject.h"
#include "GameManager.h"
#include "EventManager.h"

using namespace NCL::CSC8503;

PaintballEnemy::PaintballEnemy(const std::string& name, Vector4 color) : enemyColor(color)
{
	type = GameObjectType::enemy;

	stateMachine = new StateMachine();
	navMesh = nullptr;
	player = nullptr;
	canSeeTest = false;
	type = GameObjectType::enemy;


	

	patrolling = new State([&](float dt) -> void {
		if (health > 0)
		{
			currentState = patrolling;
			this->GetRenderObject()->SetAnimation(ResourceManager::GetInstance().GetMoveanim());
			Patrol(dt);
		}
		else
		{
			this->GetRenderObject()->SetAnimation(nullptr);
		}
		});

	chasing = new State([&](float dt) -> void {
		currentState = chasing;
		if (health > 0)
		{
			this->GetRenderObject()->SetAnimation(ResourceManager::GetInstance().GetMoveanim());
			Chase(dt);
		}
		else
		{
			this->GetRenderObject()->SetAnimation(nullptr);
		}
		});

	attacking = new State([&](float dt) -> void {
		currentState = attacking;
		if (health > 0)
		{
			this->GetRenderObject()->SetAnimation(ResourceManager::GetInstance().GetIdleanim());
			Attack(Vector4(1, 0, 0, 1));
		}
		else
		{
			this->GetRenderObject()->SetAnimation(nullptr);
		}
		});

	stateMachine->AddState(patrolling);
	stateMachine->AddState(chasing);
	stateMachine->AddState(attacking);

	stateMachine->AddTransition(new StateTransition(patrolling, chasing, [&]() -> bool {
		return canSeeTest && (distanceToPlayer <= stopchaseRange);
		}));

	stateMachine->AddTransition(new StateTransition(chasing, attacking, [&]() -> bool {
		return canSeeTest && (distanceToPlayer < attackRange);
		}));

	stateMachine->AddTransition(new StateTransition(attacking, chasing, [&]() -> bool {
		return canSeeTest && (distanceToPlayer > attackRange);
		}));

	stateMachine->AddTransition(new StateTransition(attacking, patrolling, [&]() -> bool {
		return !canSeeTest || (distanceToPlayer > stopchaseRange);
		}));

	stateMachine->AddTransition(new StateTransition(chasing, patrolling, [&]() -> bool {
		return !canSeeTest || (distanceToPlayer > stopchaseRange);
		}));

	GameManager::GetInstance().IncreaseEnemyCount(); 
}

PaintballEnemy::~PaintballEnemy()
{
}

void NCL::CSC8503::PaintballEnemy::TakeDamage(int damage, Vector4 bulletColor) {
	if (IsOppositeColor(bulletColor)) {
		health -= damage;
	}

	if (health <= 0) {
		if (indicatorSphere) {
			GameManager::GetInstance().DeleteObject(indicatorSphere);
			indicatorSphere = nullptr;
		}
	}
}

bool PaintballEnemy::IsOppositeColor(const Vector4& bulletColor) {
	// Check if the colors are opposites
	if ((enemyColor.x == 1 && enemyColor.y == 0 && enemyColor.z == 0 && // Red
		bulletColor.x == 0 && bulletColor.y == 1 && bulletColor.z == 0) || // Green vs Red

		(enemyColor.x == 0 && enemyColor.y == 1 && enemyColor.z == 0 && // Green
			bulletColor.x == 0 && bulletColor.y == 0 && bulletColor.z == 1) || // Blue vs Green

		(enemyColor.x == 0 && enemyColor.y == 0 && enemyColor.z == 1 && // Blue
			bulletColor.x == 1 && bulletColor.y == 0 && bulletColor.z == 0)) // Red vs Blue
	{
		return true;
	}
	return false;
}



void PaintballEnemy::Update(float dt)
{
	if (!IsActive()) {
		return;
	}

	StateGameObject::Update(dt);
	canSeeTest = CanSeePlayer();
	if(player) distanceToPlayer = (player->GetTransform().GetPosition() - GetTransform().GetPosition()).length();
	Debug::Print("canSeeTest: " + std::to_string(canSeeTest), Vector2(10, 90), Debug::WHITE);
	leftCD -= dt;

	if (canSeeTest) {
		rp3d::Vector3 enemyPos = GetTransform().GetPosition();
		rp3d::Vector3 playerPos = player->GetTransform().GetPosition();
		rp3d::Vector3 direction = playerPos - enemyPos;
		direction.y = 0;
		direction.normalize();

		float angle = atan2(direction.x, direction.z);
		rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0, angle, 0);
		GetTransform().SetOrientation(newRotation);
	}

	if (indicatorSphere) {
		rp3d::Vector3 enemyPos = GetTransform().GetPosition();
		indicatorSphere->GetTransform().SetPosition(enemyPos + rp3d::Vector3(0, 4, 0));
	}

	if (audioObject) {
		float speed = physicsObject->GetRigidbody().getLinearVelocity().length();
		float clampedSpeed = Maths::Clamp(speed * 0.06f, 0.f, 1.f);
		audioObject->PlayEvent("event:/Effect/FootStep", clampedSpeed);
	}


	if (health <= 0)
	{

		rp3d::Vector3 currentPos = this->GetTransform().GetPosition();

		this->GetPhysicsObject()->AddForce(reactphysics3d::Vector3(0, 30, 0));
		if (currentPos.y >= 10.0f&&!isDead) {
			GameManager::GetInstance().DecreaseEnemyCount();
			isDead = true;
			//SetActive(false); // Deactivate the object
			GameManager::GetInstance().DeleteObject(this);
		}
	}


	// --- Stuck detection ---
	rp3d::Vector3 currentPos = this->GetTransform().GetPosition();
	float movedDistance = (currentPos - lastPos).length();

	if (movedDistance < stuckThreshold) {
		stuckTimer += dt;
	}
	else {
		stuckTimer = 0.0f; // reset if we moved enough
	}

	lastPos = currentPos;

	if (stuckTimer > maxStuckTime && currentState != attacking) {
		std::cout << "Enemy stuck! Resetting path..." << std::endl;
		pathNodes.clear();
		stuckTimer = 0.0f;

		// Optional: immediately go back to patrol
		canSeeTest = false;
	}




}

void PaintballEnemy::Chase(float dt) {
	rp3d::Vector3 enemyPos = GetTransform().GetPosition();
	rp3d::Vector3 playerPos = player->GetTransform().GetPosition();

	float distanceToPlayer = (playerPos - enemyPos).length();

	if (distanceToPlayer <= chaseRange && CanSeePlayer()) {
		NavigationPath newPath;
		if (navMesh->FindPath(Util::RP3dToNCL(enemyPos), Util::RP3dToNCL(playerPos), newPath)) {
			pathNodes.clear();
			Vector3 pos;
			int count = 0;
			while (newPath.PopWaypoint(pos)) {
				if (count % 2 == 0) {
					pathNodes.push_back(pos);
				}
				count++;
			}
		}
	}

	MoveEnemyAlongPath();
}

void PaintballEnemy::Patrol(float dt) {
	if (canSeeTest) return;

	rp3d::Vector3 enemyPos = GetTransform().GetPosition();

	if (pathNodes.empty() || (enemyPos - patrolTarget).length() < 1.0f) {
		bool foundValidTarget = false;

		for (int i = 0; i < 5; ++i) { 
			float randomX = (rand() % 40) - 20;
			float randomZ = (rand() % 40) - 20;

			rp3d::Vector3 candidate = enemyPos + rp3d::Vector3(randomX, 0, randomZ);

	
			rp3d::Ray ray(enemyPos, candidate);
			RaycastHitCallback callback;

			GameManager::GetInstance().getRPworld()->raycast(ray, &callback);

			if (!callback.rb) {
				patrolTarget = candidate;
				CalculatePath(patrolTarget);
				foundValidTarget = true;
				break;
			}
		}
	}
	MoveEnemyAlongPath();
}


void PaintballEnemy::Attack(Vector4 color)
{
	const float attackRange = 55.0f;

	
	if (!player || !player->IsActive()) {
		return;
	}

	if (leftCD < 0) {
		GameManager::GetInstance().AddObject(GameObjectType::bullet,
			transform.GetPosition() + rp3d::Vector3(0, 4, 0), rp3d::Vector3(1, 1, 1), rp3d::Quaternion().identity(),
			color, "basic", "basic", "", "", "", "", "", "", "", "basic", 1, false, player->GetTransform().GetPosition() - transform.GetPosition());
		leftCD = totalCD;
	}
}

bool PaintballEnemy::CanSeePlayer() {
	if (player == nullptr) return false;

	rp3d::Vector3 enemyPos = GetTransform().GetPosition();
	rp3d::Vector3 playerPos = player->GetTransform().GetPosition();

	float distanceToPlayer = (playerPos - enemyPos).length();
	if (distanceToPlayer > stopchaseRange) {
		return false;
	}

	rp3d::Ray ray(enemyPos, playerPos);
	RaycastHitCallback callback;
	GameManager::GetInstance().getRPworld()->raycast(ray, &callback);

	if (callback.rb && callback.rb->getUserData()) {

		return (PaintballGameObject*)callback.rb->getUserData() == player;
	}
	return false;
}

void PaintballEnemy::MoveEnemyAlongPath() {
	if (pathNodes.empty()) {
		this->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
		return;
	}

	rp3d::Vector3 currentPos = this->GetTransform().GetPosition();
	rp3d::Vector3 targetPos = Util::NCLToRP3d(pathNodes.front());
	targetPos.y = currentPos.y; // Keep enemy on the same Y level

	// Compute direction and distance to the target node
	rp3d::Vector3 direction = targetPos - currentPos;
	float distanceToTarget = direction.length();

	// Movement parameters
	float moveSpeed = 5.0f;
	float arrivalThreshold = 0.3f; // Distance at which a node is considered reached

	if (distanceToTarget < arrivalThreshold) {
		// Remove the reached node
		pathNodes.erase(pathNodes.begin());
		if (pathNodes.empty()) {
			this->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
			return;
		}
		// Update the target position to the new front of the path
		targetPos = Util::NCLToRP3d(pathNodes.front());
		targetPos.y = currentPos.y;
		direction = targetPos - currentPos;
	}
	if (direction.length() > 0.01f) {
		direction.normalize();
		float angle = atan2(direction.x, direction.z); 
		rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0, angle, 0);
		GetTransform().SetOrientation(newRotation);
	}


	// Normalize direction and apply speed
	direction.normalize();
	rp3d::Vector3 velocity = direction * moveSpeed;

	// Set linear velocity
	this->GetPhysicsObject()->SetLinearVelocity(velocity);


	for (size_t i = 1; i < pathNodes.size(); ++i) {
		const Vector3& from = pathNodes[i - 1];
		const Vector3& to = pathNodes[i];
		Debug::DrawLine(from, to, Vector4(1, 1, 0, 1), 0.01f); 
	}


}

void PaintballEnemy::CalculatePath(rp3d::Vector3 pos) {
	pathNodes.clear();
	if (navMesh == nullptr)return;

	rp3d::Vector3 startPos = this->GetTransform().GetPosition();
	rp3d::Vector3 endPos = pos;

	NavigationPath outPath;

	if (navMesh->FindPath(Util::RP3dToNCL(startPos), Util::RP3dToNCL(endPos), outPath)) {
		Vector3 pos;
		int count = 0;
		while (outPath.PopWaypoint(pos)) {
			if (count % 2 == 0) {
				pathNodes.push_back(pos);
			}
			count++;
		}
	}
}

void PaintballEnemy::InitBehaviorTree() {
	BTSelector* root = new BTSelector();

	BTSequence* attackSequence = new BTSequence();
	attackSequence->AddChild(new ConditionCanSeePlayer(this));
	attackSequence->AddChild(new ActionAttackPlayer(this));

	BTSequence* chaseSequence = new BTSequence();
	chaseSequence->AddChild(new ConditionCanSeePlayer(this));
	chaseSequence->AddChild(new ActionMoveToPlayer(this));

	root->AddChild(chaseSequence);
	root->AddChild(attackSequence);
	root->AddChild(new ActionPatrol(this));

	behaviorTree = root;
}

