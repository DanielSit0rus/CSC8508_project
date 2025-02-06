#pragma once
#include <fmod_studio.hpp>
#include <iostream>
#include <unordered_map>

#include "EventManager.h"

#pragma comment(lib, "fmodstudio_vc.lib")

namespace NCL {
	namespace CSC8503 {
		class AudioSystem {
		public:
			static AudioSystem& GetInstance() {
				static AudioSystem instance;
				return instance;
			}

			bool Init();
			void Update();
			void Release();

			bool PlayEvent(const std::string& eventName);
			void StopEvent(const std::string& eventName);
			void PauseEvent(const std::string& eventName);
			void ResumeEvent(const std::string& eventName);

		private:

			AudioSystem() = default;
			~AudioSystem();

			bool LoadBank(const std::string& bankFile);
			bool LoadBus(const std::string& busName);

			FMOD::Studio::System* studioSystem = nullptr;

			const std::string bankPath = "../Assets/Sounds/Banks/";
			std::unordered_map<std::string, FMOD::Studio::Bank*> banks;
			std::unordered_map<std::string, FMOD::Studio::Bus*> buses;
			std::unordered_map<std::string, FMOD::Studio::EventInstance*> events;
			//std::unordered_map<std::string, std::string> eventPaths;  // 存储事件名称到路径的映射
		};
	}
}