#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif

#include <reactphysics3d/reactphysics3d.h>
#include "PaintballGameObject.h"
#include "Util.h"

namespace NCL {
	namespace CSC8503 {

		class RaycastHitCallback : public rp3d::RaycastCallback {
		public:
			rp3d::RigidBody* rb = nullptr;
			rp3d::Vector3 hitpoint;
			
			rp3d::decimal notifyRaycastHit(const rp3d::RaycastInfo& info) override {
				rb = dynamic_cast<rp3d::RigidBody*>(info.body);
				hitpoint = info.worldPoint;
				return info.hitFraction;
			}
		};

		class TutorialGame		{
		public:
			TutorialGame();
			
			bool pauseGame();
			bool UnpauseGame();

			~TutorialGame();
			bool pause = false;
			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitWorld();
			
			void InitDefaultFloor();			

			PaintballGameObject* playerObject;
			PaintballGameObject* AddPlayerToWorld(const rp3d::Vector3& position);

			PaintballGameObject* AddFloorToWorld(const rp3d::Vector3& position);

			//rp3d
			PaintballGameObject* AddRp3dCubeToWorld(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			PaintballGameObject* AddRp3dObjToWorld(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			PaintballGameObject* AddRp3dConcaveToWorld(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color);

			reactphysics3d::ConcaveMeshShape* CreateConcaveMeshShape(Mesh* mesh);

#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			GameWorld*			world;

			KeyboardMouseController controller;

			float		forceMagnitude;

			Mesh*	capsuleMesh = nullptr;
			Mesh*	cubeMesh	= nullptr;
			Mesh*	sphereMesh	= nullptr;

			Texture*	basicTex	= nullptr;
			Shader*		basicShader = nullptr;

			//Coursework Meshes
			Mesh*	catMesh		= nullptr;
			Mesh*	kittenMesh	= nullptr;
			Mesh*   mapMesh     = nullptr;
			Mesh*	enemyMesh	= nullptr;
			Mesh*	bonusMesh	= nullptr;
			Mesh*	gooseMesh = nullptr;

			//rp3d
			rp3d::PhysicsCommon physicsCommon;
			rp3d::PhysicsWorld* RpWorld = nullptr;
			rp3d::PhysicsWorld::WorldSettings RpSettings;
			vector<PaintballGameObject*> objList_pb;

			PaintballGameObject* selectionObject = nullptr;
			PaintballGameObject* lockedObject = nullptr;
		};

	}
}

