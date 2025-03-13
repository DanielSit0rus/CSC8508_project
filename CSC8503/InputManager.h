#pragma once
#include "Window.h";
#include "GameManager.h"
#include"PaintballGameWorld.h"

namespace NCL::CSC8503 {
    class InputManager {
    public:

        void Update();
        InputManager() : world(nullptr) {}
        InputManager(PaintballGameWorld* gameWorld) : world(gameWorld) {}

    private:
        void HandleMainMenuInput();
        void HandleGameInput();
        void  HandleLoadingInput();
        bool IsKeyPressed(KeyCodes::Type key);
        PaintballGameWorld* world;

        
    };
}

