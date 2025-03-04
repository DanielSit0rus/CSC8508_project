#include "InputManager.h"
#include "GameManager.h"
#include "Window.h"
#include <iostream>

using namespace NCL::CSC8503;

void InputManager::Update() {
    GameState currentState = GameManager::GetInstance().GetGameState();
    switch (currentState) {
    case GameState::MainMenu:
        HandleMainMenuInput();
        break;
    case GameState::InGame:
        HandleGameInput();
        break;
    default:
        break;
    }
}

void InputManager::HandleMainMenuInput() {
    // Placeholder for menu input handling
}

void InputManager::HandleGameInput() {
    PaintballPlayer* player = GameManager::GetInstance().GetPlayer();
    if (!player) {
        std::cout << "[HandleGameInput] No player to control!" << std::endl;
        return;
    }

    PaintballPhysicsObject* physicsObject = player->GetPhysicsObject();
    if (!physicsObject) {
        std::cout << "[HandleGameInput] Player has no physics object!" << std::endl;
        return;
    }

    PerspectiveCamera* camera = player->GetCamera();
    if (!camera) {
        std::cout << "[HandleGameInput] Player has no camera!" << std::endl;
        return;
    }

    float moveForce = 10.0f; // Example default force

    if (IsKeyPressed(KeyCodes::UP)) {
        MoveForward(physicsObject, camera, moveForce);
    }
    if (IsKeyPressed(KeyCodes::DOWN)) {
        MoveBackward(physicsObject, camera, moveForce);
    }
    if (IsKeyPressed(KeyCodes::LEFT)) {
        MoveLeft(physicsObject, camera, moveForce);
    }
    if (IsKeyPressed(KeyCodes::RIGHT)) {
        MoveRight(physicsObject, camera, moveForce);
    }
    if (IsKeyPressed(KeyCodes::SPACE)) {
        Jump(physicsObject, moveForce);
    }
    if (IsKeyPressed(KeyCodes::SHIFT)) {
        GoDown(physicsObject, moveForce);
    }
}

bool InputManager::IsKeyPressed(KeyCodes::Type key) {
    return Window::GetKeyboard()->KeyDown(key);
}

// Movement functions (previously in CharacterController) now inside InputManager

void InputManager::MoveForward(PaintballPhysicsObject* physicsObject, PerspectiveCamera* camera, float forceMagnitude) {
    Vector3 fwd = CalculateForward(camera);
    physicsObject->AddForce(Util::NCLToRP3d(fwd * forceMagnitude));
}

void InputManager::MoveBackward(PaintballPhysicsObject* physicsObject, PerspectiveCamera* camera, float forceMagnitude) {
    Vector3 fwd = CalculateForward(camera);
    physicsObject->AddForce(Util::NCLToRP3d(-fwd * forceMagnitude));
}

void InputManager::MoveRight(PaintballPhysicsObject* physicsObject, PerspectiveCamera* camera, float forceMagnitude) {
    Vector3 right = CalculateRight(camera);
    physicsObject->AddForce(Util::NCLToRP3d(right * forceMagnitude));
}

void InputManager::MoveLeft(PaintballPhysicsObject* physicsObject, PerspectiveCamera* camera, float forceMagnitude) {
    Vector3 right = CalculateRight(camera);
    physicsObject->AddForce(Util::NCLToRP3d(-right * forceMagnitude));
}

void InputManager::Jump(PaintballPhysicsObject* physicsObject, float forceMagnitude) {
    if (physicsObject->isStand()) {
        physicsObject->ApplyLinearImpulse(rp3d::Vector3(0, forceMagnitude * 0.025f, 0));
    }
}

void InputManager::GoDown(PaintballPhysicsObject* physicsObject, float forceMagnitude) {
    physicsObject->AddForce(rp3d::Vector3(0, -forceMagnitude, 0));
}

// Calculate movement direction based on camera

Vector3 InputManager::CalculateForward(PerspectiveCamera* camera) {
    const Matrix4& view = camera->BuildViewMatrix();
    const Matrix4& camWorld = Matrix::Inverse(view);
    Vector3 right = Vector3(camWorld.GetColumn(0));
    Vector3 forward = Vector::Cross(Vector3(0, 1, 0), right);
    forward.y = 0; // Ensure the forward vector is horizontal
    forward = Vector::Normalise(forward);
    return forward;
}

Vector3 InputManager::CalculateRight(PerspectiveCamera* camera) {
    const Matrix4& view = camera->BuildViewMatrix();
    const Matrix4& camWorld = Matrix::Inverse(view);
    return Vector3(camWorld.GetColumn(0)); // Right axis from camera world matrix
}
