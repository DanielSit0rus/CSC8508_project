#include "Console.h"

using namespace NCL;
using namespace CSC8503;

void Console::Init(Window* win) {
    w = win;
    RegisterCommand("help", [this](const std::string&) { ShowHelpCommnad(); });
    RegisterCommand("save", [this](const std::string&) { EventManager::Trigger(EventType::Data_Save); });
    RegisterCommand("load", [this](const std::string&) { EventManager::Trigger(EventType::Data_Load); });
    RegisterCommand("net", [this](const std::string& args) { NetworkCommand(args); });
    RegisterCommand("clear", [this](const std::string&) { ClearCommnad(); });
        
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
        std::cout << "Type help to list all commands.\n" << std::endl;
    }
}




#pragma region CommandFunc
void Console::ShowHelpCommnad() const {
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << " - " << cmd.first << std::endl;
    }
}

void Console::NetworkCommand(std::string s) const {
    std::istringstream iss(s);
    std::vector<std::string> args;
    std::string word;

    while (iss >> word) {
        args.push_back(word);
    }

    if (args.empty())
    {
        std::cout << "Unknown argument: " << s << "\nAvailable: client(c), server(s), test(t)" << std::endl;
        return;
    }

    if (args[0] == "c" || args[0] == "client") {
        EventManager::Trigger(EventType::Network_StartAsClient);
    }
    else if (args[0] == "s" || args[0] == "server") {
        EventManager::Trigger(EventType::Network_StartAsServer);
    }
    else if (args[0] == "t" || args[0] == "test") {
        if (args.size() == 1) {
            EventManager::Trigger(EventType::Network_Test);
        }
        else {
            std::ostringstream oss;
            for (size_t i = 1; i < args.size(); ++i) {
                if (i > 1) oss << " ";
                oss << args[i];
            }
            std::string joinedArgs = oss.str();
            EventManager::Trigger(EventType::Network_Test, joinedArgs);
        }
    }
    else {
        std::cout << "Unknown argument: " << s << "\nAvailable: client(c), server(s), test(t)" << std::endl;
    }
}
#pragma endregion