#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
#include "ISaveable.h"
#include "EventManager.h"

using json = nlohmann::json;  // ����һ��������

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

			void JsonSave();
			void JsonLoad();

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
			nlohmann::json jsonData;
		};
	}
}

