#pragma once
#include <nlohmann/json.hpp>

using json = nlohmann::json;  // 定义一个简化名称

namespace NCL {
    namespace CSC8503 {
        class ISaveable {
        public:
            virtual ~ISaveable() = default;

            virtual void RegisterSL();    // Remember to register
            virtual void UnRegisterSL();

            virtual void SaveData(json& j) = 0;
            virtual void LoadData(json& j) = 0;
        };
    }
}