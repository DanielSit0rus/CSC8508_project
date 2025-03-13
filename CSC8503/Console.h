#pragma once

#include "EventManager.h"
#include "GameManager.h"

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
            void Release();

            void ProcessInput();
            void RegisterCommand(const std::string& command, CommandHandler handler, const std::string& text = "");
            void ShowConsole(bool isShow);
            void ShowConsole();

            void ShowMouse(bool isShow);

        private:

            Console() = default;
            ~Console() = default;

            void HandleCommand(const std::string& input);

#pragma region CommandFunc
            void ShowHelpCommnad() const;
            void NetworkCommand(const std::string& args) const;
            void ClearCommnad() const { system("cls"); }
            void AddObjCommand(const std::string& args) const;
            void AudioCommand(const std::string& args) const;
            void TestCommand(const std::string& args);
#pragma endregion
            bool running = true;
            bool testing = false;

            Window* w = nullptr;
            bool isShow = false;

            struct CommandInfo {
                CommandHandler handler;
                std::string description;
            };
            std::unordered_map<std::string, CommandInfo> commands;

            std::thread* testThread = nullptr;
        };
    }
}