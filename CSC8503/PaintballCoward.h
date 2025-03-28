#pragma once
#include "StateGameObject.h"
#include "NavigationMesh.h"
#include "NavigationPath.h"

using namespace NCL::Maths;

namespace NCL::CSC8503 {

    class PaintballCoward;

    class BTNodeCoward {
    public:
        virtual bool Execute(float dt) = 0;
        virtual ~BTNodeCoward() {}
    };

    class BTSelectorCoward : public BTNodeCoward {
    private:
        std::vector<BTNodeCoward*> children;
    public:
        BTSelectorCoward() {}
        ~BTSelectorCoward() {
            for (auto child : children) delete child;
        }
        void AddChild(BTNodeCoward* child) { children.push_back(child); }
        bool Execute(float dt) override {
            for (auto child : children) {
                if (child->Execute(dt)) return true;
            }
            return false;
        }
    };

    class BTSequenceCoward : public BTNodeCoward {
    private:
        std::vector<BTNodeCoward*> children;
    public:
        BTSequenceCoward() {}
        ~BTSequenceCoward() {
            for (auto child : children) delete child;
        }
        void AddChild(BTNodeCoward* child) { children.push_back(child); }
        bool Execute(float dt) override {
            for (auto child : children) {
                if (!child->Execute(dt)) return false;
            }
            return true;
        }
    };


    class PaintballCoward : public StateGameObject {
    public:
        PaintballCoward(const std::string& name, Vector4 color);
        ~PaintballCoward();

        void Update(float dt) override;
        void TakeDamage(int damage, Vector4 bulletColor);
        bool IsOppositeColor(const Vector4& bulletColor);

        void SetNavMesh(NavigationMesh* navMesh) { this->navMesh = navMesh; }
        void SetPlayer(PaintballGameObject* player) { this->player = player; }
        void SetIndicator(PaintballGameObject* sphere) { this->indicatorSphere = sphere; }


        void InitBehaviorTree();
        bool CanSeePlayer();
        bool IsPlayerTooClose() { return distanceToPlayer < fleeDistance; }
        void AttackPlayer() { Attack(enemyColor); }
        void Patrol(float dt);
        void Flee(float dt);

        bool isDead = false;

    protected:
        void CalculatePath(rp3d::Vector3 pos);
        void MoveEnemyAlongPath();
        void Attack(Vector4 color);
        void MoveEnemyAlongPathWithSpeed(float moveSpeed);

        PaintballGameObject* player;
        PaintballGameObject* indicatorSphere;
        NavigationMesh* navMesh;
        std::vector<Vector3> pathNodes;
        rp3d::Vector3 patrolTarget;

        bool canSeeTest;
        float distanceToPlayer = 0.0f;
        float attackRange = 120.0f;
        float fleeDistance = 50.0f;
        float totalCD = 1.0f;
        float leftCD = 0.0f;

        int health = 100;
        Vector4 enemyColor;

        BTNodeCoward* behaviorTree;
    };

    class ConditionCanSeePlayerCoward : public BTNodeCoward {
    private:
        PaintballCoward* owner;
    public:
        ConditionCanSeePlayerCoward(PaintballCoward* owner) : owner(owner) {}
        bool Execute(float dt) override { return owner->CanSeePlayer(); }
    };

    class ConditionIsTooClose : public BTNodeCoward {
    private:
        PaintballCoward* owner;
    public:
        ConditionIsTooClose(PaintballCoward* owner) : owner(owner) {}
        bool Execute(float dt) override { return owner->IsPlayerTooClose(); }
    };

    class ActionAttackPlayerCoward : public BTNodeCoward {
    private:
        PaintballCoward* owner;
    public:
        ActionAttackPlayerCoward(PaintballCoward* owner) : owner(owner) {}
        bool Execute(float dt) override { owner->AttackPlayer(); return true; }
    };

    class ActionPatrolCoward : public BTNodeCoward {
    private:
        PaintballCoward* owner;
    public:
        ActionPatrolCoward(PaintballCoward* owner) : owner(owner) {}
        bool Execute(float dt) override { owner->Patrol(dt); return true; }
    };

    class ActionFleeFromPlayer : public BTNodeCoward {
    private:
        PaintballCoward* owner;
    public:
        ActionFleeFromPlayer(PaintballCoward* owner) : owner(owner) {}
        bool Execute(float dt) override { owner->Flee(dt); return true; }
    };
}
