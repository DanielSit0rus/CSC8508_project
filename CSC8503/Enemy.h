#pragma once

#include "PhysicsObject.h"
#include "StateMachine.h"
#include "TutorialGame.h"
#include "NavigationMesh.h"
#include "StateGameObject.h"
//#include "BTNode.h"

namespace NCL {
    namespace CSC8503 {
        class EnemyAI : public StateGameObject {
        public:
            //EnemyAI(NavigationMesh* navMesh);
            EnemyAI();
            ~EnemyAI();

            //void Update(float dt) override;
            void Patrol(float dt);
            void SetPlayer(GameObject* playerObj);
            bool CanSeePlayer();
            void MoveEnemyAlongPath();
            void DisplayPath();
            void CalculatePathToPlayer();

        private:
            void InitFSM();
            //void InitBehaviorTree();

            //BTNode* behaviorTree;
            GameObject* player;
            NavigationMesh* navMesh;
            std::vector<Vector3> pathNodes;
        };
    }
}
