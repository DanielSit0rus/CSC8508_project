#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif

#include <iostream>
#include <algorithm>
#include<string>
#include <reactphysics3d/reactphysics3d.h>
#include "PaintballGameWorld.h"
#include "PaintballGameObject.h"
#include "Util.h"
#include "NavigationMesh.h"
#include "ResourceManager.h"
#include "GameManager.h"

#include <fmod.h>
#include "AudioSystem.h"
#include "PaintballPlayer.h"
#include "Enemy.h"
#include "InputManager.h"

namespace NCL {
	namespace CSC8503 {
		class Enemy;
		class TutorialGame		{
		public:
			TutorialGame();
		/*	bool gameStarted = false;*/
			bool pauseGame();
			bool UnpauseGame();

			~TutorialGame();
			bool pause = false;
			virtual void UpdateGame(float dt);

			static TutorialGame* GetGame() {
				return instance;
			};

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();
			void LockedObjectMovement();

			void AssetsLoading();
			int assetsLoadedStep = 0;
			void UpdateLoading(float dt);
			virtual void UpdatePaused(float dt);
			virtual void UpdateFailure(float dt);
			virtual void UpdateFinish(float dt);
			void UpdateMenu(float dt);
			void UpdateSetting(float dt);

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitWorld();
			
			void InitDefaultFloor();			

			PaintballPlayer* playerObject;

			void CalculatePathToPlayer();

			void DisplayPath();

			void MoveEnemyAlongPath();

			//reactphysics3d::ConcaveMeshShape* CreateConcaveMeshShape(Mesh* mesh);

#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PaintballGameWorld*			world;
			GameManager& G1 = GameManager::GetInstance();

			KeyboardMouseController controller;

			float		forceMagnitude;

			PaintballGameObject* selectionObject = nullptr;
			PaintballGameObject* lockedObject = nullptr;
			PaintballGameObject* enemyObject = nullptr;
			PaintballGameObject* CharacterObject = nullptr;
			NavigationMesh* navMesh;
			std::vector<Vector3> pathNodes;

			//rp3d
			
			vector<PaintballGameObject*> objList_pb;


			//FMOD
			FMOD_3D_ATTRIBUTES* listenerAttributes = nullptr;
			PaintballGameObject* speakerObj = nullptr;

			//shoot 
			PaintballPlayer* shoottest = nullptr;
			Enemy* enemy = nullptr;

			InputManager inputManager;

			static TutorialGame* instance;
		};


	}
}

