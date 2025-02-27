#pragma once
#include "ResourceManager.h"
#include "GameWorld.h"
#include "PaintballGameWorld.h"
#include "PaintballGameObject.h"
#include <reactphysics3d/reactphysics3d.h>
#include "NavigationMesh.h"
#include "PaintballBullet.h"
#include "PaintballPlayer.h"
#include "BulletContactListener.h"

namespace NCL {
    enum class GameState {
        MainMenu,
        InGame,
        Settings,
        Paused,
        GameOver
        // add others if necessary
    };
	namespace CSC8503 {
        class PaintballPlayer; // Ç°ÏòÉùÃ÷ forward declaration
        class GameManager
        {
        public:
            static GameManager& GetInstance() {
                static GameManager instance;
                return instance;
            }

            GameManager(const GameManager&) = delete;
            GameManager& operator=(const GameManager&) = delete;

            ~GameManager() {
                physicsCommon.destroyPhysicsWorld(RpWorld);
            };

            void Init(PaintballGameWorld* world);
            void Update();

            //void InitWorld();
            rp3d::PhysicsWorld* getRPworld() { return RpWorld; }
            rp3d::PhysicsCommon& getPhys() { return physicsCommon; }

            PaintballGameObject* AddPlayer(const rp3d::Vector3& position);
            PaintballGameObject* AddFloorToWorld(const rp3d::Vector3& position);
            PaintballGameObject* AddCube(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            PaintballGameObject* AddSphere(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            PaintballGameObject* AddConcaveMesh(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color);

            PaintballPlayer* AddPlayerClass(rp3d::Vector3 position);

            PaintballGameObject* AddBullet(bool isenemy, const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

            reactphysics3d::ConcaveMeshShape* CreateConcaveMeshShape(Mesh* mesh);

            void DeleteObject(PaintballGameObject* obj) { objectsToDelete.push_back(obj); }

            // GameState management
            void SetGameState(GameState newState) {
                currentState = newState;
                HandleStateChange(newState);
            }

            GameState GetGameState() const {
                return currentState;
            }

            

        private:
            GameManager() = default;
            GameState currentState = GameState::MainMenu;

            void HandleStateChange(GameState state) {
                // Implement logic that should happen on state change
                switch (state) {
                case GameState::MainMenu:
                    // Pause game, show main menu
                    break;
                case GameState::InGame:
                    // Resume or start game
                    break;
                case GameState::Paused:
                    // Pause or toggle game pause
                    break;
                    // Handle other states
                }
            }


            std::vector<PaintballGameObject*> gameObjects;
            PaintballPlayer* player;
            std::vector<PaintballGameObject*> enemies;


            PaintballGameWorld* world = nullptr;
            rp3d::PhysicsCommon physicsCommon;
            rp3d::PhysicsWorld* RpWorld = nullptr;
            rp3d::PhysicsWorld::WorldSettings RpSettings;
            BulletContactListener bulletlistener;

            std::vector<PaintballGameObject*> objectsToDelete;

            //NavigationMesh* navMesh;

        };
	}
}
