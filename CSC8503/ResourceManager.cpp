#include "ResourceManager.h"
using namespace NCL::CSC8503;

void ResourceManager::LoadAssets(GameTechRenderer* renderer) {
    if (GetMesh("cube", false) == nullptr) { // Only load once
        renderer->LoadMesh(meshes, "cube", "cube.msh");
        renderer->LoadMesh(meshes, "sphere", "sphere.msh");

        renderer->LoadMesh(meshes, "cat", "ORIGAMI_Chat.msh");
        renderer->LoadMesh(meshes, "kitten", "Kitten.msh");

        renderer->LoadMesh(meshes, "floor_1", "Map1Floor.msh");
        renderer->LoadMesh(meshes, "mainWall_1", "MainWall.msh");
        renderer->LoadMesh(meshes, "doorWay_1", "DoorWay.msh");
        renderer->LoadMesh(meshes, "oppDoorWay_1", "OppDoorWay.msh");
        renderer->LoadMesh(meshes, "midDoor", "MIdDoor.msh");
        renderer->LoadMesh(meshes, "wall_1", "Walls.msh");
        renderer->LoadMesh(meshes, "wall_2", "Walls2.msh");
        renderer->LoadMesh(meshes, "openWall", "OpenWall.msh");
        renderer->LoadMesh(meshes, "wall_3", "Wall3.msh");
        renderer->LoadMesh(meshes, "wall_4", "Wall4.msh");
        renderer->LoadMesh(meshes, "cylinder", "Cylinders.msh");
        renderer->LoadMesh(meshes, "bigCylinder", "BigCylinder.msh");
        renderer->LoadMesh(meshes, "target", "Targets.msh");
        renderer->LoadMesh(meshes, "bigCube", "BigCube.msh");
        renderer->LoadMesh(meshes, "floor_2", "FloorM2.msh");
        renderer->LoadMesh(meshes, "mainWall_2", "MainWallM2.msh");
        renderer->LoadMesh(meshes, "doorWay_2", "DoorWayM2.msh");
        renderer->LoadMesh(meshes, "wall_2_1", "Wall1M2.msh");
        renderer->LoadMesh(meshes, "oppDoorWay_2", "OppDoorWayM2.msh");
        renderer->LoadMesh(meshes, "wall_2_2", "Wall2M2.msh");
        renderer->LoadMesh(meshes, "wall_2_3", "Wall3M2.msh");
        renderer->LoadMesh(meshes, "columns_2", "ColumnsM2.msh");
        renderer->LoadMesh(meshes, "midTunnel", "MidTunnel.msh");
        renderer->LoadMesh(meshes, "elevation", "Elevation.msh");
        renderer->LoadMesh(meshes, "bridge", "Bridge.msh");
        renderer->LoadMesh(meshes, "elevRail", "ElevRail.msh");
        renderer->LoadMesh(meshes, "midRail", "MidRail.msh");
        renderer->LoadMesh(meshes, "bridgeRail", "BridgeRail.msh");

        renderer->LoadMesh(meshes, "goose", "goose.msh");
        renderer->LoadMesh(meshes, "enemy", "Keeper.msh");
        renderer->LoadMesh(meshes, "bonus", "19463_Kitten_Head_v1.msh");
        renderer->LoadMesh(meshes, "capsule", "capsule.msh");

        renderer->LoadMesh(meshes, "role", "Role_T.msh");
        Roleanim = std::make_shared<MeshAnimation>("Role_T_Idle.anm");
        Moveanim = std::make_shared<MeshAnimation>("Role_T.anm");
        Rolemat = std::make_shared<MeshMaterial>("Role_T.mat");

        renderer->LoadTexture(textures, "basic", "checkerboard.png");
        renderer->LoadTexture(textures, "floor", "6x6_base.png");

        renderer->LoadTexture(textures, "texture1_diffuse", "Texture_1_Diffuse.png");
        renderer->LoadTexture(textures, "texture1_specular", "Texture_1_Specular.png");
        renderer->LoadTexture(textures, "texture1_normal", "Texture_1_Normal.png");

        renderer->LoadTexture(textures, "texture2_diffuse", "Texture_2_Diffuse.png");
        renderer->LoadTexture(textures, "texture2_specular", "Texture_2_Specular.png");
        renderer->LoadTexture(textures, "texture2_normal", "Texture_2_Normal.png");

        renderer->LoadTexture(textures, "texture3_diffuse", "Texture_3_Diffuse.png");
        renderer->LoadTexture(textures, "texture3_specular", "Texture_3_Specular.png");
        renderer->LoadTexture(textures, "texture3_normal", "Texture_3_Normal.png");

        renderer->LoadTexture(textures, "texture4_diffuse", "Texture_4_Diffuse.png");
        renderer->LoadTexture(textures, "texture4_specular", "Texture_4_Specular.png");
        renderer->LoadTexture(textures, "texture4_normal", "Texture_4_Normal.png");

        renderer->LoadTexture(textures, "texture5_normal", "Texture_5_Normal.png");
        renderer->LoadTexture(textures, "texture5_diffuse", "Texture_5_Diffuse.png");
        renderer->LoadTexture(textures, "texture5_specular", "Texture_5_Specular.png");

        //Map2
        renderer->LoadTexture(textures, "texture6_base", "Texture_6_Base.png");  //GROUND 8
        renderer->LoadTexture(textures, "texture6_normal", "Texture_6_Normal.png");
        renderer->LoadTexture(textures, "texture6_height", "Texture_6_Height.png");
        renderer->LoadTexture(textures, "texture6_metallic", "Texture_6_Metallic.png");
        renderer->LoadTexture(textures, "texture6_occlusion", "Texture_6_Occlusion.png"); // base, height, metallic, occlusion, normal (3)

        renderer->LoadTexture(textures, "texture7_base", "Texture_7_Base.png");  //OW T03
        renderer->LoadTexture(textures, "texture7_normal", "Texture_7_Normal.png");
        renderer->LoadTexture(textures, "texture7_height", "Texture_7_Height.png");
        renderer->LoadTexture(textures, "texture7_occlusion", "Texture_7_Occlusion.png");
        renderer->LoadTexture(textures, "texture7_maskmap", "Texture_7_MaskMap.png");
        renderer->LoadTexture(textures, "texture7_roughness", "Texture_7_Roughness.png"); // base, height, metallic, occlusion, mask map, normal (4)


        renderer->LoadTexture(textures, "texture8_base", "Texture_8_Base.png");   //OW T06
        renderer->LoadTexture(textures, "texture8_normal", "Texture_8_Normal.png");
        renderer->LoadTexture(textures, "texture8_height", "Texture_8_Height.png");
        renderer->LoadTexture(textures, "texture8_occlusion", "Texture_8_Occlusion.png");
        renderer->LoadTexture(textures, "texture8_maskmap", "Texture_8_MaskMap.png");
        renderer->LoadTexture(textures, "texture8_roughness", "Texture_8_Roughness.png"); // base, height, metallic, occlusion, mask map, normal (4)

        renderer->LoadTexture(textures, "texture9_base", "Texture_9_Base.png");   //OW T14
        renderer->LoadTexture(textures, "texture9_normal", "Texture_9_Normal.png");
        renderer->LoadTexture(textures, "texture9_height", "Texture_9_Height.png");
        renderer->LoadTexture(textures, "texture9_occlusion", "Texture_9_Occlusion.png");
        renderer->LoadTexture(textures, "texture9_maskmap", "Texture_9_MaskMap.png");
        renderer->LoadTexture(textures, "texture9_roughness", "Texture_9_Roughness.png");// base, height, metallic, occlusion, mask map, normal (4)

        renderer->LoadTexture(textures, "texture10_base", "Texture_10_Base.png");  //OW T15
        renderer->LoadTexture(textures, "texture10_normal", "Texture_10_Normal.png");
        renderer->LoadTexture(textures, "texture10_height", "Texture_10_Height.png");
        renderer->LoadTexture(textures, "texture10_occlusion", "Texture_10_Occlusion.png");
        renderer->LoadTexture(textures, "texture10_maskmap", "Texture_10_MaskMap.png");
        renderer->LoadTexture(textures, "texture10_roughness", "Texture_10_Roughness.png");// base, height, metallic, occlusion, mask map, normal (4)

        renderer->LoadTexture(textures, "texture11_base", "Texture_11_Base.png");  //WALL 2
        renderer->LoadTexture(textures, "texture11_height", "Texture_11_Height.png");
        renderer->LoadTexture(textures, "texture11_metallic", "Texture_11_Metallic.png");
        renderer->LoadTexture(textures, "texture11_occlusion", "Texture_11_Occlusion.png");
        renderer->LoadTexture(textures, "texture11_normal", "Texture_11_Normal.png"); // base, height, metallic, occlusion, mask map, normal (3)

        renderer->LoadTexture(textures, "texture12_base", "Texture_12_Base.png");   //WALL 5
        renderer->LoadTexture(textures, "texture12_normal", "Texture_12_Normal.png");
        renderer->LoadTexture(textures, "texture12_height", "Texture_12_Height.png");
        renderer->LoadTexture(textures, "texture12_metallic", "Texture_12_Metallic.png");
        renderer->LoadTexture(textures, "texture12_occlusion", "Texture_12_Occlusion.png"); // base, height, metallic, occlusion, mask map, normal (3)

        renderer->LoadTexture(textures, "texture13_base", "Texture_13_Base.png");  //WALL 8
        renderer->LoadTexture(textures, "texture13_normal", "Texture_13_Normal.png");
        renderer->LoadTexture(textures, "texture13_height", "Texture_13_Height.png");
        renderer->LoadTexture(textures, "texture13_metallic", "Texture_13_Metallic.png");
        renderer->LoadTexture(textures, "texture13_occlusion", "Texture_13_Occlusion.png"); // base, height, metallic, occlusion, mask map, normal (3)

        renderer->LoadTexture(textures, "texture14_base", "Texture_14_Base.png");   //WOOD
        renderer->LoadTexture(textures, "texture14_normal", "Texture_14_Normal.png");
        renderer->LoadTexture(textures, "texture14_height", "Texture_14_Height.png");
        renderer->LoadTexture(textures, "texture14_metallic", "Texture_14_Metallic.png");
        renderer->LoadTexture(textures, "texture14_occlusion", "Texture_14_Occlusion.png"); // base, height, metallic, occlusion, mask map, normal (3)

        renderer->LoadShader(shaders, "basic", "scene.vert", "scene.frag");

        //build ID list at the end
        for (const auto& pair : meshes) {
            meshID.push_back(pair.first);
        }        
        for (const auto& pair : textures) {
            textureID.push_back(pair.first);
        }        
        for (const auto& pair : shaders) {
            shaderID.push_back(pair.first);
        }
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

int ResourceManager::GetResourceIdByString(const std::string& type, const std::string& target)
{
    if (type == "mesh") {
        for (int i = 0; i < meshID.size(); ++i) {
            if (meshID[i] == target) {
                return i;
            }
        }
    }
    else if (type == "texture") {
        for (int i = 0; i < textureID.size(); ++i) {
            if (textureID[i] == target) {
                return i;
            }
        }
    }
    else if (type == "shader")
    {
        for (int i = 0; i < shaderID.size(); ++i) {
            if (shaderID[i] == target) {
                return i;
            }
        }
    }
    else
    {
        std::cout << "[ResourceManager::findIdByString] Cannot find type[" << type << "] - target[" << target << "]" << std::endl;
    }
    return -1;
}

std::string ResourceManager::GetResourceByID(const std::string& type, const int& id)
{
    if (id == -1) return "";

    if (type == "mesh") {
        return meshID[id];
    }
    else if (type == "texture") {
        return textureID[id];

    }
    else if (type == "shader")
    {
        return shaderID[id];
    }
    else
    {
        std::cout << "[ResourceManager::GetResourceByID] Unknown type : [" << type << "]" << std::endl;
        return "";
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

