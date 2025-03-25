#include "AudioSystem.h"

using namespace NCL::CSC8503;

AudioSystem::~AudioSystem() {
    Release();
}

bool AudioSystem::Init()
{
    FMOD_RESULT result;
    result = Studio::System::create(&studioSystem);
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
    LoadBank("Effect.bank");
    LoadBank("Felicia.bank");
    LoadBank("Congzheng.bank");

    LoadBus("BGM");
    LoadBus("Voice");
    LoadBus("Effect");

    SetBusVolume("BGM", 1);
    SetBusVolume("Voice", 1);
    SetBusVolume("Effect", 1);

    RegisterSL();

    listenerAttributes = new FMOD_3D_ATTRIBUTES();
    triggerAttributes = new FMOD_3D_ATTRIBUTES();

	EventManager::Subscribe(EventType::Loading_Start, [this]() {PlayEvent("event:/BGM/BGM1"); });
	EventManager::Subscribe(EventType::MainMenu_Start, [this]() {PlayEvent("event:/BGM/BGM1"); });
    EventManager::Subscribe(EventType::Game_Start, [this]() {
        PlayEvent("event:/BGM/BGM2"); 
        PauseEvent("event:/BGM/BGM1");
        });
    EventManager::Subscribe(EventType::Game_End, [this]() {StopEvent("event:/BGM/BGM2"); });
    EventManager::Subscribe(EventType::Game_Pause, [this]() {PauseEvent("event:/BGM/BGM2"); });
    EventManager::Subscribe(EventType::Game_Resume, [this]() {ResumeEvent("event:/BGM/BGM2"); });
    

    //EventManager::Subscribe(EventType::Game_Start, [this](int& a) {a = 2; });

    return true;
}

void AudioSystem::Update()
{
    for (auto& pair : activeEvents) {
        auto& instances = pair.second;

        instances.erase(std::remove_if(instances.begin(), instances.end(),
            [&](FMOD::Studio::EventInstance* instance) {
                FMOD_STUDIO_PLAYBACK_STATE state;
                instance->getPlaybackState(&state);

                if (state == FMOD_STUDIO_PLAYBACK_STOPPED) {
                    instance->stop(FMOD_STUDIO_STOP_IMMEDIATE);
                    eventPool[pair.first].push_back(instance);
                    return true;
                }
                return false;
            }),
            instances.end());
    }

    //std::cout << activeEvents["event:/Effect/GunShoot"].size() << ", " << eventPool["event:/Effect/GunShoot"].size() << std::endl;

    if (GameManager::GetInstance().GetGameState() != PaintballGameState::LOADING) {
        Matrix4 view = GameManager::GetInstance().GetMainCamera().BuildViewMatrix();;
        Vector3 forward = Vector::Normalise(-Vector3(view.array[0][2], view.array[1][2], view.array[2][2]));
        Vector3 up = Vector::Normalise(-Vector3(view.array[0][1], view.array[1][1], view.array[2][1]));
        Vector3 pos = GameManager::GetInstance().GetMainCamera().GetPosition();
        listenerAttributes->position = { pos.x, pos.y, pos.z };
        listenerAttributes->forward = { forward.x,forward.y,forward.z };
        listenerAttributes->up = { up.x,up.y,up.z };
        AudioSystem::GetInstance().studioSystem->setListenerAttributes(0, listenerAttributes);
    }

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

    delete listenerAttributes;
}


bool AudioSystem::LoadBank(const std::string& bankFile)
{
    Bank* bank = nullptr;
    if (studioSystem->loadBankFile((bankPath + bankFile).c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank) != FMOD_OK) {
        std::cerr << "¼ÓÔØBankÊ§°Ü£º" << bankFile << std::endl;
        return false;
    }
    banks[bankFile] = bank;
    return true;
}

bool AudioSystem::LoadBus(const std::string& busName) {
    Bus* bus = nullptr;

    FMOD_RESULT result = studioSystem->getBus(("bus:/" + busName).c_str(), &bus);

    if (result != FMOD_OK || bus == nullptr) {
        std::cerr << "Failed to load bus: " << busName << std::endl;
        return false;
    }

    buses[busName] = bus;
    return true;
}

void AudioSystem::SetBusVolume(const std::string& busName, float v) {
    if (v < 0 || v > 1)
    {
        std::cerr << "[Audio] Invalid volume value£º" << busName << std::endl;
        return;
    }

    auto it = buses.find(busName);
    if (it != buses.end()) {
        buses[busName]->getVolume(&volTemp);
        if (volTemp == v) return;

        buses[busName]->setVolume(v);
        //std::cout << "[Audio] Set bus(" << busName << ") volume to [" << v << "] successfully." << std::endl;
    }
    else
    {
        std::cerr << "[Audio] Cannot find the bus £º" << busName << std::endl;
    }
}

void AudioSystem::GetBusVolume(const std::string& busName, float& v)
{
    auto it = buses.find(busName);
    if (it != buses.end()) {
        buses[busName]->getVolume(&v);
    }
    else
    {
        std::cerr << "[Audio] Cannot find the bus £º" << busName << std::endl;
    }
}

bool AudioSystem::TriggerEvent(const std::string& eventName, rp3d::Vector3 pos) {
    auto descIt = eventDescriptions.find(eventName);
    if (descIt == eventDescriptions.end()) {
        Studio::EventDescription* eventDesc = nullptr;
        if (studioSystem->getEvent(eventName.c_str(), &eventDesc) != FMOD_OK || !eventDesc) {
            std::cerr << "[Audio] Cannot find the event: " << eventName << std::endl;
            return false;
        }
        eventDescriptions[eventName] = eventDesc;
    }

    Studio::EventInstance* eventInstance = nullptr;

    if (!eventPool[eventName].empty()) {
        eventInstance = eventPool[eventName].back();
        eventPool[eventName].pop_back();
    }
    else {
        if (eventDescriptions[eventName]->createInstance(&eventInstance) != FMOD_OK || !eventInstance) {
            std::cerr << "[Audio] Cannot create event instance: " << eventName << std::endl;
            return false;
        }
    }

    triggerAttributes->position = { pos.x, pos.y, pos.z };
    eventInstance->set3DAttributes(triggerAttributes);
    eventInstance->start();
    eventInstance->get3DAttributes(triggerAttributes);
    activeEvents[eventName].push_back(eventInstance);

    return true;
}

bool AudioSystem::TriggerEvent(const std::string& eventName) {
    auto it = events.find(eventName);
    if (it != events.end()) {
        it->second->start();
        return true;
    }

    auto descIt = eventDescriptions.find(eventName);
    if (descIt == eventDescriptions.end()) {
        EventDescription* eventDesc = nullptr;
        if (studioSystem->getEvent(eventName.c_str(), &eventDesc) != FMOD_OK || !eventDesc) {
            std::cerr << "[Audio] Cannot find the event £º" << eventName << std::endl;
            return false;
        }
        eventDescriptions[eventName] = eventDesc;
    }

    EventInstance* eventInstance = nullptr;
    if (eventDescriptions[eventName]->createInstance(&eventInstance) != FMOD_OK || !eventInstance) {
        std::cerr << "[Audio] Cannot create the event instance £º" << eventName << std::endl;
        return false;
    }

    eventInstance->start();

    events[eventName] = eventInstance;
    return true;
}

bool AudioSystem::PlayEvent(const std::string& eventName, bool isReplay) {
    auto it = events.find(eventName);
    if (it != events.end()) {

        bool state;
        it->second->getPaused(&state);
        if (state == true) {
            it->second->setPaused(false);
        }
        else
        {
            if (!isReplay) return true;
        }
        
        it->second->start();
        return true;
    }

    auto descIt = eventDescriptions.find(eventName);
    if (descIt == eventDescriptions.end()) {
        EventDescription* eventDesc = nullptr;
        if (studioSystem->getEvent(eventName.c_str(), &eventDesc) != FMOD_OK || !eventDesc) {
            std::cerr << "[Audio] Cannot find the event £º" << eventName << std::endl;
            return false;
        }
        eventDescriptions[eventName] = eventDesc;
    }

    EventInstance* eventInstance = nullptr;
    if (eventDescriptions[eventName]->createInstance(&eventInstance) != FMOD_OK || !eventInstance) {
        std::cerr << "[Audio] Cannot create the event instance £º" << eventName << std::endl;
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
        EventInstance* eventInstance = events[eventName];

        bool state;
        eventInstance->getPaused(&state);
        if (state == false) {
            eventInstance->setPaused(true);
        }
    }
}

void AudioSystem::ResumeEvent(const std::string& eventName) {
    if (events.count(eventName)) {
        EventInstance* eventInstance = events[eventName];

        bool state;
        eventInstance->getPaused(&state);
        if (state == true) {
            eventInstance->setPaused(false);
        }
    }
}

void AudioSystem::PauseEvent(EventInstance* event) {
    bool state;
    event->getPaused(&state);
    if (state == false) {
        event->setPaused(true);
    }
}

void AudioSystem::ResumeEvent(EventInstance* event) {
    bool state;
    event->getPaused(&state);
    if (state == true) {
        event->setPaused(false);
    }
}

EventInstance* AudioSystem::GetEvent(const std::string& eventName)
{

    auto descIt = eventDescriptions.find(eventName);
    if (descIt == eventDescriptions.end()) {
        EventDescription* eventDesc = nullptr;
        if (studioSystem->getEvent(eventName.c_str(), &eventDesc) != FMOD_OK || !eventDesc) {
            std::cerr << "[Audio] Cannot find the event £º" << eventName << std::endl;
            return nullptr;
        }
        eventDescriptions[eventName] = eventDesc;
    }

    EventInstance* eventInstance = nullptr;
    if (eventDescriptions[eventName]->createInstance(&eventInstance) != FMOD_OK || !eventInstance) {
        std::cerr << "[Audio] Cannot create the event instance £º" << eventName << std::endl;
        return nullptr;
    }
    return eventInstance;
}
