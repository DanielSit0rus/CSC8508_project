#pragma once
#include "ResourceManager.h"
#include "GameWorld.h"
#include"PaintballGameWorld.h"
#include "PaintballGameWorld.h"
#include "PaintballGameObject.h"
#include <reactphysics3d/reactphysics3d.h>
#include "NavigationMesh.h"
#include "PaintballBullet.h"
#include "PaintballPlayer.h"
#include "BulletContactListener.h"
#include "Camera.h"
#include <unordered_set>

namespace NCL {
    const UINT16 PLAYER = 0x0001;   // 0000 0000 0000 0001
    const UINT16 ENEMY = 0x0002;    // 0000 0000 0000 0010
    const UINT16 BULLET = 0x0004;   // 0000 0000 0000 0100

    enum PaintballGameState {
        LOADING,
        PLAYING,
        SERVERPLAYING,
        CLIENTPLAYING,
        PAUSED,
        FAILURE,
        FINISH,
        MENU,
        SETTING,
        CHOOSESERVER,
        EXIT
    };//imgui gamestate

    namespace CSC8503 {
        class PaintballPlayer;
        class PaintballBullet;
        class PaintballEnemy; //forward declaration
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

            void Init(PaintballGameWorld* world, GameTechRenderer* renderer, float gameTime = 120);
            void Update(float dt);
            void PostCleanUp();

            rp3d::PhysicsWorld* getRPworld() { return RpWorld; }
            rp3d::PhysicsCommon& getPhysCommon() { return physicsCommon; }
            bool isPhysEnabled() const { return hasPhys; }
            void EnablePhys(bool arg) { hasPhys = arg; }

            bool GetNetwork() const { return hasNetwork; }
            void SetNet(bool arg) { hasNetwork = arg; }

            void CleanWorld();
            void InitWorld();
            void InitWorld(int arg);

            PaintballGameObject* AddObject(GameObjectType type, const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation,
                Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f), const string& meshName = "", const string& textureName = "basic", const string& shaderName = "basic",
                float mass = 10.0f, bool isEnemy = false, rp3d::Vector3 oriV3 = rp3d::Vector3(),
                int network = -1);

            PaintballGameObject* AddFloorToWorld(const rp3d::Vector3& position);
            PaintballGameObject* AddCube(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            PaintballGameObject* AddSphere(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            PaintballGameObject* AddConcaveMesh(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation,
                const string& meshName, const string& textureName = "basic", const string& shaderName = "basic", Vector4 color = Vector4(1, 1, 1, 1));

            PaintballGameObject* Addcharacter(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

            PaintballPlayer* AddPlayerClass(rp3d::Vector3 position);

            PaintballBullet* AddBullet(rp3d::Vector3 ori3, bool isenemy, const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, Vector4 color, float mass = 10.0f);
            reactphysics3d::ConcaveMeshShape* CreateConcaveMeshShape(Mesh* mesh);


            PaintballGameObject* AddTrigger(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            PaintballGameObject* AddTrap();


            void DeleteObject(PaintballGameObject* obj) {
                if (obj->GetNetworkObject()) {
                    obj->GetNetworkObject()->toDelete = true;
                }

                objectsToDelete.insert(obj);
            }

            PaintballPlayer* GetPlayer() const {
                return player;
            }

            void SetPlayer(PaintballPlayer* p) {
                player = p;
            }

            PerspectiveCamera& GetMainCamera() {
                return world->GetMainCamera();
            }
            Vector3 GetCameraFront();


            PaintballGameWorld* GetWorld() { return world; }

            float GetLeftTime() const { return leftTime; }

            std::map<int, NetworkObject*>& GetNetworkObjects() { return networkObjects; }
            std::map<int, PaintballPlayer*>& GetNetworkPlayers() { return networkPlayers; }
            int& GetThisPeer() { return thisPeer; }

            void RequestRebuildWorld(int arg) { toRebuild = arg; }

            void SetGameState(PaintballGameState state);
            PaintballGameState GetGameState() const { return curState; };


            //Pointers - to be private later
            PaintballPlayer* playerObject;
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
            PaintballEnemy* enemy = nullptr;
            //test
            float		forceMagnitude;

        private:
            GameManager() = default;

            float totalTime;
            float leftTime;
            bool hasNetwork = false;

            //NEW
            std::vector<PaintballGameObject*> gameObjects;
            PaintballPlayer* player;
            std::vector<PaintballGameObject*> enemies;

            GameTechRenderer* renderer;

            bool hasPhys = true;
            PaintballGameWorld* world = nullptr;
            rp3d::PhysicsCommon physicsCommon;
            rp3d::PhysicsWorld* RpWorld = nullptr; // not used anymore
            rp3d::PhysicsWorld::WorldSettings RpSettings;
            BulletContactListener bulletlistener;

            std::unordered_set<PaintballGameObject*> objectsToDelete;



            //NavigationMesh* navMesh;

            //FMOD
            bool canStart_FMOD = true;

            int thisPeer = -1;
            std::map<int, NetworkObject*> networkObjects;
            std::map<int, PaintballPlayer*> networkPlayers;

            int toRebuild = -1;

            Window* w = nullptr;
            PaintballGameState curState;
            PaintballGameState lastState = LOADING;
        };
    }
}
