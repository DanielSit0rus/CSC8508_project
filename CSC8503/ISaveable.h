#pragma once
#include <nlohmann/json.hpp>

namespace NCL {
    namespace CSC8503 {
        class ISaveable {
        public:
            virtual ~ISaveable() = default;

            virtual void RegisterSL();    // Remember to register
            virtual void UnRegisterSL();

            virtual void SaveData(nlohmann::json j) = 0;
            virtual void LoadData(nlohmann::json j) = 0;
        };
    }
}