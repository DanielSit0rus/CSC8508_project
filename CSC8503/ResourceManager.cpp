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

        //Map2
        textures["texture6_base"] = renderer->LoadTexture("Texture_6_Albedo.png");  //GROUND 8
        textures["texture6_height"] = renderer->LoadTexture("Texture_6_Height.png");
        textures["texture6_metallic"] = renderer->LoadTexture("Texture_6_Metallic.png");
        textures["texture6_occlusion"] = renderer->LoadTexture("Texture_6_Occlusion.png");
        textures["texture6_normal"] = renderer->LoadTexture("Texture_6_Normal.png");

        textures["texture7_base"] = renderer->LoadTexture("Texture_7_Base.png");  //OW T03
        textures["texture7_height"] = renderer->LoadTexture("Texture_7_Height.png");
        textures["texture7_occlusion"] = renderer->LoadTexture("Texture_7_Occlusion.png");
        textures["texture7_maskmap"] = renderer->LoadTexture("Texture_7_MaskMap.png");
        textures["texture7_roughness"] = renderer->LoadTexture("Texture_7_Roughness.png");
        textures["texture7_normal"] = renderer->LoadTexture("Texture_7_Normal.png");

        textures["texture8_base"] = renderer->LoadTexture("Texture_8_Base.png");   //OW T06
        textures["texture8_height"] = renderer->LoadTexture("Texture_8_Height.png");
        textures["texture8_occlusion"] = renderer->LoadTexture("Texture_8_Occlusion.png");
        textures["texture8_maskmap"] = renderer->LoadTexture("Texture_8_MaskMap.png");
        textures["texture8_roughness"] = renderer->LoadTexture("Texture_8_Roughness.png");
        textures["texture8_normal"] = renderer->LoadTexture("Texture_8_Normal.png");

        textures["texture9_base"] = renderer->LoadTexture("Texture_9_Base.png");   //OW T14
        textures["texture9_height"] = renderer->LoadTexture("Texture_9_Height.png");
        textures["texture9_occlusion"] = renderer->LoadTexture("Texture_9_Occlusion.png");
        textures["texture9_maskmap"] = renderer->LoadTexture("Texture_9_MaskMap.png");
        textures["texture9_roughness"] = renderer->LoadTexture("Texture_9_Normal.png");
        textures["texture9_normal"] = renderer->LoadTexture("Texture_9_Roughness.png");

        textures["texture10_base"] = renderer->LoadTexture("Texture_10_Base.png");  //OW T15
        textures["texture10_height"] = renderer->LoadTexture("Texture_10_Height.png");
        textures["texture10_occlusion"] = renderer->LoadTexture("Texture_10_Occlusion.png");
        textures["texture10_maskmap"] = renderer->LoadTexture("Texture_10_MaskMap.png");
        textures["texture10_roughness"] = renderer->LoadTexture("Texture_10_Roughness.png");
        textures["texture10_normal"] = renderer->LoadTexture("Texture_10_Normal.png");

        textures["texture11_base"] = renderer->LoadTexture("Texture_11_Base.png");  //WALL 2
        textures["texture11_height"] = renderer->LoadTexture("Texture_11_Height.png");
        textures["texture11_metallic"] = renderer->LoadTexture("Texture_11_Metallic.png");
        textures["texture11_occlusion"] = renderer->LoadTexture("Texture_11_Occlusion.png");
        textures["texture11_normal"] = renderer->LoadTexture("Texture_11_Normal.png");

        textures["texture12_base"] = renderer->LoadTexture("Texture_12_Base.png");   //WALL 5
        textures["texture12_height"] = renderer->LoadTexture("Texture_12_Height.png");
        textures["texture12_metallic"] = renderer->LoadTexture("Texture_12_Metallic.png");
        textures["texture12_occlusion"] = renderer->LoadTexture("Texture_12_Occlusion.png");
        textures["texture12_normal"] = renderer->LoadTexture("Texture_12_Normal.png");

        textures["texture13_base"] = renderer->LoadTexture("Texture_13_Base.png");  //WALL 8
        textures["texture13_height"] = renderer->LoadTexture("Texture_13_Height.png");
        textures["texture13_metallic"] = renderer->LoadTexture("Texture_13_Metallic.png");
        textures["texture13_occlusion"] = renderer->LoadTexture("Texture_13_Occlusion.png");
        textures["texture13_normal"] = renderer->LoadTexture("Texture_13_Normal.png");

        textures["texture14_base"] = renderer->LoadTexture("Texture_14_Base.png");   //WOOD
        textures["texture14_height"] = renderer->LoadTexture("Texture_14_Height.png");
        textures["texture14_metallic"] = renderer->LoadTexture("Texture_14_Metallic.png");
        textures["texture14_occlusion"] = renderer->LoadTexture("wood_10_occlusion.png");
        textures["texture14_normal"] = renderer->LoadTexture("Texture_14_Normal.png");

        shaders["basic"] = renderer->LoadShader("scene.vert", "scene.frag");


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

