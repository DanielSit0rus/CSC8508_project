#include "PaintballEnemy.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "Debug.h"
#include "GameManager.h"
#include "EventManager.h"

using namespace NCL::CSC8503;

PaintballEnemy::PaintballEnemy() :StateGameObject()
{
	navMesh = nullptr;
	player = nullptr;

	State* patrolling = new State([&](float dt) -> void {
		Patrol(dt);
		});


	State* attacking = new State([&](float dt) -> void {
		AttackPlayer(dt);
		});


	stateMachine->AddState(patrolling);
	stateMachine->AddState(attacking);

	stateMachine->AddTransition(new StateTransition(patrolling, attacking, [&]() -> bool {
		return canSeeTest;
		}));


	stateMachine->AddTransition(new StateTransition(attacking, patrolling, [&]() -> bool {
		return !canSeeTest;
		}));

	InitBehaviorTree();
}

PaintballEnemy::~PaintballEnemy()
{
	delete behaviorTree;
}

void PaintballEnemy::Update(float dt)
{
	StateGameObject::Update(dt);
	behaviorTree->Execute();
	//std::cout << "PaintballEnemy::Update" << std::endl;
}

void PaintballEnemy::Patrol(float dt) {
	CalculatePath(player->GetTransform().GetPosition());

	//std::cout << player->GetTransform().GetPosition().x << std::endl;

	MoveEnemyAlongPath();
}

void PaintballEnemy::AttackPlayer(float dt) {
	static float attackCooldown = 0.0f;
	attackCooldown -= dt;

	if (attackCooldown <= 0.0f) {
		attackCooldown = 1.5f; // Ã¿ 1.5 ÃëÉä»÷Ò»´Î

		rp3d::Vector3 enemyPos = GetTransform().GetPosition();
		rp3d::Vector3 playerPos = player->GetTransform().GetPosition();
		std::cout << "PaintballEnemy::AttackPlayer" << std::endl;

		/*	if (CanSeePlayer()) {
				Debug::DrawLine(enemyPos, playerPos, Vector4(1, 0, 0, 1));
				playerHealth -= 10;
				Debug::Print("Player HP: " + std::to_string(playerHealth), Vector2(10, 10), Debug::RED);
			}

			if (playerHealth <= 0) {
				Debug::Print("Player Killed!", Vector2(10, 20), Debug::RED);
			}*/
	}
}

bool PaintballEnemy::CanSeePlayer() {
	rp3d::Vector3 enemyPos = GetTransform().GetPosition();
	rp3d::Vector3 playerPos = player->GetTransform().GetPosition();

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

	// Get current position and the next target position
	rp3d::Vector3 currentPos = this->GetTransform().GetPosition();
	rp3d::Vector3 targetPos = pathNodes.size() > 1 ? Util::NCLToRP3d(pathNodes[pathNodes.size() - 2])
		: Util::NCLToRP3d(pathNodes.back());
	targetPos.y = currentPos.y; // Keep enemy on the same Y level

	Debug::Print("Moving to: " + std::to_string(targetPos.x) + ", " + std::to_string(targetPos.y) + ", " + std::to_string(targetPos.z), Vector2(10, 100), Debug::WHITE);
	
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