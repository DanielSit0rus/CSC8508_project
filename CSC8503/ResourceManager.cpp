#include "ResourceManager.h"
using namespace NCL::CSC8503;

void ResourceManager::LoadAssets(GameTechRenderer* renderer) {
    if (cubeMesh == nullptr) { // Only load once
        cubeMesh = renderer->LoadMesh("cube.msh");
        sphereMesh = renderer->LoadMesh("sphere.msh");
        catMesh = renderer->LoadMesh("ORIGAMI_Chat.msh");
        kittenMesh = renderer->LoadMesh("Kitten.msh");

        floor1Mesh = renderer->LoadMesh("Map1Floor.msh");
        mainWallMesh = renderer->LoadMesh("MainWall.msh");
        doorWayMesh = renderer->LoadMesh("DoorWay.msh");
        oppDoorWayMesh = renderer->LoadMesh("OppDoorWay.msh");
        midDoorMesh = renderer->LoadMesh("MIdDoor.msh");
        wallMesh = renderer->LoadMesh("Walls.msh");
        wall2Mesh = renderer->LoadMesh("Walls2.msh");
        openWallMesh = renderer->LoadMesh("OpenWall.msh");
        wall3Mesh = renderer->LoadMesh("Wall3.msh");
        wall4Mesh = renderer->LoadMesh("Wall4.msh");
        cylinderMesh = renderer->LoadMesh("Cylinders.msh");
        bigCylinderMesh = renderer->LoadMesh("BigCylinder.msh");
        targetMesh = renderer->LoadMesh("Targets.msh");
        bigCubeMesh = renderer->LoadMesh("BigCube.msh");
        floor2Mesh = renderer->LoadMesh("FloorM2.msh");
        mainWall2Mesh = renderer->LoadMesh("MainWallM2.msh");
        doorWayM2Mesh = renderer->LoadMesh("DoorWayM2.msh");
        wall1M2Mesh = renderer->LoadMesh("Wall1M2.msh");
        oppDoorWayM2Mesh = renderer->LoadMesh("OppDoorWayM2.msh");
        wall2M2Mesh = renderer->LoadMesh("Wall2M2.msh");
        wall3M2Mesh = renderer->LoadMesh("Wall3M2.msh");
        columnsM2Mesh = renderer->LoadMesh("ColumnsM2.msh");
        midTunnelMesh = renderer->LoadMesh("MidTunnel.msh");
        elevationMesh = renderer->LoadMesh("Elevation.msh");
        bridgeMesh = renderer->LoadMesh("Bridge.msh");
        elevRailMesh = renderer->LoadMesh("ElevRail.msh");
        midRailMesh = renderer->LoadMesh("MidRail.msh");
        bridgeRailMesh = renderer->LoadMesh("BridgeRail.msh");

        gooseMesh = renderer->LoadMesh("goose.msh");
        enemyMesh = renderer->LoadMesh("Keeper.msh");
        bonusMesh = renderer->LoadMesh("19463_Kitten_Head_v1.msh");
        capsuleMesh = renderer->LoadMesh("capsule.msh");

        RoleMesh = renderer->LoadMesh("Role_T.msh");
        Roleanim = std::make_shared<MeshAnimation>("Role_T_Idle.anm");
        Moveanim = std::make_shared<MeshAnimation>("Role_T.anm");
        Rolemat = std::make_shared<MeshMaterial>("Role_T.mat");

        basicTex = renderer->LoadTexture("checkerboard.png");
        basicShader = renderer->LoadShader("scene.vert", "scene.frag");
    }
}

//void ResourceManager::ReloadAnimations() {
//
//    if (Roleanim) {
//        delete Roleanim;
//        Roleanim = nullptr;
//    }
//    if (Rolemat) {
//        delete Rolemat;
//        Rolemat = nullptr;
//    }
//    Roleanim = new MeshAnimation("Role_T.anm");
//    Rolemat = new MeshMaterial("Role_T.mat");
//
//    if (!Roleanim) {
//        std::cerr << "Error: Failed to reload role animation!" << std::endl;
//    }
//}

