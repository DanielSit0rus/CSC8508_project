#pragma once

#include "StateMachine.h"
#include "NavigationMesh.h"
#include "StateGameObject.h"
//#include "BTNode.h"

namespace NCL {
    namespace CSC8503 {
        class Enemy : public StateGameObject {
        public:
            //EnemyAI(NavigationMesh* navMesh);
            Enemy();
            ~Enemy();

            //void Update(float dt) override;
            void Patrol(float dt);
            void SetPlayer(PaintballGameObject* playerObj);
            bool CanSeePlayer();
            void MoveEnemyAlongPath();
            void DisplayPath();
            void CalculatePathToPlayer();

        private:
            void InitFSM();
            //void InitBehaviorTree();

            //BTNode* behaviorTree;
            PaintballGameObject* player;
            NavigationMesh* navMesh;
            std::vector<Vector3> pathNodes;
        };
    }
}
