#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"
#include "Util.h"
#include "TutorialGame.h"
#include "PaintballEnemy.h"
#include "GameManager.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject() {
	counter = 0.0f;
	stateMachine = new StateMachine();

	State* patrolling = new State([&](float dt) -> void {
		Patrol(dt);
		});


	State* attacking = new State([&](float dt) -> void {
		AttackPlayer(dt);
		});


	stateMachine->AddState(patrolling);
	stateMachine->AddState(attacking);

	stateMachine->AddTransition(new StateTransition(patrolling, attacking, [&]() -> bool {
		return CanSeePlayer();
		}));

	stateMachine->AddTransition(new StateTransition(attacking, patrolling, [&]() -> bool {
		return !CanSeePlayer();
		}));

}

//void StateGameObject::SetPlayer(PaintballGameObject* playerObj) {
//	player = playerObj;
//}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	stateMachine->Update(dt);
}

void StateGameObject::Patrol(float dt) {
	static float patrolTimer = 0.0f;
	patrolTimer -= dt;

	//if (patrolTimer <= 0.0f) {
	//	patrolTimer = 5.0f;
	//	Vector3 randomPoint = Vector3(rand() % 20 - 10, 0, rand() % 20 - 10);
	//	pathNodes.clear();
	//	pathNodes.push_back(randomPoint);
	//}

	//MoveEnemyAlongPath();
}

void StateGameObject::AttackPlayer(float dt) {
	static float attackCooldown = 0.0f;
	attackCooldown -= dt;

	if (attackCooldown <= 0.0f) {
		attackCooldown = 1.5f; // Ã¿ 1.5 ÃëÉä»÷Ò»´Î

		//Vector3 enemyPos = GetTransform().GetPosition();
		//Vector3 playerPos = playerObj->GetTransform().GetPosition();
		Vector3 enemyPos = Util::RP3dToNCL(enemyObject->GetTransform().GetPosition());
		Vector3 playerPos = Util::RP3dToNCL(playerObj->GetTransform().GetPosition());

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

bool StateGameObject::CanSeePlayer() {
	/*Vector3 enemyPos = GetTransform().GetPosition();
	Vector3 playerPos = playerObj->GetTransform().GetPosition();*/
	Vector3 enemyPos = Util::RP3dToNCL(enemyObject->GetTransform().GetPosition());
	Vector3 playerPos = Util::RP3dToNCL(playerObj->GetTransform().GetPosition());

	rp3d::Ray ray(Util::NCLToRP3d(enemyPos), Util::NCLToRP3d(playerPos));
	RaycastHitCallback callback;
	RpWorld->raycast(ray, &callback);

	if (callback.rb && callback.rb->getUserData()) {
		return (PaintballGameObject*)callback.rb->getUserData() == playerObj;
	}
	return false;
}

//float StateGameObject::DistanceToPlayer() {
//	if (!playerObj) return -1.0f;
//
//	//Vector3 enemyPos = GetTransform().GetPosition();
//	//Vector3 playerPos = playerObj->GetTransform().GetPosition();
//	Vector3 enemyPos = Util::RP3dToNCL(enemyObject->GetTransform().GetPosition());
//	Vector3 playerPos = Util::RP3dToNCL(playerObj->GetTransform().GetPosition());
//
//	return (enemyPos - playerPos).Length();
//}
