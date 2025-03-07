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
			
			bool pauseGame();
			bool UnpauseGame();

			~TutorialGame();
			virtual void UpdateGame(float dt);

			void ShowMainPage();
			void ShowPausedPage();
			void ShowMenuPage();

			//place here for Dev
			void InitWorld();
			void InitCamera();

		protected:
			void InitialiseAssets();

			void UpdateKeys();
			void LockedObjectMovement();

			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			
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

			//FMOD
			PaintballGameObject* speakerObj = nullptr;

			//shoot 
			PaintballPlayer* shoottest = nullptr;
			Enemy* enemy = nullptr;

			InputManager inputManager;

		};


	}
}

