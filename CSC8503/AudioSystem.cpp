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
    LoadBank("Lin.bank");
    LoadBus("BGM");

    buses["BGM"]->setVolume(1.0f);

    RegisterSL();

    /*
    EventManager::Subscribe(EventType::Game_Start, [this]() {PlayEvent("event:/BGM/BGM1"); });
    EventManager::Subscribe(EventType::Game_End, [this]() {StopEvent("event:/BGM/BGM1"); });
    */
    EventManager::Subscribe(EventType::Game_Pause, [this]() {PauseEvent(eventInstance); });
    EventManager::Subscribe(EventType::Game_Resume, [this]() {ResumeEvent(eventInstance); });
    

    //EventManager::Subscribe(EventType::Game_Start, [this](int& a) {a = 2; });

    EventDescription* eventDescription = nullptr;
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

    // ����3D����
    sourceAttributes = new FMOD_3D_ATTRIBUTES();
    sourceAttributes->position = { 0.0f, 0.0f, 0.0f }; // ������Դλ��
    sourceAttributes->velocity = { 0.0f, 0.0f, 0.0f }; // ������Դ�ٶ�
    sourceAttributes->forward = { 0.0f, 0.0f, 1.0f };  // ������Դǰ������
    sourceAttributes->up = { 0.0f, 1.0f, 0.0f };       // ������Դ�Ϸ�����

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
    Bank* bank = nullptr;
    if (studioSystem->loadBankFile((bankPath + bankFile).c_str(), FMOD_STUDIO_LOAD_BANK_NORMAL, &bank) != FMOD_OK) {
        std::cerr << "����Bankʧ�ܣ�" << bankFile << std::endl;
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


bool AudioSystem::PlayEvent(const std::string& eventName) {
    auto it = events.find(eventName);
    if (it != events.end()) {
        it->second->start();
        return true;
    }

    EventDescription* eventDesc = nullptr;
    if (studioSystem->getEvent(eventName.c_str(), &eventDesc) != FMOD_OK || !eventDesc) {
        std::cerr << "��ȡ�¼�ʧ�ܣ�" << eventName << std::endl;
        return false;
    }

    EventInstance* eventInstance = nullptr;
    if (eventDesc->createInstance(&eventInstance) != FMOD_OK || !eventInstance) {
        std::cerr << "�����¼�ʵ��ʧ�ܣ�" << eventName << std::endl;
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
    eventInstance->getPaused(&state);
    if (state == false) {
        eventInstance->setPaused(true);
    }
}

void AudioSystem::ResumeEvent(EventInstance* event) {
    bool state;
    eventInstance->getPaused(&state);
    if (state == true) {
        eventInstance->setPaused(false);
    }
}