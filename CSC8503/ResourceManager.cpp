#include "ResourceManager.h"
using namespace NCL::CSC8503;

void ResourceManager::LoadAssets(GameTechRenderer* renderer) {
    if (cubeMesh == nullptr) { // Only load once
        cubeMesh = renderer->LoadMesh("cube.msh");
        sphereMesh = renderer->LoadMesh("sphere.msh");
        catMesh = renderer->LoadMesh("ORIGAMI_Chat.msh");
        kittenMesh = renderer->LoadMesh("Kitten.msh");
        mapMesh = renderer->LoadMesh("MAIN map.msh");
        secondMapMesh = renderer->LoadMesh("Second Map 1.msh");
        gooseMesh = renderer->LoadMesh("goose.msh");
        enemyMesh = renderer->LoadMesh("Keeper.msh");
        bonusMesh = renderer->LoadMesh("19463_Kitten_Head_v1.msh");
        capsuleMesh = renderer->LoadMesh("capsule.msh");

        RoleMesh = renderer->LoadMesh("Role_T.msh");
        Roleanim = new MeshAnimation("Role_T.anm");
        Rolemat = new MeshMaterial("Role_T.mat");

        basicTex = renderer->LoadTexture("checkerboard.png");
        basicShader = renderer->LoadShader("scene.vert", "scene.frag");
    }
}

