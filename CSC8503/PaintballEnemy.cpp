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
		//Attack(Vector3(1, 1, 1), Vector4(1, 0, 0, 1));
		});


	State* attacking = new State([&](float dt) -> void {
		Attack(Vector3(1, 1, 1), Vector4(1,0,0,1));
		});


	stateMachine->AddState(patrolling);
	stateMachine->AddState(attacking);

	stateMachine->AddTransition(new StateTransition(patrolling, attacking, [&]() -> bool {
		return canSeeTest;
		}));


	stateMachine->AddTransition(new StateTransition(attacking, patrolling, [&]() -> bool {
		return !canSeeTest;
		}));
}

PaintballEnemy::~PaintballEnemy()
{
}

void PaintballEnemy::Update(float dt)
{
	canSeeTest = CanSeePlayer();
	StateGameObject::Update(dt);
	//std::cout << "PaintballEnemy::Update" << std::endl;
}

void PaintballEnemy::Patrol(float dt) {
	rp3d::Vector3 enemyPos = GetTransform().GetPosition();
	rp3d::Vector3 playerPos = player->GetTransform().GetPosition();

	float distanceToPlayer = (playerPos - enemyPos).length();

	const float chaseRange = 5.0f;
	const float patrolRange = 10.0f;

	if (distanceToPlayer <= chaseRange) {
		CalculatePath(player->GetTransform().GetPosition());
		MoveEnemyAlongPath();
	}
	else if(distanceToPlayer >= patrolRange) {
		if (pathNodes.empty() || (this->GetTransform().GetPosition() - patrolTarget).length() < 1.0f) {
			float randomX = (rand() % 40) - 20;
			float randomZ = (rand() % 40) - 20;

			//patrolTarget = rp3d:: Vector3(randomX, this->GetTransform().GetPosition().y, randomZ);
			patrolTarget = Util::NCLToRP3d(NCL::Maths::Vector3(randomX, this->GetTransform().GetPosition().y, randomZ));

			CalculatePath(patrolTarget);
		}
	}
	//CalculatePath(player->GetTransform().GetPosition());

	//std::cout << player->GetTransform().GetPosition().x << std::endl;
}

//void PaintballEnemy::SetNewPatrolTarget() {
//	// ѡ�����Ѳ�ߵ㣨��Χ�� -20 �� 20 ֮�䣩
//	float randomX = (rand() % 40) - 20;
//	float randomZ = (rand() % 40) - 20;
//
//	patrolTarget = Vector3(randomX, this->GetTransform().GetPosition().y, randomZ);
//
//	Debug::Print("New patrol target: " + std::to_string(randomX) + ", " + std::to_string(randomZ), Vector2(10, 130), Debug::WHITE);
//
//	CalculatePath(patrolTarget);
//}
//
//bool PaintballEnemy::HasReachedTarget() {
//	float distance = (this->GetTransform().GetPosition() - patrolTarget).Length();
//	return distance < 1.0f;  // 1.0f �����㵽��Ŀ��
//}

void PaintballEnemy::Attack(Vector3 front, Vector4 color)
{

	GameManager::GetInstance().AddObject(GameObjectType::bullet,
		transform.GetPosition() + rp3d::Vector3(0, 4, 0), rp3d::Vector3(1, 1, 1), rp3d::Quaternion().identity(),
		color, "", "basic", "basic", "", "", 1, false, Util::NCLToRP3d(front));
}

bool PaintballEnemy::CanSeePlayer() {
	rp3d::Vector3 enemyPos = GetTransform().GetPosition();
	rp3d::Vector3 playerPos = player->GetTransform().GetPosition();

	//rp3d::Ray ray(enemyPos, playerPos);
	rp3d::Vector3 direction = playerPos - enemyPos;
	rp3d::Ray ray(enemyPos, direction);
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

	// Get current position and the next target position
	rp3d::Vector3 currentPos = this->GetTransform().GetPosition();
	rp3d::Vector3 targetPos = pathNodes.size() > 1 ? Util::NCLToRP3d(pathNodes[pathNodes.size() - 2])
		: Util::NCLToRP3d(pathNodes.back());
	targetPos.y = currentPos.y; // Keep enemy on the same Y level

	// Compute direction and distance to the target node
	rp3d::Vector3 direction = targetPos - currentPos;
	float distanceToTarget = direction.length();

	// Movement parameters
	float moveSpeed = 4.0f;
	float arrivalThreshold = 0.5f; // Distance at which a node is considered reached

	if (distanceToTarget < arrivalThreshold) {
		// Remove the reached node
		pathNodes.erase(pathNodes.begin());
		if (pathNodes.empty()) {
			this->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
			return;
		}
		// Update the target position to the new front of the path
		targetPos = pathNodes.size() > 1 ? Util::NCLToRP3d(pathNodes[pathNodes.size() - 2])
			: Util::NCLToRP3d(pathNodes.back());
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
		while (outPath.PopWaypoint(pos)) {
			pathNodes.push_back(pos);
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

void PaintballEnemy::SetTransform(const rp3d::Vector3& pos) {
	this->GetTransform().SetPosition(pos);
}