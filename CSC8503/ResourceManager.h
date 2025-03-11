#pragma once
#include"GameTechRenderer.h"
#include"MeshAnimation.h"
#include"MeshMaterial.h"

namespace NCL {
	namespace CSC8503 {
		class ResourceManager
		{
        public:
            static ResourceManager& GetInstance() {
                static ResourceManager instance; // Guaranteed to be initialized once
                return instance;
            }

            void LoadAssets(GameTechRenderer* renderer);
            void ReloadAnimations();

            // Getters for assets
            Mesh* GetCubeMesh() const { return cubeMesh; }
            Mesh* GetSphereMesh() const { return sphereMesh; }
            Mesh* GetCatMesh() const { return catMesh; }
            Mesh* GetKittenMesh() const { return kittenMesh; }
            Mesh* GetFloor1Mesh() const { return floor1Mesh; }
            Mesh* GetMainWallMesh() const { return mainWallMesh; }
            Mesh* GetDoorWayMesh() const { return doorWayMesh; }
            Mesh* GetOppDoorWayMesh() const { return oppDoorWayMesh; }
            Mesh* GetMidDoorMesh() const { return midDoorMesh; }
            Mesh* GetWallMesh() const { return wallMesh; }
            Mesh* GetWall2Mesh() const { return wall2Mesh; }
            Mesh* GetOpenWallMesh() const { return openWallMesh; }
            Mesh* GetWall3Mesh() const { return wall3Mesh; }
            Mesh* GetWall4Mesh() const { return wall4Mesh; }
            Mesh* GetCylinderMesh() const { return cylinderMesh; }
            Mesh* GetBigCylinderMesh() const { return bigCylinderMesh; }
            Mesh* GetTargetMesh() const { return targetMesh; }
            Mesh* GetBigCubeMesh() const { return bigCubeMesh; }
            Mesh* GetFloor2Mesh() const { return floor2Mesh; }
            Mesh* GetMainWall2Mesh() const { return mainWall2Mesh; }
            Mesh* GetGooseMesh() const { return gooseMesh; }
            Mesh* GetEnemyMesh() const { return enemyMesh; }
            Mesh* GetBonusMesh() const { return bonusMesh; }
            Mesh* GetCapsuleMesh() const { return capsuleMesh; }

            Mesh* GetRoleMesh() const { return RoleMesh; }
            std::shared_ptr<MeshAnimation> GetIdleanim() const { return Roleanim; }

            std::shared_ptr<MeshMaterial> GetRolemat() const { return Rolemat; }

            std::shared_ptr<MeshAnimation> GetMoveanim() const { return Moveanim; }
            Texture* GetBasicTex() const { return basicTex; }
            Shader* GetBasicShader() const { return basicShader; }

            private:
                ResourceManager() = default; // Private constructor
                ~ResourceManager() = default;

                // Delete copy constructor and assignment operator
                ResourceManager(const ResourceManager&) = delete;
                ResourceManager& operator=(const ResourceManager&) = delete;

                // Assets
                Mesh* cubeMesh = nullptr;
                Mesh* sphereMesh = nullptr;
                Mesh* catMesh = nullptr;
                Mesh* kittenMesh = nullptr;
                Mesh* floor1Mesh = nullptr;
                Mesh* mainWallMesh = nullptr;
                Mesh* doorWayMesh = nullptr;
                Mesh* oppDoorWayMesh = nullptr;
                Mesh* midDoorMesh = nullptr;
                Mesh* wallMesh = nullptr;
                Mesh* wall2Mesh = nullptr;
                Mesh* openWallMesh = nullptr;
                Mesh* wall3Mesh = nullptr;
                Mesh* wall4Mesh = nullptr;
                Mesh* cylinderMesh = nullptr;
                Mesh* bigCylinderMesh = nullptr;
                Mesh* targetMesh = nullptr;
                Mesh* bigCubeMesh = nullptr;
                Mesh* floor2Mesh = nullptr;
                Mesh* mainWall2Mesh = nullptr;
                Mesh* gooseMesh = nullptr;
                Mesh* enemyMesh = nullptr;
                Mesh* bonusMesh = nullptr;
                Mesh* capsuleMesh = nullptr;

                Mesh* RoleMesh = nullptr;
                std::shared_ptr<MeshAnimation> Roleanim;
                std::shared_ptr<MeshAnimation> Moveanim;
                std::shared_ptr<MeshMaterial> Rolemat;

                Texture* basicTex = nullptr;
                Shader* basicShader = nullptr;
		};
	}
}
