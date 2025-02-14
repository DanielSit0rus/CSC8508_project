#include "SLSystem.h"
using namespace NCL::CSC8503;
using namespace NCL;

SLSystem::~SLSystem()
{
    for (auto* item : saveableList) {
        delete item;
    }
}

void NCL::CSC8503::SLSystem::JsonSave() {
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
    std::ofstream out_file("../Assets/Json/output_data.json");
    out_file << j.dump(4);  // 格式化输出（4个空格的缩进）
    out_file.close();

    std::cout << "JSON written to person_data.json" << std::endl;
}

void NCL::CSC8503::SLSystem::JsonLoad()
{
    // 从文件读取 JSON 数据
    std::ifstream in_file("../Assets/Json/output_data.json");
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
