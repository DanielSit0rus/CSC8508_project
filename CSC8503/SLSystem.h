#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "ISaveable.h"

using json = nlohmann::json;  // 定义一个简化名称
namespace NCL {
	namespace CSC8503 {
		class SLSystem
		{
		public:
			static SLSystem& GetInstance() {
				static SLSystem instance;
				return instance;	
			}
			SLSystem(const SLSystem&) = delete;
			SLSystem& operator=(const SLSystem&) = delete;

			void JsonSave();
			void JsonLoad();

			void RegisterSaveData(ISaveable* saveable) {
				if (std::find(saveableList.begin(), saveableList.end(), saveable) == saveableList.end()) {
					saveableList.push_back(saveable);
				}
			}

			void UnRegisterSaveData(ISaveable* saveable) {
				auto it = std::find(saveableList.begin(), saveableList.end(), saveable);
				if (it != saveableList.end()) {
					saveableList.erase(it);
				}
			}

		private:

			void SaveAll() {
				for (const auto& item : saveableList) {
					item->SaveData();
				}
			}

			void LoadAll() {
				for (const auto& item : saveableList) {
					item->LoadData();
				}
			}

			SLSystem() = default;
			~SLSystem();

			std::list<ISaveable*> saveableList;
		};
	}
}

