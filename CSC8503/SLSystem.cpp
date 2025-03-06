#include "SLSystem.h"
using namespace NCL::CSC8503;

SLSystem::~SLSystem()
{
    saveableList.clear();
}

void SLSystem::Init()
{
    EventManager::Subscribe(EventType::Data_Save, [this]() {SaveAll(); });
    EventManager::Subscribe(EventType::Data_Load, [this]() {LoadAll(); });
}

void SLSystem::RegisterISaveable(ISaveable* saveable) {
    if (std::find(saveableList.begin(), saveableList.end(), saveable) == saveableList.end()) {
        saveableList.push_back(saveable);
    }
}

void SLSystem::UnRegisterISaveable(ISaveable* saveable) {
    auto it = std::find(saveableList.begin(), saveableList.end(), saveable);
    if (it != saveableList.end()) {
        saveableList.erase(it);
    }
}

void SLSystem::SaveAll() {
    for (const auto& item : saveableList) {
        item->SaveData(jsonData);
    }

    std::string filename = savePath + "save_" + getCurrentTime() + ".json";
    std::ofstream out_file(filename);
    out_file << jsonData.dump(4);  // 格式化输出（4个空格的缩进）
    out_file.close();

    std::cout << "JSON written to " << filename << std::endl;
    std::cout << "[SLSystem] Save finished" << std::endl;
}

void SLSystem::LoadAll() {
    std::cout << "[SLSystem] saveableList.size() = " << saveableList.size() <<std::endl;
    for (const auto& item : saveableList) {
        item->LoadData(jsonData);
    }
    std::cout << "[SLSystem] Load finished" << std::endl;
}

void SLSystem::JsonSave() {
    // 创建 JSON 对象
    nlohmann::json j;

    // 填充数据
    j["person"] = {
        {"name", "john"},
        {"age", 30},
        {"address", {
            {"street", "123 Main St"},
            {"city", "Anytown"},
            {"zipcode", "12345"}
        }},
        {"contacts", {
            {"email", "john@example.com"},
            {"phone", "123-456-7890"}
        }},
        {"hobbies", {"reading", "coding", "hiking"}}
    };

    // 打开文件并写入 JSON 数据
    std::string filename = savePath + "save_" + getCurrentTime() + ".json";
    std::ofstream out_file(filename);
    out_file << j.dump(4);  // 格式化输出（4个空格的缩进）
    out_file.close();

    std::cout << "[Test] JSON written to " << filename << std::endl;
}

void SLSystem::JsonLoad()
{
    // 从文件读取 JSON 数据
    std::ifstream in_file(savePath + "output_data.json");
    if (!in_file) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // 解析 JSON 数据
    nlohmann::json j;
    in_file >> j;

    // 访问和输出数据
    std::cout << "Name: " << j["person"]["name"] << std::endl;
    std::cout << "Age: " << j["person"]["age"] << std::endl;
    std::cout << "City: " << j["person"]["address"]["city"] << std::endl;
    std::cout << "Email: " << j["person"]["contacts"]["email"] << std::endl;
    std::cout << "Hobbies: ";
    for (const auto& hobby : j["person"]["hobbies"]) {
        std::cout << hobby << " ";
    }
    std::cout << std::endl;
}


#include <iostream>
#include <chrono>
#include <iomanip>

std::string SLSystem::getCurrentTime() {
    auto now = std::chrono::system_clock::now();
    std::time_t now_c = std::chrono::system_clock::to_time_t(now);
    std::tm localTime;
    localtime_s(&localTime, &now_c);
    std::ostringstream oss;
    oss << std::put_time(&localTime, "%Y-%m-%d_%H-%M-%S");
    return oss.str();
}
