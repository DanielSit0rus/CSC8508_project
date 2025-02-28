#include "Enemy.h"
#include "StateGameObject.h"
#include "PaintballGameObject.h"
#include "StateMachine.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

EnemyAI::EnemyAI() {
    stateMachine = new StateMachine();
    //behaviorTree = nullptr;
    player = nullptr;
    navMesh = nullptr;

    //InitBehaviorTree();
}

void EnemyAI::Patrol(float dt) {
    static float stopTime = 0.0f;
    stopTime -= dt;

    if (stopTime <= 0.0f) {
        stopTime = rand() % 5 + 1;
        GetPhysicsObject()->SetLinearVelocity({ 0, 0, 0 });
        return;
    }

    StateGameObject::Patrol(dt); // 继续调用父类巡逻逻辑
}


EnemyAI::~EnemyAI() {
    delete stateMachine;
    //delete behaviorTree;
}

// AI 每帧更新
/*
void EnemyAI::Update(float dt) {
    if (behaviorTree) {
        behaviorTree->Execute(); // 如果使用行为树
    }
    else {
        stateMachine->Update(dt); // 否则使用状态机
    }
}
*/

// 绑定玩家对象
void EnemyAI::SetPlayer(GameObject* playerObj) {
    player = playerObj;
}

/*
// 初始化行为树
void EnemyAI::InitBehaviorTree() {
    behaviorTree = new Selector();

    Sequence* attackSequence = new Sequence();
    attackSequence->AddChild(new ConditionCanSeePlayer(this));
    attackSequence->AddChild(new ActionAttackPlayer(this));

    Sequence* chaseSequence = new Sequence();
    chaseSequence->AddChild(new ConditionCanSeePlayer(this));
    chaseSequence->AddChild(new ActionMoveToPlayer(this));

    behaviorTree->AddChild(attackSequence);
    behaviorTree->AddChild(chaseSequence);
    behaviorTree->AddChild(new ActionPatrol(this)); // 默认巡逻
}
*/
