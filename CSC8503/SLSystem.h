#pragma once
#include <iostream>
#include <fstream>
#include <nlohmann/json.hpp>
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
		private:
			SLSystem() = default;
			~SLSystem() = default;

		};
	}
}

