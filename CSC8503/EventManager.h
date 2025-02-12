#pragma once

#include <unordered_map>
#include <functional>
#include <vector>

namespace NCL {
    namespace CSC8503 {
        enum class GameEventType {
            Game_Start,
            Game_Pause,
            Game_Resume,
            Game_End,
        };

        class EventManager {
        public:
            using EventCallback = std::function<void()>;

            static void Subscribe(GameEventType eventType, EventCallback callback);
            static void Trigger(GameEventType eventType);

        private:
            static std::unordered_map<GameEventType, std::vector<EventCallback>> eventListeners;
        };
    }
}
