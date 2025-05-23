#pragma once
#include "OGLRenderer.h"
#include "OGLShader.h"
#include "OGLTexture.h"
#include "OGLMesh.h"
#include"UI.h"
#include "PaintballGameWorld.h"
#include <vector>
#include "Light.h"

namespace NCL {
	namespace CSC8503 {
		class RenderObject;

		class GameTechRenderer : public OGLRenderer, ISaveable {
		public:
			GameTechRenderer(PaintballGameWorld& world);
			~GameTechRenderer();
			void AddLight(const Light& light);

			Mesh* LoadMesh(const std::string& name);
			Texture* LoadTexture(const std::string& name);
			Shader* LoadShader(const std::string& vertex, const std::string& fragment);

			void		LoadMesh(std::unordered_map<std::string, Mesh*>& meshMap, const std::string& key, const std::string& filename);
			void		LoadTexture(std::unordered_map<std::string, Texture*>& textureMap, const std::string& key, const std::string& filename);
			void		LoadShader(std::unordered_map<std::string, Shader*>& shaderMap, const std::string& key, const std::string& vertex, const std::string& fragment);

			void UpdateLight(int index, const Vector3& position, const Vector3& direction);

			UI* GetUI() { return ui; };

			void SaveData(json& j);
			void LoadData(json& j);

		protected:
			void NewRenderLines();
			void NewRenderText();
			void NewRenderTextures();

			void RenderFrame()	override;
			void DebugShadowFrustum(const Matrix4& shadowProjMatrix, const Matrix4& shadowViewMatrix);


			OGLShader* defaultShader;

			PaintballGameWorld& gameWorld;
			UI* ui;

			void BuildObjectList();
			void SortObjectList();
			void RenderShadowMap();
			void RenderShadowMap2();
			void RenderCamera();
			void RenderSkybox();

			void LoadSkybox();

			void SetDebugStringBufferSizes(size_t newVertCount);
			void SetDebugLineBufferSizes(size_t newVertCount);

			vector<const PaintballRenderObject*> activeObjects;

			OGLShader* debugShader;
			OGLShader* skyboxShader;
			OGLShader* animShader;
			OGLMesh* skyboxMesh;
			OGLMesh* debugTexMesh;
			GLuint		skyboxTex;

			//shadow mapping things
			OGLShader* shadowShader;
			GLuint		shadowTex;
			GLuint		shadowFBO;
			Matrix4     shadowMatrix;

			Vector4		lightColour;
			float		lightRadius;
			Vector3		lightPosition;

			//Debug data storage things
			vector<Vector3> debugLineData;

			vector<Vector3> debugTextPos;
			vector<Vector4> debugTextColours;
			vector<Vector2> debugTextUVs;

			std::vector<Light> lights;


			GLuint lineVAO;
			GLuint lineVertVBO;
			size_t lineCount;

			GLuint textVAO;
			GLuint textVertVBO;
			GLuint textColourVBO;
			GLuint textTexVBO;
			size_t textCount;

			json tempSave;
		};
	}
}

