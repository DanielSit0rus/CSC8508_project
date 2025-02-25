#pragma once

#include "EventManager.h"

#include <iostream>
#include <string>
#include <functional>
namespace NCL {
    namespace CSC8503 {
        class Console {
        public:
            using CommandHandler = std::function<void(const std::string&)>;

            static Console& GetInstance() {
                static Console instance;
                return instance;
            }
            Console(const Console&) = delete;
            Console& operator=(const Console&) = delete;

            void Init();
            void ProcessInput();
            void RegisterCommand(const std::string& command, CommandHandler handler);

        private:

            Console() = default;
            ~Console() = default;

            void HandleCommand(const std::string& input);


#pragma region CommandFunc
            void ShowHelp() const;
            void NetWorkConnect(std::string args) const;
#pragma endregion



            std::string input;
            std::unordered_map<std::string, CommandHandler> commands;
        };
    }
}