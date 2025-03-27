#pragma once
#include "PaintballEnemy.h"
#include "Statemachine.h"
#include "StateGameObject.h"
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"
#include "NavigationMesh.h"
namespace NCL {
    namespace CSC8503 {
        class PaintballCoward : public PaintballEnemy {
        public:
            PaintballCoward(const std::string& name, Vector4 color);
            ~PaintballCoward();
            void Update(float dt) override;
            void InitBehaviorTree();
        protected:
            // Maintain distance from player
            void KeepDistance(float dt);
            // Override parent class methods
            void Chase(float dt);
            void Attack(Vector4 color);
            // Setup state machine
            void SetupStateMachine();
            // Member variables
            float optimalDistance;   // Ideal attack distance
            float minDistance;       // Minimum safe distance
            float maxAttackRange;    // Maximum attack range
            float evadeSpeed;        // Evasion speed
            GameObjectType type;     // Game object type
            // Internal behavior tree node classes
            class ConditionIsThreatened : public BTNode {
            public:
                ConditionIsThreatened(PaintballCoward* owner) : owner(owner) {}
                bool Execute() override {
                    if (!owner || !owner->player) return false;
                    // If player is visible and too close
                    if (owner->canSeeTest && owner->distanceToPlayer < owner->minDistance) {
                        return true;
                    }
                    return false;
                }
            private:
                PaintballCoward* owner;
            };
            class ConditionInAttackRange : public BTNode {
            public:
                ConditionInAttackRange(PaintballCoward* owner) : owner(owner) {}
                bool Execute() override {
                    if (!owner || !owner->player) return false;
                    // If player is visible and at suitable distance (not too close or too far)
                    if (owner->distanceToPlayer <= owner->maxAttackRange &&
                        owner->distanceToPlayer >= owner->minDistance) {
                        return true;
                    }
                    return false;
                }
            private:
                PaintballCoward* owner;
            };
            class ActionMaintainDistance : public BTNode {
            public:
                ActionMaintainDistance(PaintballCoward* owner) : owner(owner) {}
                bool Execute() override {
                    if (!owner || !owner->player) return false;
                    owner->KeepDistance(0.016f);
                    return true;
                }
            private:
                PaintballCoward* owner;
            };
            class ActionAttackFromDistance : public BTNode {
            public:
                ActionAttackFromDistance(PaintballCoward* owner) : owner(owner) {}
                bool Execute() override {
                    if (!owner || !owner->player) return false;
                    owner->Attack(owner->enemyColor);
                    return true;
                }
            private:
                PaintballCoward* owner;
            };
            // Using parent class condition nodes (for behavior tree construction)
            using ConditionCanSeePlayer = PaintballEnemy::ConditionCanSeePlayer;
            using ActionPatrol = PaintballEnemy::ActionPatrol;
        };
    }
}