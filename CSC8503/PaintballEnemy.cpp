#include "PaintballEnemy.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"

#include "GameManager.h"
#include "EventManager.h"

using namespace NCL::CSC8503;

PaintballEnemy::PaintballEnemy() :StateGameObject()
{
	stateMachine = new StateMachine();
	navMesh = nullptr;
	player = nullptr;
	canSeeTest = false;

	State* patrolling = new State([&](float dt) -> void {
		Patrol(dt);
		});

	State* chasing = new State([&](float dt) -> void {
		Chase(dt);
		});

	State* attacking = new State([&](float dt) -> void {
		Attack( Vector4(1, 0, 0, 1));
		});

	stateMachine->AddState(patrolling);
	stateMachine->AddState(chasing);
	stateMachine->AddState(attacking);

	stateMachine->AddTransition(new StateTransition(patrolling, chasing, [&]() -> bool {
		return canSeeTest && (distanceToPlayer <= chaseRange);
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

}

PaintballEnemy::~PaintballEnemy()
{
}

void PaintballEnemy::Update(float dt)
{
	StateGameObject::Update(dt);
	canSeeTest = CanSeePlayer();
	distanceToPlayer = (player->GetTransform().GetPosition() - GetTransform().GetPosition()).length();
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
}

void PaintballEnemy::Chase(float dt) {
	rp3d::Vector3 enemyPos = GetTransform().GetPosition();
	rp3d::Vector3 playerPos = player->GetTransform().GetPosition();

	float distanceToPlayer = (playerPos - enemyPos).length();

	if (distanceToPlayer <= chaseRange && CanSeePlayer()) {
		CalculatePath(player->GetTransform().GetPosition());
	}
	MoveEnemyAlongPath();
}

void PaintballEnemy::Patrol(float dt) {
	if (canSeeTest) return;
	rp3d::Vector3 enemyPos = GetTransform().GetPosition();

	if (pathNodes.empty() || (enemyPos - patrolTarget).length() < 1.0f) {
		float randomX = (rand() % 40) - 20;
		float randomZ = (rand() % 40) - 20;
		patrolTarget = Util::NCLToRP3d(NCL::Maths::Vector3(randomX, enemyPos.y, randomZ));

		Debug::Print("New Patrol Target: " + std::to_string(randomX) + ", " + std::to_string(randomZ), Vector2(10, 150), Debug::WHITE);

		CalculatePath(patrolTarget);
	}

	MoveEnemyAlongPath();
}

void PaintballEnemy::Attack(Vector4 color)
{
	const float attackRange = 55.0f;
	if (leftCD < 0) {
		GameManager::GetInstance().AddObject(GameObjectType::bullet,
			transform.GetPosition() + rp3d::Vector3(0, 4, 0), rp3d::Vector3(1, 1, 1), rp3d::Quaternion().identity(),
			color, "basic", "basic", "", "", "", "", "", "", "", "basic", 1, false, player->GetTransform().GetPosition() - transform.GetPosition());
		leftCD = totalCD;
	}
}

bool PaintballEnemy::CanSeePlayer() {
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
	float moveSpeed = 10.0f;
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

	// Normalize direction and apply speed
	direction.normalize();
	rp3d::Vector3 velocity = direction * moveSpeed;

	// Set linear velocity
	this->GetPhysicsObject()->SetLinearVelocity(velocity);

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
