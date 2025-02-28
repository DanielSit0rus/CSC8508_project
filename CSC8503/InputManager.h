#pragma once
#include "Window.h";
#include "GameManager.h"

namespace NCL::CSC8503 {
    class InputManager {
    public:

        //void Update() {
        //    GameState currentState = GameManager::GetInstance().GetGameState();
        //    switch (currentState) {
        //        case GameState::MainMenu:
        //            HandleMainMenuInput();
        //            break;
        //        case GameState::InGame:
        //            HandleGameInput();
        //            break;
        //        // Additional cases as needed
        //    }
        //}
        static bool IsKeyPressed(KeyCodes::Type key);

        /*void InputManager::HandleMainMenuInput() {
            if (IsKeyPressed(KeyCodes::W)) { NavigateMenuUp(); }
            if (IsKeyPressed(KeyCodes::S)) { NavigateMenuDown(); }
        }

        void InputManager::HandleGameInput() {
            PaintballPlayer* player = GameManager::GetInstance().GetPlayer();
            if (player == nullptr) return;

            if (IsKeyPressed(KeyCodes::W)) { player->GetController()->MoveForward(10.0f); }
            if (IsKeyPressed(KeyCodes::S)) { player->GetController()->MoveBackward(10.0f); }
            if (IsKeyPressed(KeyCodes::A)) { player->GetController()->MoveLeft(10.0f); }
            if (IsKeyPressed(KeyCodes::D)) { player->GetController()->MoveRight(10.0f); }
            if (IsKeyPressed(KeyCodes::SPACE)) { player->GetController()->Jump(10.0f); }
            if (IsKeyPressed(KeyCodes::SHIFT)) { player->GetController()->GoDown(10.0f); }
        }*/
        static bool IsKeyPressed(KeyCodes::Type key);
    };
}

