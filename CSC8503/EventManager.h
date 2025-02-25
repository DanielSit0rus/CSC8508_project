#pragma once

#include <unordered_map>
#include <functional>
#include <vector>

namespace NCL {
    namespace CSC8503 {
        enum class EventType {
            Game_Start,
            Game_Pause,
            Game_Resume,
            Game_End,

            Data_Save,
            Data_Load,

            Network_StartAsServer,
            Network_StartAsClient,
            Network_Test,
        };

        class EventManager {
        public:
            using EventCallback = std::function<void()>;
            using EventCallbackInt = std::function<void(int&)>;

            static void Subscribe(EventType eventType, EventCallback callback);
            static void Subscribe(EventType eventType, EventCallbackInt callback);

            static void Trigger(EventType eventType);
            static void Trigger(EventType eventType, int& t);

        private:
            static std::unordered_map<EventType, std::vector<EventCallback>> eventListeners;
            static std::unordered_map<EventType, std::vector<EventCallbackInt>> eventListenersInt;
        };
    }
}
