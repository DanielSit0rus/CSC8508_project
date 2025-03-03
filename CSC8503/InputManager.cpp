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
    PaintballPlayer* player = GameManager::GetInstance().GetPlayer();

    if (!player) {
       // std::cout << "[HandleGameInput] Warning: No player to control!" << std::endl;
        return;
    }

    CharacterController* controller = player->GetController();
    if (!controller) {
       // std::cout << "[HandleGameInput] Player found, but controller is NULL!" << std::endl;
        return;
    }
    else {
       // std::cout << "[HandleGameInput] Player found, controller is valid. Processing input..." << std::endl;
    }

    float moveForce = 10.0f; // Example default force

    if (IsKeyPressed(KeyCodes::UP)) {
        std::cout << "Key is pressed! (Code: UP)" << std::endl;
        player->MoveForward(moveForce);
    }
    if (IsKeyPressed(KeyCodes::DOWN)) {
        std::cout << "Key is pressed! (Code: DOWN)" << std::endl;
        player->MoveBackward(moveForce);
    }
    if (IsKeyPressed(KeyCodes::LEFT)) {
        std::cout << "Key is pressed! (Code: LEFT)" << std::endl;
        player->MoveLeft(moveForce);
    }
    if (IsKeyPressed(KeyCodes::RIGHT)) {
        std::cout << "Key is pressed! (Code: RIGHT)" << std::endl;
        player->MoveRight(moveForce);
    }
    if (IsKeyPressed(KeyCodes::SPACE)) {
        std::cout << "Key is pressed! (Code: SPACE)" << std::endl;
        player->Jump(moveForce);
    }
    if (IsKeyPressed(KeyCodes::SHIFT)) {
        std::cout << "Key is pressed! (Code: SHIFT)" << std::endl;
        player->GoDown(moveForce);
    }
}



bool InputManager::IsKeyPressed(KeyCodes::Type key) {
    // Check if key is currently held down
    return Window::GetKeyboard()->KeyDown(key);
}
