#include "PaintballRenderObject.h"
#include "Mesh.h"
#include "ResourceManager.h"

using namespace NCL::CSC8503;
using namespace NCL;

PaintballRenderObject::PaintballRenderObject(PaintballTransform* parentTransform, std::string meshName, std::string textureName, std::string shaderName,
    std::shared_ptr<MeshAnimation> animation, std::shared_ptr<MeshMaterial> material)
    : transform(parentTransform), meshName(meshName), textureName(textureName), shaderName(shaderName), animation(animation), material(material) {

    colour = Vector4(1, 1, 1, 1);
    currentFrame = 0;
    frameTime = 0.0f;

    mesh = ResourceManager::GetInstance().GetMesh(meshName);
    texture = ResourceManager::GetInstance().GetTexture(textureName);
    shader = ResourceManager::GetInstance().GetShader(shaderName);
}

PaintballRenderObject::~PaintballRenderObject() {

 
}

void PaintballRenderObject::SetDefaultTexture(const std::string& t)
{
    texture = ResourceManager::GetInstance().GetTexture(t);
}

void PaintballRenderObject::SetSpecularTexture(const std::string& t)
{
    specularTextureName = t;
    specularTexture = ResourceManager::GetInstance().GetTexture(t);
}

void PaintballRenderObject::SetNormalTexture(const std::string& t)
{
    normalTextureName = t;
    normalTexture = ResourceManager::GetInstance().GetTexture(t);
}

void NCL::CSC8503::PaintballRenderObject::SetHeightTexture(const std::string& t)
{
    heightTextureName = t;
    heightTexture = ResourceManager::GetInstance().GetTexture(t);
}

void NCL::CSC8503::PaintballRenderObject::SetMetallicTexture(const std::string& t)
{
    metallicTextureName = t;
    metallicTexture = ResourceManager::GetInstance().GetTexture(t);
}

void NCL::CSC8503::PaintballRenderObject::SetOcclusionTexture(const std::string& t)
{
    occlusionTextureName = t;
    occlusionTexture = ResourceManager::GetInstance().GetTexture(t);
}

void NCL::CSC8503::PaintballRenderObject::SetMaskMapTexture(const std::string& t)
{
    maskMapTextureName = t;
    maskMapTexture = ResourceManager::GetInstance().GetTexture(t);

}
void NCL::CSC8503::PaintballRenderObject::SetRoughnessTexture(const std::string& t)
{
    roughnessTextureName = t;
    roughnessTexture = ResourceManager::GetInstance().GetTexture(t);

}

void PaintballRenderObject::UpdateAnimation(float dt) {
    if (!animation)
    {
        return;
    }
    int frameCount = animation->GetFrameCount();
    if (frameCount <= 1) {
        std::cout << "Animation has too few frames!" << std::endl;
        return;
    }
    frameTime -= dt;
    while (frameTime < 0.0f) {
        currentFrame = (currentFrame + 1) % animation->GetFrameCount();
        frameTime += 1.0f / animation->GetFrameRate();
    }
}