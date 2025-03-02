#include "InputManager.h"
#include "GameManager.h"

using namespace NCL::CSC8503;

void InputManager::Update() {
    // Check the current game state from GameManager.
    GameState currentState = GameManager::GetInstance().GetGameState();
    switch (currentState) {
    case GameState::MainMenu:
        HandleMainMenuInput();
        break;
    case GameState::InGame:
        HandleGameInput();
        break;
        // Add other states if need
    default:
        break;
    }
}

void InputManager::HandleMainMenuInput() {

    // This is purely conceptual:
    if (IsKeyPressed(KeyCodes::W)) {
       
    }
    if (IsKeyPressed(KeyCodes::S)) {
      
    }
    // Possibly an ENTER for selection...
    // if (IsKeyPressed(KeyCodes::ENTER)) { ... }
}

void InputManager::HandleGameInput() {
    // Example: Retrieve the main player from GameManager.
    PaintballPlayer* player = GameManager::GetInstance().GetPlayer();
    if (!player) return;

    float moveForce = 10.0f; // Example default force

    if (IsKeyPressed(KeyCodes::W)) {
        player->MoveForward(moveForce);
    }
    if (IsKeyPressed(KeyCodes::S)) {
        player->MoveBackward(moveForce);
    }
    if (IsKeyPressed(KeyCodes::A)) {
        player->MoveLeft(moveForce);
    }
    if (IsKeyPressed(KeyCodes::D)) {
        player->MoveRight(moveForce);
    }
    if (IsKeyPressed(KeyCodes::SPACE)) {
        player->Jump(moveForce);
    }
    if (IsKeyPressed(KeyCodes::SHIFT)) {
        player->GoDown(moveForce);
    }
}

bool InputManager::IsKeyPressed(KeyCodes::Type key) {
    // Check if key is currently held down
    return Window::GetKeyboard()->KeyDown(key);
}
