#include "../NCLCoreClasses/KeyboardMouseController.h"

#pragma once
#include "GameTechRenderer.h"
#ifdef USEVULKAN
#include "GameTechVulkanRenderer.h"
#endif
#include "PhysicsSystem.h"
#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PushdownMachine.h"

#include "PushdownState.h"

#include <reactphysics3d/reactphysics3d.h>

namespace NCL {
	namespace CSC8503 {


		class Goose : public GameObject {
		public:
			Goose(GameObject* player, const std::vector<Vector3>& patrolNodes, GameWorld* world)
				: player(player), patrolNodes(patrolNodes), world(world) {
				SetupStateMachine();
			}
			//void VisualizeRay();

			void Update(float dt) {
				stateMachine->Update(dt);
				VisualizeRay();
			}

		private:
			void SetupStateMachine();
			void Patrol(float dt);
			void ChasePlayer(float dt);
			void VisualizeRay();
			void Raycast();

			StateMachine* stateMachine;
			GameObject* player;
			std::vector<Vector3> patrolNodes;
			GameWorld* world;
			int currentNodeIndex = 0;
			bool isPlayerVisible = false;
		};


		class TutorialGame		{
		public:
			TutorialGame();
			
			bool pauseGame();
			bool UnpauseGame();
			bool getcatstatus() {
				return physics->catded;
			}
			int getscore() {
				return physics->score;
			}
			~TutorialGame();
			bool pause = false;
			virtual void UpdateGame(float dt);

		protected:
			void InitialiseAssets();

			void InitCamera();
			void UpdateKeys();

			
			std::vector<StateGameObject*> kittens;
			/*
			These are some of the world/object creation functions I created when testing the functionality
			in the module. Feel free to mess around with them to see different objects being created in different
			test scenarios (constraints, collision types, and so on). 
			*/
			void InitGameExamples();
			void InitWorld();
			void InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius);
			void InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing);
			void InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims);
			void InitMazeWorld(const std::vector<std::vector<int>>& mazePattern, const Vector3& cubeDims, float spacing);

			void InitDefaultFloor();

			GameObject* AddcylinderToWorld(const Vector3& position);

			StateGameObject* AddStateObjectToWorld(const Vector3& position, GameObject* player);
			StateGameObject * testStateObject;
			void runKittenAI();
			bool SelectObject();
			void MoveSelectedObject();
			void DebugObjectMovement();
			void LockedObjectMovement();
			void AddGate();
			StateMachine* testMachine = new StateMachine();


			void  BridgeConstraintTest();
			

			GameObject* AddFloorToWorld(const Vector3& position);
			GameObject* AddSphereToWorld(const Vector3& position, float radius, float inverseMass = 10.0f);
			GameObject* AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass = 10.0f, Vector4 color = Vector4(1.0f,1.0f,1.0f,1.0f));
			GameObject* playerObject;
			GameObject* DoorFrame; 
			
			GameObject* homecube;
			Goose* GooseObject;
			//StateGameObject* kittenObject1;
			Goose* AddGooseToWorld(const Vector3& position, GameObject* player);
			GameObject* AddKitttenToWorld(const Vector3& position);
			GameObject* AddPlayerToWorld(const Vector3& position);
			GameObject* AddEnemyToWorld(const Vector3& position);
			GameObject* AddBonusToWorld(const Vector3& position);


			void FlipSelectMode();

			//rp3d
			GameObject* AddRp3dCubeToWorld(const Vector3& position, Vector3 dimensions, Quaternion orientation, float inverseMass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			GameObject* AddRp3dObjToWorld(const Vector3& position, Vector3 dimensions, Quaternion orientation, float inverseMass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
			
			
#ifdef USEVULKAN
			GameTechVulkanRenderer*	renderer;
#else
			GameTechRenderer* renderer;
#endif
			PhysicsSystem*		physics;
			GameWorld*			world;

			KeyboardMouseController controller;

			bool useGravity;
			bool inSelectionMode;

			float		forceMagnitude;

			GameObject* selectionObject = nullptr;

			Mesh*	capsuleMesh = nullptr;
			Mesh*	cubeMesh	= nullptr;
			Mesh*	sphereMesh	= nullptr;

			Texture*	basicTex	= nullptr;
			Shader*		basicShader = nullptr;

			//Coursework Meshes
			Mesh*	catMesh		= nullptr;
			Mesh* kittenMesh = nullptr;
			Mesh*	enemyMesh	= nullptr;
			Mesh*	bonusMesh	= nullptr;
			Mesh*	gooseMesh = nullptr;
			Mesh* mapMesh = nullptr;

			//Coursework Additional functionality	
			GameObject* lockedObject	= nullptr;
			Vector3 lockedOffset		= Vector3(0, 14, 20);
			void LockCameraToObject(GameObject* o) {
				lockedObject = o;
			}

			GameObject* objClosest = nullptr;


			//rp3d
			rp3d::PhysicsCommon physicsCommon;
			rp3d::PhysicsWorld* RpWorld = nullptr;
			rp3d::PhysicsWorld::WorldSettings RpSettings;
			vector<GameObject*> objList_rp3d;
		};

	}
}

