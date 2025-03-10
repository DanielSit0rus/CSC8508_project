#include "GameTechRenderer.h"
#include "GameObject.h"
#include "RenderObject.h"
#include "Camera.h"
#include "TextureLoader.h"
#include "MshLoader.h"
using namespace NCL;
using namespace Rendering;
using namespace CSC8503;

#define SHADOWSIZE 4096

Matrix4 biasMatrix = Matrix::Translation(Vector3(0.5f, 0.5f, 0.5f)) * Matrix::Scale(Vector3(0.5f, 0.5f, 0.5f));

GameTechRenderer::GameTechRenderer(PaintballGameWorld& world) : OGLRenderer(*Window::GetWindow()), gameWorld(world)	{
	glEnable(GL_DEPTH_TEST);

	debugShader  = new OGLShader("debug.vert", "debug.frag");
	shadowShader = new OGLShader("shadow.vert", "shadow.frag");
	animShader = new OGLShader("anim.vert", "anim.frag");

	Light light1(Vector3(-10, 50, 0), Vector3(0, -1, 0), Vector4(1, 1, 1, 1), 1000.0f,50.0f);
	AddLight(light1);
	glGenTextures(1, &shadowTex);
	glBindTexture(GL_TEXTURE_2D, shadowTex);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT,
			     SHADOWSIZE, SHADOWSIZE, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE, GL_COMPARE_R_TO_TEXTURE);
	glBindTexture(GL_TEXTURE_2D, 0);

	glGenFramebuffers(1, &shadowFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D, shadowTex, 0);
	glDrawBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClearColor(1, 1, 1, 1);

	//Set up the light properties
	lightColour = Vector4(0.8f, 0.8f, 0.5f, 1.0f);
	lightRadius = 1000.0f;
	lightPosition = light1.GetPosition();

	//Skybox!
	skyboxShader = new OGLShader("skybox.vert", "skybox.frag");
	skyboxMesh = new OGLMesh();
	skyboxMesh->SetVertexPositions({Vector3(-1, 1,-1), Vector3(-1,-1,-1) , Vector3(1,-1,-1) , Vector3(1,1,-1) });
	skyboxMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	skyboxMesh->UploadToGPU();

	LoadSkybox();

	glGenVertexArrays(1, &lineVAO);
	glGenVertexArrays(1, &textVAO);

	glGenBuffers(1, &lineVertVBO);
	glGenBuffers(1, &textVertVBO);
	glGenBuffers(1, &textColourVBO);
	glGenBuffers(1, &textTexVBO);

	Debug::CreateDebugFont("PressStart2P.fnt", *LoadTexture("PressStart2P.png"));

	//Debug quad for drawing tex
	debugTexMesh = new OGLMesh();
	debugTexMesh->SetVertexPositions({ Vector3(-1, 1,0), Vector3(-1,-1,0) , Vector3(1,-1,0) , Vector3(1,1,0) });
	debugTexMesh->SetVertexTextureCoords({ Vector2(0, 1), Vector2(0,0) , Vector2(1,0) , Vector2(1,1) });
	debugTexMesh->SetVertexIndices({ 0,1,2,2,3,0 });
	debugTexMesh->UploadToGPU();


	SetDebugStringBufferSizes(10000);
	SetDebugLineBufferSizes(1000);
}

GameTechRenderer::~GameTechRenderer()	{
	glDeleteTextures(1, &shadowTex);
	glDeleteFramebuffers(1, &shadowFBO);
}

void GameTechRenderer::LoadSkybox() {
	std::string filenames[6] = {
		"/Cubemap/skyrender0004.png",
		"/Cubemap/skyrender0001.png",
		"/Cubemap/skyrender0003.png",
		"/Cubemap/skyrender0006.png",
		"/Cubemap/skyrender0002.png",
		"/Cubemap/skyrender0005.png"
	};

	uint32_t width[6]	 = { 0 };
	uint32_t height[6]	 = { 0 };
	uint32_t channels[6] = { 0 };
	int flags[6]		 = { 0 };

	vector<char*> texData(6, nullptr);

	for (int i = 0; i < 6; ++i) {
		TextureLoader::LoadTexture(filenames[i], texData[i], width[i], height[i], channels[i], flags[i]);
		if (i > 0 && (width[i] != width[0] || height[0] != height[0])) {
			std::cout << __FUNCTION__ << " cubemap input textures don't match in size?\n";
			return;
		}
	}
	glGenTextures(1, &skyboxTex);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	GLenum type = channels[0] == 4 ? GL_RGBA : GL_RGB;

	for (int i = 0; i < 6; ++i) {
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, width[i], height[i], 0, type, GL_UNSIGNED_BYTE, texData[i]);
	}

	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
}

void GameTechRenderer::RenderFrame() {
	glEnable(GL_CULL_FACE);
	glClearColor(1, 1, 1, 1);
	BuildObjectList();
	SortObjectList();
	RenderShadowMap();
	RenderSkybox();
	RenderCamera();
	glDisable(GL_CULL_FACE); //Todo - text indices are going the wrong way...
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	NewRenderLines();
	NewRenderTextures();
	NewRenderText();
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void NCL::CSC8503::GameTechRenderer::DebugShadowFrustum(const Matrix4& shadowProjMatrix, const Matrix4& shadowViewMatrix)
{
	// Define the 8 corners of the frustum in clip space
	Vector4 frustumCorners[] = {
		Vector4(-1, -1, 0, 1), // Near bottom left
		Vector4(1, -1, 0, 1), // Near bottom right
		Vector4(-1,  1, 0, 1), // Near top left
		Vector4(1,  1, 0, 1), // Near top right
		Vector4(-1, -1, 1, 1), // Far bottom left
		Vector4(1, -1, 1, 1), // Far bottom right
		Vector4(-1,  1, 1, 1), // Far top left
		Vector4(1,  1, 1, 1)  // Far top right
	};

	// Compute inverse light-space transform
	Matrix4 lightMatrix = shadowProjMatrix * shadowViewMatrix;
	Matrix4 inverseLightMatrix = Matrix::Inverse(lightMatrix);

	// Transform frustum corners from light clip space to world space
	Vector3 worldFrustumCorners[8];
	for (int i = 0; i < 8; i++) {
		Vector4 worldPos = inverseLightMatrix * frustumCorners[i];

		// Manually extract x, y, and z instead of using .xyz()
		worldFrustumCorners[i] = Vector3(worldPos.x / worldPos.w, worldPos.y / worldPos.w, worldPos.z / worldPos.w);
	}

	// Define a yellow color for visualization
	Vector4 debugColor = Vector4(1, 1, 0, 1); // Yellow

	// Draw Near Plane
	Debug::DrawLine(worldFrustumCorners[0], worldFrustumCorners[1], debugColor);
	Debug::DrawLine(worldFrustumCorners[1], worldFrustumCorners[3], debugColor);
	Debug::DrawLine(worldFrustumCorners[3], worldFrustumCorners[2], debugColor);
	Debug::DrawLine(worldFrustumCorners[2], worldFrustumCorners[0], debugColor);

	// Draw Far Plane
	Debug::DrawLine(worldFrustumCorners[4], worldFrustumCorners[5], debugColor);
	Debug::DrawLine(worldFrustumCorners[5], worldFrustumCorners[7], debugColor);
	Debug::DrawLine(worldFrustumCorners[7], worldFrustumCorners[6], debugColor);
	Debug::DrawLine(worldFrustumCorners[6], worldFrustumCorners[4], debugColor);

	// Connect Near and Far Planes
	Debug::DrawLine(worldFrustumCorners[0], worldFrustumCorners[4], debugColor);
	Debug::DrawLine(worldFrustumCorners[1], worldFrustumCorners[5], debugColor);
	Debug::DrawLine(worldFrustumCorners[2], worldFrustumCorners[6], debugColor);
	Debug::DrawLine(worldFrustumCorners[3], worldFrustumCorners[7], debugColor);
}

void GameTechRenderer::BuildObjectList() {
	activeObjects.clear();

	gameWorld.OperateOnContents(
		[&](PaintballGameObject* o) {
			if (o->IsActive()) {
				const PaintballRenderObject* g = o->GetRenderObject();
				if (g) {
					activeObjects.emplace_back(g);
				}
			}
		}
	);


}

void GameTechRenderer::SortObjectList() {

}

void GameTechRenderer::RenderShadowMap() {
	// Bind the FBO for the shadow map and clear only the depth buffer.
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClear(GL_DEPTH_BUFFER_BIT);
	glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);
	glViewport(0, 0, SHADOWSIZE, SHADOWSIZE);

	// Cull front faces to help reduce shadow acne.
	glCullFace(GL_FRONT);

	// Use the shadow shader.
	UseShader(*shadowShader);
	int mvpLocation = glGetUniformLocation(shadowShader->GetProgramID(), "mvpMatrix");

	// Compute the light�s view matrix.
	// Instead of always looking at (0,0,0), choose a target that covers your scene.
	// For example, if your scene is centered around (0,0,0), that is acceptable.
	Vector3 lightTarget = Vector3(0, 0, 0);  // Change this if your scene center is different.
	Matrix4 shadowViewMatrix = Matrix::View(Vector3(-10, 50, 0), lightTarget, Vector3(0, 1, 0));

	// Use a perspective projection for the light camera.
	// (A 45� field-of-view is typical.)
	//Matrix4 shadowProjMatrix = Matrix::Perspective(45.0f, 1.0f, 1.0f, 1000.0f);
	Matrix4 shadowProjMatrix = Matrix::Perspective(20.0f, 75.0f, 1.0f, 100.0f);






	// Combine to get the light�s MVP (for all objects)
	Matrix4 lightMVP = shadowProjMatrix * shadowViewMatrix;

	// Compute the final shadow matrix with bias (to move coordinates from [-1,1] to [0,1]).
	shadowMatrix = biasMatrix * lightMVP;

	// Render each active object using the shadow shader.SSS
	for (const auto& obj : activeObjects) {
		Matrix4 modelMatrix = (*obj).GetTransform()->GetMatrix();
		// For each object, the final matrix is lightMVP * modelMatrix.
		Matrix4 mvpMatrix = lightMVP * modelMatrix;
		glUniformMatrix4fv(mvpLocation, 1, false, (float*)&mvpMatrix);
		BindMesh((OGLMesh&)*(*obj).GetMesh());
		size_t layerCount = (*obj).GetMesh()->GetSubMeshCount();
		for (size_t j = 0; j < layerCount; ++j) {
			DrawBoundMesh((uint32_t)j);
		}
	}
	Debug::DrawLine(lightPosition, lightTarget, Vector4(1, 1, 0, 1)); // Draw light direction
	Debug::DrawBox(lightPosition, Vector3(1, 1, 1), Vector4(1, 0, 0, 1), 20000000.0f); // Draw light position

	DebugShadowFrustum(shadowProjMatrix, shadowViewMatrix);



	// Restore the viewport and state.
	glViewport(0, 0, windowSize.x, windowSize.y);
	glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glCullFace(GL_BACK);
}




void GameTechRenderer::AddLight(const Light& light) {
	if (lights.size() < 8) { // Limit to 8 lights (defined in shader)
		lights.push_back(light);
	}
}

void GameTechRenderer::RenderSkybox() {
	glDisable(GL_CULL_FACE);
	glDisable(GL_BLEND);
	glDisable(GL_DEPTH_TEST);

	Matrix4 viewMatrix = gameWorld.GetMainCamera().BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera().BuildProjectionMatrix(hostWindow.GetScreenAspect());

	UseShader(*skyboxShader);

	int projLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "projMatrix");
	int viewLocation = glGetUniformLocation(skyboxShader->GetProgramID(), "viewMatrix");
	int texLocation  = glGetUniformLocation(skyboxShader->GetProgramID(), "cubeTex");

	glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
	glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

	glUniform1i(texLocation, 0);
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, skyboxTex);

	BindMesh(*skyboxMesh);
	DrawBoundMesh();

	glEnable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);
}

void GameTechRenderer::RenderCamera() {

	Matrix4 viewMatrix = gameWorld.GetMainCamera().BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera().BuildProjectionMatrix(hostWindow.GetScreenAspect());

	OGLShader* activeShader = nullptr;
	int projLocation	= 0;
	int viewLocation	= 0;
	int modelLocation	= 0;
	int colourLocation  = 0;
	int hasVColLocation = 0;
	int hasTexLocation  = 0;
	int shadowLocation  = 0;

	int lightPosLocation	= 0;
	int lightColourLocation = 0;
	int lightRadiusLocation = 0;

	int cameraLocation = 0;

	//TODO - PUT IN FUNCTION
	glActiveTexture(GL_TEXTURE0 + 1);
	glBindTexture(GL_TEXTURE_2D, shadowTex);

	for (const auto&i : activeObjects) {
		//OGLShader* shader = (OGLShader*)(*i).GetShader();
		OGLShader* shader = i->GetAnimation() ? animShader : (OGLShader*)(*i).GetShader();
		UseShader(*shader);

		if ((*i).GetDefaultTexture()) {
			BindTextureToShader(*(OGLTexture*)(*i).GetDefaultTexture(), "mainTex", 0);
		}


		if (activeShader != shader) {
			projLocation	= glGetUniformLocation(shader->GetProgramID(), "projMatrix");
			viewLocation	= glGetUniformLocation(shader->GetProgramID(), "viewMatrix");
			modelLocation	= glGetUniformLocation(shader->GetProgramID(), "modelMatrix");
			shadowLocation  = glGetUniformLocation(shader->GetProgramID(), "shadowMatrix");
			colourLocation  = glGetUniformLocation(shader->GetProgramID(), "objectColour");
			hasVColLocation = glGetUniformLocation(shader->GetProgramID(), "hasVertexColours");
			hasTexLocation  = glGetUniformLocation(shader->GetProgramID(), "hasTexture");

			/*lightPosLocation	= glGetUniformLocation(shader->GetProgramID(), "lightPos");
			lightColourLocation = glGetUniformLocation(shader->GetProgramID(), "lightColour");
			lightRadiusLocation = glGetUniformLocation(shader->GetProgramID(), "lightRadius");*/

			cameraLocation = glGetUniformLocation(shader->GetProgramID(), "cameraPos");

			Vector3 camPos = gameWorld.GetMainCamera().GetPosition();
			glUniform3fv(cameraLocation, 1, &camPos.x);

			glUniformMatrix4fv(projLocation, 1, false, (float*)&projMatrix);
			glUniformMatrix4fv(viewLocation, 1, false, (float*)&viewMatrix);

			

			activeShader = shader;
		}
		int hasAnimLocation = glGetUniformLocation(shader->GetProgramID(), "hasAnimation");
		

		if (i->GetAnimation()) {
			//std::cout << "Animation detected" << std::endl;
			vector<Matrix4> frameMatrices;
			const Matrix4* invBindPose = i->GetMesh()->GetInverseBindPose().data();
			const Matrix4* frameData = i->GetAnimation()->GetJointData(i->GetCurrentFrame());
			if (!frameData) {
				std::cout << "Error: Frame data is NULL!" << std::endl;
				continue;
			}
			for (unsigned int j = 0; j < i->GetMesh()->GetJointCount(); ++j) {
				frameMatrices.emplace_back(frameData[j] * invBindPose[j]);
			}
			int jointLocation = glGetUniformLocation(shader->GetProgramID(), "joints");
			glUniformMatrix4fv(jointLocation, static_cast<GLsizei>(frameMatrices.size()), false, (float*)frameMatrices.data());
			//std::cout << "Final World Position: " << i->GetTransform()->GetPosition().x << ", " << i->GetTransform()->GetPosition().y << ", " << i->GetTransform()->GetPosition().z << std::endl;
		}

		int numLights = (int)lights.size();
		glUniform1i(glGetUniformLocation(shader->GetProgramID(), "numLights"), numLights);
		for (int i = 0; i < numLights; ++i) {
			std::string index = std::to_string(i);

			Vector3 pos = lights[i].GetPosition();
			glUniform3fv(glGetUniformLocation(shader->GetProgramID(), ("lights[" + index + "].position").c_str()), 1, (float*)&pos);

			Vector4 col = lights[i].GetColor();
			glUniform4fv(glGetUniformLocation(shader->GetProgramID(), ("lights[" + index + "].color").c_str()), 1, (float*)&col);

			float rad = lights[i].GetRadius();
			glUniform1f(glGetUniformLocation(shader->GetProgramID(), ("lights[" + index + "].radius").c_str()), rad);

			Vector3 dir = lights[i].GetDirection(); // New: Spotlight direction
			glUniform3fv(glGetUniformLocation(shader->GetProgramID(), ("lights[" + index + "].direction").c_str()), 1, (float*)&dir);

			float cutoff = cos(lights[i].GetCutoff() * (3.14159265359f / 180.0f)); // Convert to radians
			glUniform1f(glGetUniformLocation(shader->GetProgramID(), ("lights[" + index + "].cutoff").c_str()), cutoff);
		}
		int shadowTexLocation = glGetUniformLocation(shader->GetProgramID(), "shadowTex");
		glUniform1i(shadowTexLocation, 1);

		
		Matrix4 modelMatrix = (*i).GetTransform()->GetMatrix();
		glUniformMatrix4fv(modelLocation, 1, false, (float*)&modelMatrix);			
		
		Matrix4 fullShadowMat = shadowMatrix * modelMatrix;
		glUniformMatrix4fv(shadowLocation, 1, false, (float*)&fullShadowMat);

		Vector4 colour = i->GetColour();
		glUniform4fv(colourLocation, 1, &colour.x);

		glUniform1i(hasVColLocation, !(*i).GetMesh()->GetColourData().empty());

		glUniform1i(hasTexLocation, (OGLTexture*)(*i).GetDefaultTexture() ? 1:0);

		BindMesh((OGLMesh&)*(*i).GetMesh());
		size_t layerCount = (*i).GetMesh()->GetSubMeshCount();
		for (size_t i = 0; i < layerCount; ++i) {
			DrawBoundMesh((uint32_t)i);
		}
	}

}

Mesh* GameTechRenderer::LoadMesh(const std::string& name) {
	OGLMesh* mesh = new OGLMesh();
	MshLoader::LoadMesh(name, *mesh);
	mesh->SetPrimitiveType(GeometryPrimitive::Triangles);
	mesh->UploadToGPU();
	return mesh;
}

void GameTechRenderer::NewRenderLines() {
	const std::vector<Debug::DebugLineEntry>& lines = Debug::GetDebugLines();
	if (lines.empty()) {
		return;
	}

	Matrix4 viewMatrix = gameWorld.GetMainCamera().BuildViewMatrix();
	Matrix4 projMatrix = gameWorld.GetMainCamera().BuildProjectionMatrix(hostWindow.GetScreenAspect());
	
	Matrix4 viewProj  = projMatrix * viewMatrix;

	UseShader(*debugShader);
	int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
	glUniform1i(texSlot, 0);

	glUniformMatrix4fv(matSlot, 1, false, (float*)viewProj.array);

	debugLineData.clear();

	size_t frameLineCount = lines.size() * 2;

	SetDebugLineBufferSizes(frameLineCount);

	glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, lines.size() * sizeof(Debug::DebugLineEntry), lines.data());
	

	glBindVertexArray(lineVAO);
	glDrawArrays(GL_LINES, 0, (GLsizei)frameLineCount);
	glBindVertexArray(0);
}

void GameTechRenderer::NewRenderText() {
	const std::vector<Debug::DebugStringEntry>& strings = Debug::GetDebugStrings();
	if (strings.empty()) {
		return;
	}

	UseShader(*debugShader);

	OGLTexture* t = (OGLTexture*)Debug::GetDebugFont()->GetTexture();

	if (t) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, t->GetObjectID());
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glBindTexture(GL_TEXTURE_2D, 0);	
		BindTextureToShader(*t, "mainTex", 0);
	}

	Matrix4 proj = Matrix::Orthographic(0.0f, 100.0f, 100.0f, 0.0f, -1.0f, 1.0f);

	int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	glUniformMatrix4fv(matSlot, 1, false, (float*)proj.array);

	GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
	glUniform1i(texSlot, 1);

	debugTextPos.clear();
	debugTextColours.clear();
	debugTextUVs.clear();

	int frameVertCount = 0;
	for (const auto& s : strings) {
		frameVertCount += Debug::GetDebugFont()->GetVertexCountForString(s.data);
	}
	SetDebugStringBufferSizes(frameVertCount);

	for (const auto& s : strings) {
		float size = 20.0f;
		Debug::GetDebugFont()->BuildVerticesForString(s.data, s.position, s.colour, size, debugTextPos, debugTextUVs, debugTextColours);
	}

	glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector3), debugTextPos.data());
	glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector4), debugTextColours.data());
	glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
	glBufferSubData(GL_ARRAY_BUFFER, 0, frameVertCount * sizeof(Vector2), debugTextUVs.data());

	glBindVertexArray(textVAO);
	glDrawArrays(GL_TRIANGLES, 0, frameVertCount);
	glBindVertexArray(0);
}

void GameTechRenderer::NewRenderTextures() {
	const std::vector<Debug::DebugTexEntry>& texEntries = Debug::GetDebugTex();
	if (texEntries.empty()) {
		return;
	}
	UseShader(*debugShader);

	Matrix4 proj = Matrix::Orthographic(0.0f, 100.0f, 100.0f, 0.0f, -1.0f, 1.0f);

	int matSlot = glGetUniformLocation(debugShader->GetProgramID(), "viewProjMatrix");
	glUniformMatrix4fv(matSlot, 1, false, (float*)proj.array);

	GLuint texSlot = glGetUniformLocation(debugShader->GetProgramID(), "useTexture");
	glUniform1i(texSlot, 2);

	GLuint useColourSlot = glGetUniformLocation(debugShader->GetProgramID(), "useColour");
	glUniform1i(useColourSlot, 1);

	GLuint colourSlot = glGetUniformLocation(debugShader->GetProgramID(), "texColour");

	BindMesh(*debugTexMesh);

	glActiveTexture(GL_TEXTURE0);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	
	for (const auto& tex : texEntries) {	
		OGLTexture* t = (OGLTexture*)tex.t;
		glBindTexture(GL_TEXTURE_2D, t->GetObjectID());	
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		BindTextureToShader(*t, "mainTex", 0);

		Matrix4 transform = Matrix::Translation(Vector3(tex.position.x, tex.position.y, 0)) * Matrix::Scale(Vector3(tex.scale.x, tex.scale.y, 1.0f));
		Matrix4 finalMatrix = proj * transform;

		glUniformMatrix4fv(matSlot, 1, false, (float*)finalMatrix.array);

		glUniform4f(colourSlot, tex.colour.x, tex.colour.y, tex.colour.z, tex.colour.w);

		DrawBoundMesh();
	}

	glUniform1i(useColourSlot, 0);
}
 
Texture* GameTechRenderer::LoadTexture(const std::string& name) {
	return OGLTexture::TextureFromFile(name).release();
}

Shader* GameTechRenderer::LoadShader(const std::string& vertex, const std::string& fragment) {
	return new OGLShader(vertex, fragment);
}

void GameTechRenderer::SetDebugStringBufferSizes(size_t newVertCount) {
	if (newVertCount > textCount) {
		textCount = newVertCount;

		glBindBuffer(GL_ARRAY_BUFFER, textVertVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector3), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textColourVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector4), nullptr, GL_DYNAMIC_DRAW);

		glBindBuffer(GL_ARRAY_BUFFER, textTexVBO);
		glBufferData(GL_ARRAY_BUFFER, textCount * sizeof(Vector2), nullptr, GL_DYNAMIC_DRAW);

		debugTextPos.reserve(textCount);
		debugTextColours.reserve(textCount);
		debugTextUVs.reserve(textCount);

		glBindVertexArray(textVAO);

		glVertexAttribFormat(0, 3, GL_FLOAT, false, 0);
		glVertexAttribBinding(0, 0);
		glBindVertexBuffer(0, textVertVBO, 0, sizeof(Vector3));

		glVertexAttribFormat(1, 4, GL_FLOAT, false, 0);
		glVertexAttribBinding(1, 1);
		glBindVertexBuffer(1, textColourVBO, 0, sizeof(Vector4));

		glVertexAttribFormat(2, 2, GL_FLOAT, false, 0);
		glVertexAttribBinding(2, 2);
		glBindVertexBuffer(2, textTexVBO, 0, sizeof(Vector2));

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);
		glEnableVertexAttribArray(2);

		glBindVertexArray(0);
	}
}

void GameTechRenderer::SetDebugLineBufferSizes(size_t newVertCount) {
	if (newVertCount > lineCount) {
		lineCount = newVertCount;

		glBindBuffer(GL_ARRAY_BUFFER, lineVertVBO);
		glBufferData(GL_ARRAY_BUFFER, lineCount * sizeof(Debug::DebugLineEntry), nullptr, GL_DYNAMIC_DRAW);

		debugLineData.reserve(lineCount);

		glBindVertexArray(lineVAO);

		int realStride = sizeof(Debug::DebugLineEntry) / 2;

		glVertexAttribFormat(0, 3, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, start));
		glVertexAttribBinding(0, 0);
		glBindVertexBuffer(0, lineVertVBO, 0, realStride);

		glVertexAttribFormat(1, 4, GL_FLOAT, false, offsetof(Debug::DebugLineEntry, colourA));
		glVertexAttribBinding(1, 0);
		glBindVertexBuffer(1, lineVertVBO, sizeof(Vector4), realStride);

		glEnableVertexAttribArray(0);
		glEnableVertexAttribArray(1);

		glBindVertexArray(0);
	}
}