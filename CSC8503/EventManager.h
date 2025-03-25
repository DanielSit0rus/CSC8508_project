#pragma once

#include <unordered_map>
#include <functional>
#include <vector>

namespace NCL {
    namespace CSC8503 {
        enum class EventType {
			Loading_Start,

            MainMenu_Start,
            MainMenu_Quit,

            Game_Start,
            Game_Pause,
            Game_Resume,
            Game_End,



            Data_Save,
            Data_Load,

            Network_StartAsServer,
            Network_StartAsClient,
            Network_Test,
            Network_Connected,
            Network_Disconnected,
        };

        class EventManager {
        public:
            using EventCallback = std::function<void()>;
            using EventCallbackInt = std::function<void(int&)>;
            using EventCallbackString = std::function<void(std::string&)>;

            static void Subscribe(EventType eventType, EventCallback callback);
            static void Subscribe(EventType eventType, EventCallbackInt callback);
            static void Subscribe(EventType eventType, EventCallbackString callback);

            static void Trigger(EventType eventType);
            static void Trigger(EventType eventType, int& t);
            static void Trigger(EventType eventType, std::string& s);

        private:
            static std::unordered_map<EventType, std::vector<EventCallback>> eventListeners;
            static std::unordered_map<EventType, std::vector<EventCallbackInt>> eventListenersInt;
            static std::unordered_map<EventType, std::vector<EventCallbackString>> eventListenersString;
        };
    }
}
