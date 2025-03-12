#include "Console.h"
#include "GameManager.h"
#include "AudioSystem.h"
#include "GameObjectFreeList.h"

using namespace NCL;
using namespace CSC8503;

void Console::Init(Window* win) {
    w = win;
    RegisterCommand("help", [this](const std::string&) { ShowHelpCommnad(); }, "List all commands");
    RegisterCommand("clear", [this](const std::string&) { ClearCommnad(); }, "Clear console");

    RegisterCommand("save", [this](const std::string&) { EventManager::Trigger(EventType::Data_Save); }, "Save data");
    RegisterCommand("load", [this](const std::string&) { EventManager::Trigger(EventType::Data_Load); }, "Load data");
    RegisterCommand("add", [this](const std::string& args) { AddObjCommand(args); }, "Add an object");

    RegisterCommand("net", [this](const std::string& args) { NetworkCommand(args); }, "Network command");
    RegisterCommand("audio", [this](const std::string& args) { AudioCommand(args); }, "Audio command");

    RegisterCommand("test", [this](const std::string& args) { TestCommand(args); }, "Test command");

    EventManager::Subscribe(EventType::Game_Start, [this]() {ShowConsole(false); });
    EventManager::Subscribe(EventType::Game_End, [this]() {ShowConsole(true); });

    std::cout << "\nInput commnad (Type help to list all commands) :\n> ";
}

void Console::Release()
{
    running = false;
    testing = false;
    if(testThread) testThread->join();
    delete testThread;
}

void Console::ShowConsole(bool t) {
    w->ShowConsole(t);
    w->ShowOSPointer(t);
    w->LockMouseToWindow(!t);
    isShow = t;
}

void Console::ShowConsole() {
    isShow = !isShow;
    w->ShowConsole(isShow);
    w->ShowOSPointer(isShow);
    w->LockMouseToWindow(!isShow);
}


void Console::RegisterCommand(const std::string& command, CommandHandler handler, const std::string& text) {
    commands[command] = { handler, text };
}

void Console::ProcessInput() {
    std::string input;
    while (running)
    {
        std::getline(std::cin, input);
        HandleCommand(input);
        input.clear();
    }
}
void Console::HandleCommand(const std::string& input) {
    std::istringstream stream(input);
    std::string command;
    std::string args;

    if (!(stream >> command)) {
        std::cout << "Empty command." << command << std::endl;
        std::cout << "Type help to list all commands." << std::endl;
        std::cout << "\n> ";
        return;
    }
    std::getline(stream, args);

    auto it = commands.find(command);
    if (it != commands.end()) {
        it->second.handler(args);
    }
    else {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Type help to list all commands." << std::endl;
    }

    std::cout << (command == "clear" ? "> " : "\n> ");
}

#pragma region CommandFunc
void Console::ShowHelpCommnad() const {
    const int commandWidth = 8;
    std::cout << "\nAvailable commands:\n";
    for (const auto& cmd : commands) {
        std::cout << " - " << std::setw(commandWidth) << std::left << cmd.first;
        if (!cmd.second.description.empty()) {
            std::cout << ":  " << cmd.second.description << std::endl;
        }
    }
}

void Console::NetworkCommand(const std::string& s) const {
    std::istringstream stream(s);
    std::string arg;
    stream >> arg;

    if (arg == "c" || arg == "client") {
        std::string ip = "127 0 0 1";
        int a, b, c, d;
        stream >> a;
        stream >> b;
        stream >> c;
        stream >> d;

        if (0 <= a && a <= 255 && 0 <= b && b <= 255 && 0 <= c && c <= 255 && 0 <= d && d <= 255)
            ip = std::to_string(a) + " " + std::to_string(b) + " " + std::to_string(c) + " " + std::to_string(d);

        std::cout << "ip = " << ip << std::endl;
        EventManager::Trigger(EventType::Network_StartAsClient, ip);
    }
    else if (arg == "s" || arg == "server") {
        EventManager::Trigger(EventType::Network_StartAsServer);
    }
    else if (arg == "t" || arg == "test") {

        std::cout << G1.GetNetworkObjects().size() << std::endl;

        for (const auto& [key, value] : G1.GetNetworkObjects()) {
            if (value == nullptr) std::cout << "0";
            else std::cout << "1";
        }

        std::cout << std::endl;

        /*
        std::string text;
        std::getline(stream, text);
        if (!text.empty()) {
            EventManager::Trigger(EventType::Network_Test, text);
        }
        else {
            EventManager::Trigger(EventType::Network_Test);
        }
        */
    }
    else {
        std::cout << "Unknown argument: " << s << "\nAvailable: client(c), server(s), test(t)" << std::endl;
    }
}

void Console::AddObjCommand(const std::string& s) const { //add obj size num
    std::istringstream stream(s);
    std::string shape;
    float scaleValue = 1.0f;

    stream >> shape;

    std::string scale;
    if (stream >> scale) {
        std::istringstream scaleStream(scale);
        if (!(scaleStream >> scaleValue)) {
            scaleValue = 1.0f;
        }
    }
    int num = 1;
    stream >> num;
    if (num <= 0) num = 1;

    if (shape == "cube" || shape == "c") {
        for (int i = 0; i < num; i++)
            G1.AddObject(GameObjectType::cube,
                Util::NCLToRP3d(G1.GetMainCamera().GetPosition()),
                rp3d::Vector3(scaleValue, scaleValue, scaleValue),
                rp3d::Quaternion().identity()
            );
        std::cout << "A cube is added!" << std::endl;
    }
    else if (shape == "sphere" || shape == "s") {
        for (int i = 0; i < num; i++)
            G1.AddObject(GameObjectType::sphere,
                Util::NCLToRP3d(G1.GetMainCamera().GetPosition()),
                rp3d::Vector3(scaleValue, scaleValue, scaleValue),
                rp3d::Quaternion().identity()
            );
        std::cout << "A sphere is added!" << std::endl;
    }
    else {
        std::cout << "Unknown argument: " << s << "\nAvailable: cube(c), sphere(s)" << std::endl;
    }
}

void Console::AudioCommand(const std::string& s) const {
    std::istringstream stream(s);
    std::string target;

    stream >> target;

    if (target == "bus") {
        std::string name;
        float value;
        stream >> name;
        stream >> value;
        AudioSystem::GetInstance().SetBusVolume(name, value);
    }
    else {
        std::cout << "Unknown argument: " << s << std::endl;
    }
}

void Console::TestCommand(const std::string& s) {
    if (testing) {
        testing = false;
        testThread->join();
        delete testThread;
        testThread = nullptr;
    }

    std::istringstream stream(s);
    std::string target;

    stream >> target;

    if (target == "freeList" || target == "f") {
        testing = true;

        testThread = new std::thread([&]
            {
                while (testing) {
                    PerspectiveCamera camera = G1.GetMainCamera();
                    Vector3 camPos = camera.GetPosition();

                    G1.AddObject(GameObjectType::bullet,
                        Util::NCLToRP3d(camPos + G1.GetCameraFront() * 3.f), rp3d::Vector3(1, 1, 1),
                        rp3d::Quaternion().identity(),
                        Vector4(1, 1, 1, 1), "", "basic", "basic", 1, false, Util::NCLToRP3d(G1.GetCameraFront()));

                    std::this_thread::sleep_for(std::chrono::milliseconds(300));  // 限制输出频率，避免过快输出
                }
            }
        );
    }
    if (target == "t") {
        std::cout << G1.GetWorld()->GetObjectsNum() << std::endl;
    }
    if (target == "world"||target == "w") {
        std::cout << "rebuilding world..." << std::endl;
        G1.InitWorld(1);
    }
    else {
        std::cout << "Unknown argument: " << s << std::endl;
    }
}


#pragma endregion