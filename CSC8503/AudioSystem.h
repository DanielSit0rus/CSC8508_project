#pragma once
#include <fmod_studio.hpp>
#include <iostream>
#include <unordered_map>

#include "EventManager.h"
#include "ISaveable.h"

#pragma comment(lib, "fmodstudio_vc.lib")

using namespace FMOD;
using namespace FMOD::Studio;

namespace NCL {
	namespace CSC8503 {
		class AudioSystem : public ISaveable {
		public:
			static AudioSystem& GetInstance() {
				static AudioSystem instance;
				return instance;
			}
			AudioSystem(const AudioSystem&) = delete;
			AudioSystem& operator=(const AudioSystem&) = delete;

			bool Init();
			void Update();
			void Release();

			bool PlayEvent(const std::string& eventName);
			void StopEvent(const std::string& eventName);
			void PauseEvent(const std::string& eventName);
			void ResumeEvent(const std::string& eventName);

			void PauseEvent(EventInstance* event);
			void ResumeEvent(EventInstance* event);

			//3d test
			Studio::System* studioSystem = nullptr;
			FMOD::System* coreSystem = nullptr;
			EventInstance* eventInstance = nullptr;
			FMOD_3D_ATTRIBUTES* sourceAttributes = nullptr;

		private:

			AudioSystem() = default;
			~AudioSystem();

			bool LoadBank(const std::string& bankFile);
			bool LoadBus(const std::string& busName);

			void SaveData(nlohmann::json j) {
				std::cout << "[AudioSystem] Saved" << std::endl;
			}
			void LoadData(nlohmann::json j) {
				std::cout << "[AudioSystem] Loaded" << std::endl;
			}

			const std::string bankPath = "../Assets/Sounds/Banks/";
			std::unordered_map<std::string, Bank*> banks;
			std::unordered_map<std::string, Bus*> buses;
			std::unordered_map<std::string, EventInstance*> events;
			//std::unordered_map<std::string, std::string> eventPaths;  // 存储事件名称到路径的映射
		};
	}
}