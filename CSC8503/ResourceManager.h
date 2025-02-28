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

            // Getters for assets
            Mesh* GetCubeMesh() const { return cubeMesh; }
            Mesh* GetSphereMesh() const { return sphereMesh; }
            Mesh* GetCatMesh() const { return catMesh; }
            Mesh* GetKittenMesh() const { return kittenMesh; }
            Mesh* GetMapMesh() const { return mapMesh; }
            Mesh* GetGooseMesh() const { return gooseMesh; }
            Mesh* GetEnemyMesh() const { return enemyMesh; }
            Mesh* GetBonusMesh() const { return bonusMesh; }
            Mesh* GetCapsuleMesh() const { return capsuleMesh; }

            Mesh* GetRoleMesh() const { return RoleMesh; }
            MeshAnimation* GetRoleanim() const { return Roleanim; }
            MeshMaterial* GetRolemat() const { return Rolemat; }

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
                Mesh* mapMesh = nullptr;
                Mesh* gooseMesh = nullptr;
                Mesh* enemyMesh = nullptr;
                Mesh* bonusMesh = nullptr;
                Mesh* capsuleMesh = nullptr;

                Mesh* RoleMesh = nullptr;
                MeshAnimation* Roleanim;
                MeshMaterial* Rolemat;

                Texture* basicTex = nullptr;
                Shader* basicShader = nullptr;
		};
	}
}
