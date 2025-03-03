#include "Console.h"
#include "GameManager.h"

using namespace NCL;
using namespace CSC8503;

void Console::Init(Window* win) {
    w = win;
    RegisterCommand("help", [this](const std::string&) { ShowHelpCommnad(); }, "List all commands");
    RegisterCommand("save", [this](const std::string&) { EventManager::Trigger(EventType::Data_Save); }, "Save data");
    RegisterCommand("load", [this](const std::string&) { EventManager::Trigger(EventType::Data_Load); }, "Load data");
    RegisterCommand("net", [this](const std::string& args) { NetworkCommand(args); }, "Network command");
    RegisterCommand("clear", [this](const std::string&) { ClearCommnad(); }, "Clear console");
    RegisterCommand("add", [this](const std::string& args) { AddObjCommand(args); }, "Add an object");

    EventManager::Subscribe(EventType::Game_Start, [this]() {ShowConsole(false); });
    EventManager::Subscribe(EventType::Game_End, [this]() {ShowConsole(true); });

    std::cout << "\nInput commnad (Type help to list all commands) :\n> ";
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
    while (true)
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

void Console::NetworkCommand(std::string s) const {
    std::istringstream stream(s);
    std::string arg;
    stream >> arg;

    if (arg == "c" || arg == "client") {
        EventManager::Trigger(EventType::Network_StartAsClient);
    }
    else if (arg == "s" || arg == "server") {
        EventManager::Trigger(EventType::Network_StartAsServer);
    }
    else if (arg == "t" || arg == "test") {
        std::string text;
        std::getline(stream, text);
        if (!text.empty()) {
            EventManager::Trigger(EventType::Network_Test, text);
        }
        else {
            EventManager::Trigger(EventType::Network_Test);
        }
    }
    else {
        std::cout << "Unknown argument: " << s << "\nAvailable: client(c), server(s), test(t)" << std::endl;
    }
}

void Console::AddObjCommand(std::string s) const {
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

    if (shape == "cube" || shape == "c") {
        GameManager::GetInstance().AddCube(
            Util::NCLToRP3d(GameManager::GetInstance().GetMainCamera().GetPosition()),
            rp3d::Vector3(scaleValue, scaleValue, scaleValue),
            rp3d::Quaternion().identity()
        );
        std::cout << "A cube is added!" << std::endl;
    }
    else if (shape == "sphere" || shape == "s") {
        GameManager::GetInstance().AddSphere(
            Util::NCLToRP3d(GameManager::GetInstance().GetMainCamera().GetPosition()),
            rp3d::Vector3(scaleValue, scaleValue, scaleValue),
            rp3d::Quaternion().identity()
        );
        std::cout << "A sphere is added!" << std::endl;
    }
    else {
        std::cout << "Unknown argument: " << s << "\nAvailable: cube(c), sphere(s)" << std::endl;
    }
}

#pragma endregion