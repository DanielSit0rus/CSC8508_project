#include "EventManager.h"

using namespace NCL;
using namespace CSC8503;

std::unordered_map<GameEventType, std::vector<EventManager::EventCallback>> EventManager::eventListeners;

void EventManager::Subscribe(GameEventType eventType, EventCallback callback) {
    eventListeners[eventType].push_back(callback);
}

void EventManager::Trigger(GameEventType eventType) {
    for (auto& callback : eventListeners[eventType]) {
        callback();
    }
}