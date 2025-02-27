#include "Console.h"

using namespace NCL;
using namespace CSC8503;

void Console::Init(Window* win) {
    w = win;
    RegisterCommand("help", [this](const std::string&) { ShowHelp(); });
    RegisterCommand("save", [this](const std::string&) { EventManager::Trigger(EventType::Data_Save); });
    RegisterCommand("load", [this](const std::string&) { EventManager::Trigger(EventType::Data_Load); });
    RegisterCommand("net", [this](const std::string& args) { NetWorkConnect(args); });

    EventManager::Subscribe(EventType::Game_Start, [this]() {ShowConsole(false); });
    EventManager::Subscribe(EventType::Game_End, [this]() {ShowConsole(true); });
}

void Console::ShowConsole(bool t) {
    w->ShowConsole(t);
    w->ShowOSPointer(t);
    w->LockMouseToWindow(!t);
    isShow = t;
}

void Console::ShowConsole() {
    isShow =!isShow;
    w->ShowConsole(isShow);
    w->ShowOSPointer(isShow);
    w->LockMouseToWindow(!isShow);
}

void Console::RegisterCommand(const std::string& command, CommandHandler handler) {
    commands[command] = handler;
}

void Console::ProcessInput() {
    while (true)
    {
        std::cout << ">";
        std::getline(std::cin, input);
        HandleCommand(input);
        input.clear();
    }
}

void Console::HandleCommand(const std::string& input) {
    std::string command;
    std::string args;

    size_t spacePos = input.find(' ');
    if (spacePos != std::string::npos) {
        command = input.substr(0, spacePos);
        args = input.substr(spacePos + 1);
    }
    else {
        command = input;
    }
    auto it = commands.find(command);
    if (it != commands.end()) {
        it->second(args);
    }
    else {
        std::cout << "Unknown command: " << command << std::endl;
        std::cout << "Type help to list all commands." << std::endl;
    }
}




#pragma region CommandFunc
void Console::ShowHelp() const {
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << " - " << cmd.first << std::endl;
    }
}

void Console::NetWorkConnect(std::string args) const {
    if (args == "c" || args == "client") EventManager::Trigger(EventType::Network_StartAsClient);
    else if (args == "s" || args == "server") EventManager::Trigger(EventType::Network_StartAsServer);
    else if (args == "t" || args == "test") EventManager::Trigger(EventType::Network_Test);
    else
    {
        std::cout << "Unknown argument : "<< args <<"\nAvailable : client(c) , server(s), test(t)" << std::endl;
    }
}
#pragma endregion