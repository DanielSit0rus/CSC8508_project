#pragma once

#include "EventManager.h"

#include <iostream>
#include <string>
#include <functional>
#include <Window.h>

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

            void Init(Window* win);
            void ProcessInput();
            void RegisterCommand(const std::string& command, CommandHandler handler);
            void ShowConsole(bool t);
            void ShowConsole();

        private:

            Console() = default;
            ~Console() = default;

            void HandleCommand(const std::string& input);


#pragma region CommandFunc
            void ShowHelpCommnad() const;
            void NetworkCommand(std::string args) const;
            void ClearCommnad() const { system("cls"); }
            void AddObjCommand(std::string args) const;
#pragma endregion

            Window* w = nullptr;
            bool isShow = false;

            std::string input;
            std::unordered_map<std::string, CommandHandler> commands;
        };
    }
}