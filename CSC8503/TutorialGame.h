#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif

#include <iostream>
#include <algorithm>

#include <reactphysics3d/reactphysics3d.h>
#include "PaintballGameWorld.h"
#include "PaintballGameObject.h"
#include "Util.h"
#include "NavigationMesh.h"

#include <fmod.h>
#include "AudioSystem.h"

namespace NCL {
	namespace CSC8503 {
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
			void LockedObjectMovement();

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
			PaintballGameWorld*			world;

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

			PaintballGameObject* selectionObject = nullptr;
			PaintballGameObject* lockedObject = nullptr;

			//rp3d
			rp3d::PhysicsCommon physicsCommon;
			rp3d::PhysicsWorld* RpWorld = nullptr;
			rp3d::PhysicsWorld::WorldSettings RpSettings;
			vector<PaintballGameObject*> objList_pb;


			//FMOD
			FMOD_3D_ATTRIBUTES* listenerAttributes = nullptr;
			PaintballGameObject* speakerObj = nullptr;

			NavigationMesh navMesh; 
		};

	}
}

