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

            MouseLeftClick,
        };

        class EventManager {
        public:
            using EventCallback = std::function<void()>;
            using EventCallbackInt = std::function<void(int&)>;
            using EventCallbackString = std::function<void(std::string&)>;
            using EventCallbackFloat2 = std::function<void(float&, float&)>;

            static void Subscribe(EventType eventType, EventCallback callback);
            static void Subscribe(EventType eventType, EventCallbackInt callback);
            static void Subscribe(EventType eventType, EventCallbackString callback);
            static void Subscribe(EventType eventType, EventCallbackFloat2 callback);

            static void Trigger(EventType eventType);
            static void Trigger(EventType eventType, int& t);
            static void Trigger(EventType eventType, std::string& s);
            static void Trigger(EventType eventType, float& a, float& b);

        private:
            static std::unordered_map<EventType, std::vector<EventCallback>> eventListeners;
            static std::unordered_map<EventType, std::vector<EventCallbackInt>> eventListenersInt;
            static std::unordered_map<EventType, std::vector<EventCallbackString>> eventListenersString;
            static std::unordered_map<EventType, std::vector<EventCallbackFloat2>> eventListenersFloat2;
        };
    }
}
