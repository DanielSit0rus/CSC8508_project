#include <queue>
#include "PaintballBullet.h"

namespace NCL::CSC8503 {
    class PaintballBullet;
    class BulletPool {
    public:
        BulletPool(size_t initialSize, rp3d::PhysicsWorld* world)
            : physicsWorld(world) {
            for (size_t i = 0; i < initialSize; ++i) {
                PaintballBullet* bullet = CreateNewBullet();
                availableBullets.push(bullet);
            }
        }

        ~BulletPool() {
            while (!availableBullets.empty()) {
                delete availableBullets.front();
                availableBullets.pop();
            }
        }

        PaintballBullet* GetBullet() {
            if (availableBullets.empty()) {
                return CreateNewBullet(); // ‘ –Ì≥ÿ¿©’π
            }
            PaintballBullet* bullet = availableBullets.front();
            availableBullets.pop();
            return bullet;
        }

        void ReturnBullet(PaintballBullet* bullet) {
            bullet->SetActive(false); // Õ£÷π‰÷»æ°¢≈ˆ◊≤ºÏ≤‚µ»
            availableBullets.push(bullet);
        }

    private:
        PaintballBullet* CreateNewBullet() {
            PaintballBullet* bullet = new PaintballBullet("bullet");
            rp3d::RigidBody* body = physicsWorld->createRigidBody(rp3d::Transform::identity());
            bullet->SetPhysicsObject(new PaintballPhysicsObject(&bullet->GetTransform(), *body, *physicsWorld));
            bullet->SetActive(false); // œ»Ω˚”√
            return bullet;
        }

        std::queue<PaintballBullet*> availableBullets;
        rp3d::PhysicsWorld* physicsWorld;
    };
}