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

    StateGameObject::Patrol(dt); // �������ø���Ѳ���߼�
}


EnemyAI::~EnemyAI() {
    delete stateMachine;
    //delete behaviorTree;
}

// AI ÿ֡����
/*
void EnemyAI::Update(float dt) {
    if (behaviorTree) {
        behaviorTree->Execute(); // ���ʹ����Ϊ��
    }
    else {
        stateMachine->Update(dt); // ����ʹ��״̬��
    }
}
*/

// ����Ҷ���
void EnemyAI::SetPlayer(GameObject* playerObj) {
    player = playerObj;
}

/*
// ��ʼ����Ϊ��
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
    behaviorTree->AddChild(new ActionPatrol(this)); // Ĭ��Ѳ��
}
*/
