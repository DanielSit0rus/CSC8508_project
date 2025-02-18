#include "ResourceManager.h"

void ResourceManager::LoadAssets(GameTechRenderer* renderer) {
    if (cubeMesh == nullptr) { // Only load once
        cubeMesh = renderer->LoadMesh("cube.msh");
        sphereMesh = renderer->LoadMesh("sphere.msh");
        catMesh = renderer->LoadMesh("ORIGAMI_Chat.msh");
        kittenMesh = renderer->LoadMesh("Kitten.msh");
        mapMesh = renderer->LoadMesh("Map4.msh");
        gooseMesh = renderer->LoadMesh("goose.msh");
        enemyMesh = renderer->LoadMesh("Keeper.msh");
        bonusMesh = renderer->LoadMesh("19463_Kitten_Head_v1.msh");
        capsuleMesh = renderer->LoadMesh("capsule.msh");

        basicTex = renderer->LoadTexture("checkerboard.png");
        basicShader = renderer->LoadShader("scene.vert", "scene.frag");
    }
}

