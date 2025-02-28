#pragma once
#include "PaintballGameObject.h"

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject : public PaintballGameObject {
        public:
            StateGameObject();
            ~StateGameObject();

            virtual void Update(float dt);
            void SetEnemy(PaintballGameObject* enemyObject = nullptr);
            bool CanSeePlayer();
            void MoveEnemyAlongPath();
            //float DistanceToPlayer();

        protected:
            void Patrol(float dt);
            void AttackPlayer(float dt);

            float counter;
            StateMachine* stateMachine;
            PaintballGameObject* playerObj;
            PaintballGameObject* enemyObject;
            rp3d::PhysicsWorld* RpWorld = nullptr;
        };
    }
}
