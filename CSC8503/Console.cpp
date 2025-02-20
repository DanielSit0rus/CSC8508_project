#include "Console.h"

using namespace NCL;
using namespace CSC8503;

void Console::Init() {
    RegisterCommand("help", [this](const std::string&) { ShowHelp(); });
    RegisterCommand("save", [this](const std::string&) { EventManager::Trigger(EventType::Data_Save); });
    RegisterCommand("load", [this](const std::string&) { EventManager::Trigger(EventType::Data_Load); });
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
        std::cout << "Type help to list all commands. " << command << std::endl;
    }
}

#pragma region CommandFunc

void Console::ShowHelp() const {
    std::cout << "Available commands:" << std::endl;
    for (const auto& cmd : commands) {
        std::cout << " - " << cmd.first << std::endl;
    }
}

#pragma endregion
