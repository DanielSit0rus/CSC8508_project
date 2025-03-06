#include "PaintballRenderObject.h"
#include "Mesh.h"

using namespace NCL::CSC8503;
using namespace NCL;

PaintballRenderObject::PaintballRenderObject(PaintballTransform* parentTransform, Mesh* mesh, Texture* tex, Shader* shader,
    std::shared_ptr<MeshAnimation> animation, std::shared_ptr<MeshMaterial> material)
    : transform(parentTransform), mesh(mesh), texture(tex), shader(shader), animation(animation), material(material) {
    colour = Vector4(1, 1, 1, 1);
    currentFrame = 0;
    frameTime = 0.0f;
}

PaintballRenderObject::~PaintballRenderObject() {

 
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