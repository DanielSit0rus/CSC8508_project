#include "AudioSystem.h"

using namespace NCL;
using namespace CSC8503;

AudioSystem::~AudioSystem() {
    Release();
}

bool AudioSystem::Init()
{
    FMOD::Studio::System::create(&studioSystem);
    if (!studioSystem) {
        std::cout << "FMOD Studio 创建失败！" << std::endl;
        return false;
    }
    studioSystem->initialize(1024, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_3D_RIGHTHANDED, nullptr);

    LoadBank("Master.bank");
    LoadBank("Master.strings.bank");
    LoadBank("BGM.bank");
    LoadBank("Lin.bank");
    LoadBus("BGM");

    buses["BGM"]->setVolume(0.15f);

    /*
    EventManager::Subscribe(GameEventType::Game_Start, [this]() {PlayEvent("event:/BGM/BGM1"); });
    EventManager::Subscribe(GameEventType::Game_Pause, [this]() {PauseEvent("event:/BGM/BGM1"); });
    EventManager::Subscribe(GameEventType::Game_Resume, [this]() {ResumeEvent("event:/BGM/BGM1"); });
    EventManager::Subscribe(GameEventType::Game_End, [this]() {StopEvent("event:/BGM/BGM1"); });
    */

    return true;
}

void AudioSystem::Update()
{
    studioSystem->update();
}

void AudioSystem::Release()
{
    for (auto& [_, bank] : banks) {
        bank->unload();
    }
    banks.clear();

    for (auto& [_, event] : events) {
        event->release();
    }

    events.clear();

    buses.clear();
    if (studioSystem) {
        studioSystem->release();
        studioSystem = nullptr;
    }
}


bool AudioSystem::LoadBank(const std::string& bankFile)
{
    FMOD::Studio::Bank* bank = nullptr;
    if (studioSystem->loadBankFile((bankPath + bankFile).c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank) != FMOD_OK) {
        std::cerr << "加载Bank失败：" << bankFile << std::endl;
        return false;
    }
    banks[bankFile] = bank;
    return true;
}

bool AudioSystem::LoadBus(const std::string& busName) {
    FMOD::Studio::Bus* bus = nullptr;

    FMOD_RESULT result = studioSystem->getBus(("bus:/" + busName).c_str(), &bus);

    if (result != FMOD_OK || bus == nullptr) {
        std::cerr << "Failed to load bus: " << busName << std::endl;
        return false;
    }

    buses[busName] = bus;
    return true;
}


bool AudioSystem::PlayEvent(const std::string& eventName) {
    auto it = events.find(eventName);
    if (it != events.end()) {
        it->second->start();
        return true;
    }

    FMOD::Studio::EventDescription* eventDesc = nullptr;
    if (studioSystem->getEvent(eventName.c_str(), &eventDesc) != FMOD_OK || !eventDesc) {
        std::cerr << "获取事件失败：" << eventName << std::endl;
        return false;
    }

    FMOD::Studio::EventInstance* eventInstance = nullptr;
    if (eventDesc->createInstance(&eventInstance) != FMOD_OK || !eventInstance) {
        std::cerr << "创建事件实例失败：" << eventName << std::endl;
        return false;
    }

    eventInstance->start();

    events[eventName] = eventInstance;
    return true;
}

void AudioSystem::StopEvent(const std::string& eventName) {
    if (events.count(eventName)) {
        events[eventName]->stop(FMOD_STUDIO_STOP_ALLOWFADEOUT);
    }
}

void AudioSystem::PauseEvent(const std::string& eventName) {
    if (events.count(eventName)) {
        FMOD::Studio::EventInstance* eventInstance = events[eventName];

        bool state;
        eventInstance->getPaused(&state);
        if (state == false) {
            eventInstance->setPaused(true);
        }
    }
}

void AudioSystem::ResumeEvent(const std::string& eventName) {
    if (events.count(eventName)) {
        FMOD::Studio::EventInstance* eventInstance = events[eventName];

        bool state;
        eventInstance->getPaused(&state);
        if (state == true) {
            eventInstance->setPaused(false);
        }
    }
}