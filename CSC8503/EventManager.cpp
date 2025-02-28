#include "EventManager.h"

using namespace NCL;
using namespace CSC8503;

std::unordered_map<EventType, std::vector<EventManager::EventCallback>> EventManager::eventListeners;
std::unordered_map<EventType, std::vector<EventManager::EventCallbackInt>> EventManager::eventListenersInt;
std::unordered_map<EventType, std::vector<EventManager::EventCallbackString>> EventManager::eventListenersString;

void EventManager::Subscribe(EventType eventType, EventCallback callback) {
    eventListeners[eventType].push_back(callback);
}

void EventManager::Subscribe(EventType eventType, EventCallbackInt callback)
{
    eventListenersInt[eventType].push_back(callback);
}

void EventManager::Subscribe(EventType eventType, EventCallbackString callback)
{
    eventListenersString[eventType].push_back(callback);
}

void EventManager::Trigger(EventType eventType) {
    for (auto& callback : eventListeners[eventType]) {
        callback();
    }
}

void NCL::CSC8503::EventManager::Trigger(EventType eventType, int& t)
{
    for (auto& callback : eventListenersInt[eventType]) {
        callback(t);
    }
}

void NCL::CSC8503::EventManager::Trigger(EventType eventType, std::string& t)
{
    for (auto& callback : eventListenersString[eventType]) {
        callback(t);
    }
}
