//#include "PaintballEnemy.h"
//#include "PaintballGameWorld.h"
//#include "StateGameObject.h"
//#include "TutorialGame.h"
//#include "StateTransition.h"
//#include "StateMachine.h"
//#include "State.h"
//
//using namespace NCL::CSC8503;
//
//Enemy::Enemy() {
//    stateMachine = new StateMachine();
//    player = nullptr;
//
//    State* patrolling = new State([&](float dt) -> void {
//        Patrol(dt);
//        });
//
//    State* attacking = new State([&](float dt) -> void {
//        AttackPlayer(dt);
//        });
//
//    stateMachine->AddState(patrolling);
//    stateMachine->AddState(attacking);
//
//
//    stateMachine->AddTransition(new StateTransition(patrolling, attacking, [&]() -> bool {
//        return CanSeePlayer();
//        }));
//
//    stateMachine->AddTransition(new StateTransition(attacking, patrolling, [&]() -> bool {
//        return !CanSeePlayer();
//        }));
//}
//
//Enemy::~Enemy() {
//    delete stateMachine;
//}
//
//void Enemy::Update(float dt) {
//    stateMachine->Update(dt);
//}
//
//void Enemy::SetPlayer(GameObject* playerObj) {
//    player = playerObj;
//}
//
//bool Enemy::CanSeePlayer() {
//    if (!player) return false;
//
//    Vector3 enemyPos = GetTransform().GetPosition();
//    Vector3 playerPos = player->GetTransform().GetPosition();
//
//    rp3d::Ray ray(Util::RP3dToNCL(enemyPos), Util::RP3dToNCL(playerPos));
//    RaycastHitCallback callback;
//    RpWorld->raycast(ray, &callback);
//
//    if (callback.rb && callback.rb->getUserData()) {
//        return (GameObject*)callback.rb->getUserData() == player;
//    }
//    return false;
//}
//
//void Enemy::Patrol(float dt) {
//    static float patrolTimer = 0.0f;
//    patrolTimer -= dt;
//
//    if (patrolTimer <= 0.0f) {
//        patrolTimer = 5.0f;
//        Vector3 randomPoint = Vector3(rand() % 20 - 10, 0, rand() % 20 - 10);
//        pathNodes.clear();
//        pathNodes.push_back(randomPoint);
//    }
//
//    MoveEnemyAlongPath(); // AI 进行移动
//}
//
//void Enemy::AttackPlayer(float dt) {
//    static float attackCooldown = 0.0f;
//    attackCooldown -= dt;
//
//    if (attackCooldown <= 0.0f) {
//        attackCooldown = 1.5f; // 每 1.5 秒攻击一次
//
//        Vector3 enemyPos = GetTransform().GetPosition();
//        Vector3 playerPos = player->GetTransform().GetPosition();
//
//        if (CanSeePlayer()) {
//            Debug::DrawLine(enemyPos, playerPos, Vector4(1, 0, 0, 1));
//            Debug::Print("Enemy Attacking!", Vector2(10, 10), Debug::RED);
//        }
//    }
//}
