#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "ISaveable.h"
#include "EventManager.h"

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

			void Init();

			json LoadData(std::string file = "save") const;

			void JsonSaveTest();
			void JsonLoadTest() const;

			void RegisterISaveable(ISaveable* saveable);
			void UnRegisterISaveable(ISaveable* saveable);

		private:

			void SaveAll();
			void LoadAll();

			SLSystem() = default;
			~SLSystem();

			std::string getCurrentTime();

			std::string savePath = "../Assets/Json/";
			std::list<ISaveable*> saveableList;
			json jsonData;
		};
	}
}

