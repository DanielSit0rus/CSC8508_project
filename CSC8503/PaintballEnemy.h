#pragma once

#include "StateGameObject.h"
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"
#include "NavigationMesh.h"

namespace NCL {
    namespace CSC8503 {
        class PaintballEnemy : public StateGameObject {
        public:
            PaintballEnemy();
            ~PaintballEnemy();

            void Update(float dt) override;  // ��д update ����

            void SetPlayer(PaintballGameObject* playerObj) { player = playerObj; }
            void SetNavMesh(NavigationMesh* mesh) { navMesh = mesh; }
            void TakeDamage(int damage);


        private:
            void Patrol(float dt);
            void Attack(Vector4 color);
            bool CanSeePlayer();
            void MoveEnemyAlongPath();
            int  health = 100;

            void CalculatePath(rp3d::Vector3 pos);
            //void SetNewPatrolTarget();
            //bool HasReachedTarget();
            void InitBehaviorTree();

            class BTNode {
            public:
                virtual ~BTNode() {}
                virtual bool Execute() = 0;
            };

            class BTSelector : public BTNode {
            public:
                void AddChild(BTNode* child) { children.push_back(child); }
                bool Execute() override {
                    for (BTNode* child : children) {
                        if (child->Execute()) {
                            return true;
                        }
                    }
                    return false;
                }
            private:
                std::vector<BTNode*> children;
            };

            class BTSequence : public BTNode {
            public:
                void AddChild(BTNode* child) { children.push_back(child); }
                bool Execute() override {
                    for (BTNode* child : children) {
                        if (!child->Execute()) {
                            return false;
                        }
                    }
                    return true;
                }
            private:
                std::vector<BTNode*> children;
            };


            class ConditionCanSeePlayer : public BTNode {
            public:
                ConditionCanSeePlayer(PaintballEnemy* bot) : bot(bot) {}
                bool Execute() override {
                    std::cout << "ConditionCanSeePlayer\n";
                    return bot->CanSeePlayer();
                }
            private:
                PaintballEnemy* bot;
            };

            //class ConditionInAttackRange : public BTNode {
            //public:
            //    ConditionInAttackRange(PaintballEnemy* bot, float range) : bot(bot), attackRange(range) {}
            //    bool Execute() override { return bot->GetDistanceToPlayer() <= attackRange; }
            //private:
            //    PaintballEnemy* bot;
            //    float attackRange;
            //};

            class ActionPatrol : public BTNode {
            public:
                ActionPatrol(PaintballEnemy* bot) : bot(bot) {}
                bool Execute() override {
                    std::cout << "ActionPatrol\n";
                    bot->Patrol(0.016f); return true;
                }
            private:
                PaintballEnemy* bot;
            };

            class ActionMoveToPlayer : public BTNode {
            public:
                ActionMoveToPlayer(PaintballEnemy* bot) : bot(bot) {}
                bool Execute() override {
                    std::cout << "ActionMoveToPlayer\n";
                    bot->MoveEnemyAlongPath(); return true;
                }
            private:
                PaintballEnemy* bot;
            };

            class ActionAttackPlayer : public BTNode {
            public:
                ActionAttackPlayer(PaintballEnemy* bot) : bot(bot) {}
                bool Execute() override {
                    std::cout << "ActionAttackPlayer\n";
                    bot->Attack(Vector4(1, 0, 0, 1)); return true;
                }
            private:
                PaintballEnemy* bot;
            };

            BTNode* behaviorTree;
            rp3d::Vector3 patrolTarget;
            PaintballGameObject* player;
            NavigationMesh* navMesh;
            std::vector<Vector3> pathNodes;

            bool canSeeTest = false;

            float totalCD = 2;
            float leftCD = totalCD;
        };

    }
}