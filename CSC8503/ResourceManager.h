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
            Mesh* GetMesh(const std::string& name, bool debug = true);
            Texture* GetTexture(const std::string& name, bool debug = true);
            Shader* GetShader(const std::string& name, bool debug = true);

            Shader* GetBasicShader();

            std::shared_ptr<MeshAnimation> GetIdleanim() const { return Roleanim; }

            std::shared_ptr<MeshMaterial> GetRolemat() const { return Rolemat; }

            std::shared_ptr<MeshAnimation> GetMoveanim() const { return Moveanim; }


            private:
                ResourceManager() = default; // Private constructor
                ~ResourceManager() = default;

                // Delete copy constructor and assignment operator
                ResourceManager(const ResourceManager&) = delete;
                ResourceManager& operator=(const ResourceManager&) = delete;

                // Assets

                std::shared_ptr<MeshAnimation> Roleanim;
                std::shared_ptr<MeshAnimation> Moveanim;
                std::shared_ptr<MeshMaterial> Rolemat;

                std::unordered_map<std::string, Mesh*> meshes;
                std::unordered_map<std::string, Texture*> textures;
                std::unordered_map<std::string, Shader*> shaders;
		};
	}
}
