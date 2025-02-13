#include "AudioSystem.h"

using namespace NCL;
using namespace CSC8503;

AudioSystem::~AudioSystem() {
    Release();
}

bool AudioSystem::Init()
{
    FMOD_RESULT result;
    result = FMOD::Studio::System::create(&studioSystem);
    if (result != FMOD_OK) {
        std::cerr << "FMOD Studio System creation failed: " << result << std::endl;
        return false;
    }

    result = studioSystem->initialize(512, FMOD_STUDIO_INIT_NORMAL, FMOD_INIT_NORMAL, nullptr);
    if (result != FMOD_OK) {
        std::cerr << "FMOD Studio System initialization failed: " << result << std::endl;
        return false;
    }

    result = studioSystem->getCoreSystem(&coreSystem);
    if (result != FMOD_OK) {
        std::cerr << "Failed to get FMOD Core System: " << result << std::endl;
        return false;
    }

    LoadBank("Master.bank");
    LoadBank("Master.strings.bank");
    LoadBank("BGM.bank");
    LoadBank("Lin.bank");
    LoadBus("BGM");

    buses["BGM"]->setVolume(1.0f);

    /*
    EventManager::Subscribe(EventType::Game_Start, [this]() {PlayEvent("event:/BGM/BGM1"); });
    EventManager::Subscribe(EventType::Game_Pause, [this]() {PauseEvent("event:/BGM/BGM1"); });
    EventManager::Subscribe(EventType::Game_Resume, [this]() {ResumeEvent("event:/BGM/BGM1"); });
    EventManager::Subscribe(EventType::Game_End, [this]() {StopEvent("event:/BGM/BGM1"); });
    */

    //EventManager::Subscribe(EventType::Game_Start, [this](int& a) {a = 2; });

    FMOD::Studio::EventDescription* eventDescription = nullptr;
    result = studioSystem->getEvent("event:/BGM/BGM1_3D", &eventDescription);
    if (result != FMOD_OK) {
        std::cerr << "Failed to get event description: " << result << std::endl;
        return false;
    }

    result = eventDescription->createInstance(&eventInstance);
    if (result != FMOD_OK) {
        std::cerr << "Failed to create event instance: " << result << std::endl;
        return false;
    }

    // 设置3D属性
    sourceAttributes = new FMOD_3D_ATTRIBUTES();
    sourceAttributes->position = { 0.0f, 0.0f, 0.0f }; // 设置音源位置
    sourceAttributes->velocity = { 0.0f, 0.0f, 0.0f }; // 设置音源速度
    sourceAttributes->forward = { 0.0f, 0.0f, 1.0f };  // 设置音源前方方向
    sourceAttributes->up = { 0.0f, 1.0f, 0.0f };       // 设置音源上方方向

    result = eventInstance->set3DAttributes(sourceAttributes);
    if (result != FMOD_OK) {
        std::cerr << "Failed to set 3D attributes: " << result << std::endl;
        return false;
    }

    result = eventInstance->start();
    if (result != FMOD_OK) {
        std::cerr << "Failed to start event: " << result << std::endl;
        return false;
    }

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