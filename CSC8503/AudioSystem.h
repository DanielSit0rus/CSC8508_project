#pragma once
#include <fmod_studio.hpp>
#include <iostream>
#include <unordered_map>

#include "GameManager.h"
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

			void SetBusVolume(const std::string& busName, float v);

			bool TriggerEvent(const std::string& eventName,rp3d::Vector3 pos);
			bool TriggerEvent(const std::string& eventName);

			bool PlayEvent(const std::string& eventName);
			void StopEvent(const std::string& eventName);
			void PauseEvent(const std::string& eventName);
			void ResumeEvent(const std::string& eventName);

			void PauseEvent(EventInstance* event);
			void ResumeEvent(EventInstance* event);

			EventInstance* GetEvent(std::string eventName);

		private:

			AudioSystem() = default;
			~AudioSystem();

			bool LoadBank(const std::string& bankFile);
			bool LoadBus(const std::string& busName);

			void SaveData(nlohmann::json& j) {
				std::cout << "[AudioSystem] Saved" << std::endl;
			}
			void LoadData(nlohmann::json& j) {
				std::cout << "[AudioSystem] Loaded" << std::endl;
			}

			Studio::System* studioSystem = nullptr;
			FMOD::System* coreSystem = nullptr;

			const std::string bankPath = "../Assets/Sounds/Banks/";
			std::unordered_map<std::string, Bank*> banks;
			std::unordered_map<std::string, Bus*> buses;
			std::unordered_map<std::string, EventDescription*> eventDescriptions;
			std::unordered_map<std::string, EventInstance*> events;	//global events (e.g. BGM)
			//std::unordered_map<std::string, std::string> eventPaths;  // map from name to path

			FMOD_3D_ATTRIBUTES* listenerAttributes = nullptr;
			FMOD_3D_ATTRIBUTES* triggerAttributes = nullptr;
		};
	}
}