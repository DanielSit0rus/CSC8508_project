#pragma once
#include "ResourceManager.h"
#include "GameWorld.h"
#include "PaintballGameWorld.h"
#include "PaintballGameObject.h"
#include <reactphysics3d/reactphysics3d.h>
#include "NavigationMesh.h"

namespace NCL {
	namespace CSC8503 {

		class GameManager
		{
            public:
                GameManager(PaintballGameWorld* world);
                ~GameManager() {

                    physicsCommon.destroyPhysicsWorld(RpWorld);
                };

                //void InitWorld();
                rp3d::PhysicsWorld* getRPworld()
                {
                    return RpWorld;
                }
                rp3d::PhysicsCommon& getPhys()
                {
                    return physicsCommon;
                }

                PaintballGameObject* AddPlayer(const rp3d::Vector3& position);
                PaintballGameObject* AddFloorToWorld(const rp3d::Vector3& position);
                PaintballGameObject* AddCube(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
                PaintballGameObject* AddSphere(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
                PaintballGameObject* AddConcaveMesh(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color);
                reactphysics3d::ConcaveMeshShape* CreateConcaveMeshShape(Mesh* mesh);
                void InitDefaultFloor();

            private:
                PaintballGameWorld* world;
                rp3d::PhysicsCommon physicsCommon;
                rp3d::PhysicsWorld* RpWorld = nullptr;
                rp3d::PhysicsWorld::WorldSettings RpSettings;
           
            //NavigationMesh* navMesh;

		};
	}
}
