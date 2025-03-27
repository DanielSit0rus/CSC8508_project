#pragma once
#include "StateGameObject.h"
#include "NavigationMesh.h"
#include "NavigationPath.h"

using namespace NCL::Maths;

namespace NCL::CSC8503 {
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
        void MoveEnemyAlongPathWithSpeed(float moveSpeed);

        bool isDead = false;

    protected:
        void Patrol(float dt);
        void Attack(Vector4 color);
        void Flee(float dt);
        bool CanSeePlayer();
        void CalculatePath(rp3d::Vector3 pos);
        void MoveEnemyAlongPath();

        PaintballGameObject* player = nullptr;
        PaintballGameObject* indicatorSphere = nullptr;
        NavigationMesh* navMesh = nullptr;
        std::vector<Vector3> pathNodes;
        rp3d::Vector3 patrolTarget;

        bool canSeeTest = false;
        float distanceToPlayer = 0.0f;
        float attackRange = 120.0f;
        float fleeDistance = 50.0f;

        float totalCD = 1.5f;
        float leftCD = 0.0f;

        int health = 3;
        Vector4 enemyColor;
    };
}