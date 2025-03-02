#pragma once
#include "Window.h";
#include "GameManager.h"

namespace NCL::CSC8503 {
    class InputManager {
    public:

        void Update();

    private:
        void HandleMainMenuInput();
        void HandleGameInput();
        bool IsKeyPressed(KeyCodes::Type key);

        
    };
}

