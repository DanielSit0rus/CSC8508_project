#include "SLSystem.h"
using namespace NCL::CSC8503;
using namespace NCL;

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
}

void SLSystem::LoadAll() {
    std::cout << "\n\n========================="<< saveableList.size()<<"==============================\n\n";

    for (const auto& item : saveableList) {
        item->LoadData(jsonData);
    }
}

void NCL::CSC8503::SLSystem::JsonSave() {
    // ���� JSON ����
    nlohmann::json j;

    // �������
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

    // ���ļ���д�� JSON ����
    std::ofstream out_file("../Assets/Json/output_data.json");
    out_file << j.dump(4);  // ��ʽ�������4���ո��������
    out_file.close();

    std::cout << "JSON written to person_data.json" << std::endl;
}

void NCL::CSC8503::SLSystem::JsonLoad()
{
    // ���ļ���ȡ JSON ����
    std::ifstream in_file("../Assets/Json/output_data.json");
    if (!in_file) {
        std::cerr << "Error opening file!" << std::endl;
        return;
    }

    // ���� JSON ����
    nlohmann::json j;
    in_file >> j;

    // ���ʺ��������
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
