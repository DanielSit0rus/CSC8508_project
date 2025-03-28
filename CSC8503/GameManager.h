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
#include "PaintballEnemy.h"
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
        class PaintballEnemy; //forward declaration
        class PaintballBullet;
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


            void SetDyingEnemyPosition(const Vector3& pos) {
                dyingEnemyPosition = pos;
                hasDyingEnemy = true;
            }
            
            bool HasDyingEnemy() const { return hasDyingEnemy; }
            const Vector3& GetDyingEnemyPosition() const { return dyingEnemyPosition; }

            void ClearDyingEnemy() { hasDyingEnemy = false; }

            void Init(PaintballGameWorld* world, GameTechRenderer* renderer, Controller* c, float gameTime = 120);
            void Update(float dt);
            void PostCleanUp();

            rp3d::PhysicsWorld* getRPworld() { return RpWorld; }
            rp3d::PhysicsCommon& getPhysCommon() { return physicsCommon; }
            bool isPhysEnabled() const { return hasPhys; }
            void EnablePhys(bool arg) { hasPhys = arg; }

            bool GetNetwork() const { return hasNetwork; }
            void SetNet(bool arg) { hasNetwork = arg; }

            void ShowExitScreen();

            void CleanWorld();
            void InitWorld(int arg);
            void ContinueLevelFlow(bool isNext = false);
            void InitWorld_Map1();
            void InitWorld_Map2();

            void IncreaseEnemyCount() { enemyCount++; }
            void DecreaseEnemyCount() { enemyCount--; }
            int GetEnemyCount() const;

            Vector4 GetRequiredBulletColor(const Vector4& enemyColor);

            PaintballGameObject* AddObject(GameObjectType type, const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation,
                Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f),
                const string& meshName = "",
                const string& textureNameD = "", const string& textureNameS = "", const string& textureNameN = "",
                const string& textureNameH = "", const string& textureNameM = "", const string& textureNameO = "", const string& textureNameMM = "", const string& textureNameR = "",
                const string& shaderName = "basic",
                float mass = 10.0f, bool isEnemy = false, rp3d::Vector3 oriV3 = rp3d::Vector3(),
                int network = -1);

            PaintballGameObject* AddFloorToWorld(const rp3d::Vector3& position);
            PaintballGameObject* AddCube(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            PaintballGameObject* AddSphere(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));
            PaintballGameObject* AddConcaveMesh(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation,
                const string& meshName, const string& textureName = "basic", const string& shaderName = "basic", Vector4 color = Vector4(1, 1, 1, 1));

            PaintballGameObject* AddConcaveMesh(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation,
                const string& meshName, const string& diffuseTexture, const string& specularTexture, const string& normalTexture, const string& shaderName, Vector4 color = Vector4(1, 1, 1, 1));


            PaintballGameObject* AddConcaveMesh(
                const rp3d::Vector3& position,
                rp3d::Vector3 dimensions,
                rp3d::Quaternion orientation,
                const std::string& meshName,
                const std::string& diffuseTexture,
                const std::string& specularTexture,   // No default
                const std::string& normalTexture,     // No default
                const std::string& heightTexture,     // No default
                const std::string& metallicTexture,   // No default
                const std::string& occlusionTexture,  // No default
                const std::string& maskMapTexture,    // No default
                const std::string& roughnessTexture,  // No default
                const std::string& shaderName,
                Vector4 color = Vector4(1, 1, 1, 1))
                ;



            PaintballGameObject* Addcharacter(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass = 10.0f, Vector4 color = Vector4(1.0f, 1.0f, 1.0f, 1.0f));

            PaintballPlayer* AddPlayerClass(rp3d::Vector3 position);
            PaintballEnemy* AddEnemyClass(rp3d::Vector3 position);

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
            void ResetEnemyCount();
			Controller* GetController() { return controller; }

            PaintballGameWorld* GetWorld() { return world; }

            void SetLeftTime(float t) { leftTime = t; }
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

            bool isLoading = true;

            bool isView3 = false;

        private:
            GameManager() = default;

            float totalTime;
            float leftTime;
            bool hasNetwork = false;

			Controller* controller;

            //NEW
            std::vector<PaintballGameObject*> gameObjects;
            PaintballPlayer* player;

            GameTechRenderer* renderer;

            bool hasPhys = true;
            PaintballGameWorld* world = nullptr;
            rp3d::PhysicsCommon physicsCommon;
            rp3d::PhysicsWorld* RpWorld = nullptr; // not used anymore
            rp3d::PhysicsWorld::WorldSettings RpSettings;
            BulletContactListener bulletlistener;

            std::unordered_set<PaintballGameObject*> objectsToDelete;

            int enemyCount = 0;

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
            Vector3 dyingEnemyPosition;
            bool hasDyingEnemy;

            int curLevel = 1;
        };
    }
}
