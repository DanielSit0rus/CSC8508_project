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

            void Update(float dt) override;  // ÷ÿ–¥ update ∑Ω∑®

            void SetPlayer(PaintballGameObject* playerObj) { player = playerObj; }
            void SetNavMesh(NavigationMesh* mesh) { navMesh = mesh; }

        private:
            void Patrol(float dt);
            void Attack(Vector3 front, Vector4 color);
            bool CanSeePlayer();
            void MoveEnemyAlongPath();
            void CalculatePath(rp3d::Vector3 pos);
            void SetNewPatrolTarget();
            bool HasReachedTarget();
            
            rp3d::Vector3 patrolTarget;
            PaintballGameObject* player;
            NavigationMesh* navMesh;
            std::vector<Vector3> pathNodes;

            bool canSeeTest = false;
        };

    }
}