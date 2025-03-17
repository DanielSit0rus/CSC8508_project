#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"
#include"MeshAnimation.h"
#include"MeshMaterial.h"

namespace NCL {
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class PaintballTransform;
		using namespace Maths;

		class PaintballRenderObject
		{
		public:
			PaintballRenderObject(PaintballTransform* parentTransform, std::string meshName, std::string textureName, std::string shaderName,
				std::shared_ptr<MeshAnimation> animation = nullptr, std::shared_ptr<MeshMaterial> material = nullptr);
			~PaintballRenderObject();

			void SetDefaultTexture(const std::string& t);
			void SetSpecularTexture(const std::string& t);
			void SetNormalTexture(const std::string& t);
			void SetHeightTexture(const std::string& t);
			void SetMetallicTexture(const std::string& t);
			void SetOcclusionTexture(const std::string& t);
			void SetMaskMapTexture(const std::string& t);
			void SetRoughnessTexture(const std::string& t);



			Texture* GetDefaultTexture() const {
				return texture;
			}

			// New: Setters for specular and normal maps

			

			// Getters for specular and normal maps
			Texture* GetSpecularTexture() const { return specularTexture; }
			Texture* GetNormalTexture() const { return normalTexture; }
			Texture* GetHeightTexture() const { return heightTexture; }
			Texture* GetMetallicTexture() const { return metallicTexture; }
			Texture* GetOcclusionTexture() const { return occlusionTexture; }
			Texture* GetMaskMapTexture() const { return maskMapTexture; }
			Texture* GetRoughnessTexture() const { return roughnessTexture; }


			Texture* GetTexture() const { return texture; }  // Still returns the diffuse texture
		

			Mesh* GetMesh() const {
				return mesh;
			}

			PaintballTransform* GetTransform() const {
				return transform;
			}

			Shader* GetShader() const {
				return shader;
			}

			void SetColour(const Vector4& c) {
				colour = c;
			}

			Vector4 GetColour() const {
				return colour;
			}

			void UpdateAnimation(float dt);

			std::shared_ptr<MeshAnimation> GetAnimation() const {
				return animation;
			}

			void SetAnimation(std::shared_ptr<MeshAnimation> a) {
				animation = a;
			}

			int GetCurrentFrame() const {
				return currentFrame;
			}

			std::string GetMeshName() { return meshName; }
			std::string GetDefaultTextureName() { return textureName; }
			std::string GetSpecularTextureName() { return specularTextureName; }
			std::string GetNormalTextureName() { return normalTextureName; }
			std::string GetShaderName() { return shaderName; }

		protected:

			std::string meshName;
			std::string textureName;
			std::string specularTextureName;
			std::string normalTextureName;
			std::string heightTextureName;
			std::string metallicTextureName;
			std::string occlusionTextureName;
			std::string maskMapTextureName;
			std::string roughnessTextureName;
			std::string shaderName;

			Mesh* mesh;
			Texture* texture; // diffuse
			Texture* specularTexture = nullptr;
			Texture* normalTexture = nullptr;
			Texture* heightTexture = nullptr;
			Texture* metallicTexture = nullptr;
			Texture* occlusionTexture = nullptr;
			Texture* maskMapTexture = nullptr;
			Texture* roughnessTexture = nullptr;

			Shader* shader;
			PaintballTransform* transform;
			Vector4		colour;

			std::shared_ptr<MeshAnimation> animation;
			std::shared_ptr<MeshMaterial> material;
			int currentFrame;
			float frameTime;
		};
	}
}
