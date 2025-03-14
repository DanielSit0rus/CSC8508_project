#include "ResourceManager.h"
using namespace NCL::CSC8503;

void ResourceManager::LoadAssets(GameTechRenderer* renderer) {
    if (GetMesh("cube", false) == nullptr) { // Only load once
        meshes["cube"] = renderer->LoadMesh("cube.msh");
        meshes["sphere"] = renderer->LoadMesh("sphere.msh");

        meshes["cat"] = renderer->LoadMesh("ORIGAMI_Chat.msh");
        meshes["kitten"] = renderer->LoadMesh("Kitten.msh");

        meshes["floor_1"] = renderer->LoadMesh("Map1Floor.msh");
        meshes["mainWall_1"] = renderer->LoadMesh("MainWall.msh");
        meshes["doorWay_1"] = renderer->LoadMesh("DoorWay.msh");
        meshes["oppDoorWay_1"] = renderer->LoadMesh("OppDoorWay.msh");
        meshes["midDoor"] = renderer->LoadMesh("MIdDoor.msh");
        meshes["wall_1"] = renderer->LoadMesh("Walls.msh");
        meshes["wall_2"] = renderer->LoadMesh("Walls2.msh");
        meshes["openWall"] = renderer->LoadMesh("OpenWall.msh");
        meshes["wall_3"] = renderer->LoadMesh("Wall3.msh");
        meshes["wall_4"] = renderer->LoadMesh("Wall4.msh");
        meshes["cylinder"] = renderer->LoadMesh("Cylinders.msh");
        meshes["bigCylinder"] = renderer->LoadMesh("BigCylinder.msh");
        meshes["target"] = renderer->LoadMesh("Targets.msh");
        meshes["bigCube"] = renderer->LoadMesh("BigCube.msh");
        meshes["floor_2"] = renderer->LoadMesh("FloorM2.msh");
        meshes["mainWall_2"] = renderer->LoadMesh("MainWallM2.msh");
        meshes["doorWay_2"] = renderer->LoadMesh("DoorWayM2.msh");
        meshes["wall_2_1"] = renderer->LoadMesh("Wall1M2.msh");
        meshes["oppDoorWay_2"] = renderer->LoadMesh("OppDoorWayM2.msh");
        meshes["wall_2_2"] = renderer->LoadMesh("Wall2M2.msh");
        meshes["wall_2_3"] = renderer->LoadMesh("Wall3M2.msh");
        meshes["columns_2"] = renderer->LoadMesh("ColumnsM2.msh");
        meshes["midTunnel"] = renderer->LoadMesh("MidTunnel.msh");
        meshes["elevation"] = renderer->LoadMesh("Elevation.msh");
        meshes["bridge"] = renderer->LoadMesh("Bridge.msh");
        meshes["elevRail"] = renderer->LoadMesh("ElevRail.msh");
        meshes["midRail"] = renderer->LoadMesh("MidRail.msh");
        meshes["bridgeRail"] = renderer->LoadMesh("BridgeRail.msh");

        meshes["goose"] = renderer->LoadMesh("goose.msh");
        meshes["enemy"] = renderer->LoadMesh("Keeper.msh");
        meshes["bonus"] = renderer->LoadMesh("19463_Kitten_Head_v1.msh");
        meshes["capsule"] = renderer->LoadMesh("capsule.msh");

        meshes["role"] = renderer->LoadMesh("Role_T.msh");
        Roleanim = std::make_shared<MeshAnimation>("Role_T_Idle.anm");
        Moveanim = std::make_shared<MeshAnimation>("Role_T.anm");
        Rolemat = std::make_shared<MeshMaterial>("Role_T.mat");

        textures["basic"] = renderer->LoadTexture("checkerboard.png");
        textures["floor"] = renderer->LoadTexture("6x6_base.png");

        textures["texture1_diffuse"] = renderer->LoadTexture("Texture_1_Diffuse.png");
        textures["texture1_specular"] = renderer->LoadTexture("Texture_1_Specular.png");
        textures["texture1_normal"] = renderer->LoadTexture("Texture_1_Normal.png");

        textures["texture2_diffuse"] = renderer->LoadTexture("Texture_2_Diffuse.png");
        textures["texture2_specular"] = renderer->LoadTexture("Texture_2_Specular.png");
        textures["texture2_normal"] = renderer->LoadTexture("Texture_2_Normal.png");

        textures["texture3_diffuse"] = renderer->LoadTexture("Texture_3_Diffuse.png");
        textures["texture3_specular"] = renderer->LoadTexture("Texture_3_Specular.png");
        textures["texture3_normal"] = renderer->LoadTexture("Texture_3_Normal.png");

        textures["texture4_diffuse"] = renderer->LoadTexture("Texture_4_Diffuse.png");
        textures["texture4_specular"] = renderer->LoadTexture("Texture_4_Specular.png");
        textures["texture4_normal"] = renderer->LoadTexture("Texture_4_Normal.png");

        textures["texture5_normal"] = renderer->LoadTexture("Texture_5_Normal.png");
        textures["texture5_diffuse"] = renderer->LoadTexture("Texture_5_Diffuse.png");
        textures["texture5_specular"] = renderer->LoadTexture("Texture_5_Specular.png");



       
 

        shaders["basic"] = renderer->LoadShader("scene.vert", "scene.frag");
    }
}

NCL::Rendering::Mesh* ResourceManager::GetMesh(const std::string& name, bool debug)
{
    auto it = meshes.find(name);
    if (it != meshes.end()) {
        return it->second;
    }
    if (debug) std::cerr << "[ResourceManager] Cannot find mesh [" << name << "]" << std::endl;
    return nullptr;
}

NCL::Rendering::Texture* ResourceManager::GetTexture(const std::string& name, bool debug)
{
    auto it = textures.find(name);
    if (it != textures.end()) {
        return it->second;
    }
    if (debug) std::cerr << "[ResourceManager] Cannot find texture [" << name << "]" << std::endl;
    return nullptr;
}

NCL::Rendering::Shader* ResourceManager::GetShader(const std::string& name, bool debug)
{
    auto it = shaders.find(name);
    if (it != shaders.end()) {
        return it->second;
    }
    if (debug) std::cerr << "[ResourceManager] Cannot find shader [" << name << "]" << std::endl;
    return nullptr;
}

NCL::Rendering::Shader* NCL::CSC8503::ResourceManager::GetBasicShader()
{
    return GetShader("basic");
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

