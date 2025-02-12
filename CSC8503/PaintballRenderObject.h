#pragma once
#include "Texture.h"
#include "Shader.h"
#include "Mesh.h"

namespace NCL {
	using namespace NCL::Rendering;

	namespace CSC8503 {
		class PaintballTransform;
		using namespace Maths;

		class PaintballRenderObject
		{
		public:
			PaintballRenderObject(PaintballTransform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader);
			~PaintballRenderObject();

			void SetDefaultTexture(Texture* t) {
				texture = t;
			}

			Texture* GetDefaultTexture() const {
				return texture;
			}

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

		protected:
			Mesh* mesh;
			Texture* texture;
			Shader* shader;
			PaintballTransform* transform;
			Vector4		colour;
		};
	}
}
