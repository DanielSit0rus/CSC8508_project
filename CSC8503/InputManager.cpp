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
    //  for menu input handling
}

void InputManager::HandleGameInput() {
    bool isDebug = false;

    PaintballPlayer* player = GameManager::GetInstance().GetPlayer();
    if (!player) {
        if(isDebug) std::cout << " No player to control!" << std::endl;
        return;
    }

    PaintballPhysicsObject* physicsObject = player->GetPhysicsObject();
    if (!physicsObject) {
        if (isDebug) std::cout << " Player has no physics object!" << std::endl;
        return;
    }

    PerspectiveCamera* camera = player->GetCamera();
    if (!camera) {
        if (isDebug) std::cout << "Player has no camera!" << std::endl;
        return;
    }

    float moveForce = 10.0f; 

    Vector3 moveDirection(0, 0, 0);

    if (IsKeyPressed(KeyCodes::UP)) {
        moveDirection += CalculateForward(camera);
    }
    if (IsKeyPressed(KeyCodes::DOWN)) {
        moveDirection -= CalculateForward(camera);
    }
    if (IsKeyPressed(KeyCodes::LEFT)) {
        moveDirection -= CalculateRight(camera);
    }
    if (IsKeyPressed(KeyCodes::RIGHT)) {
        moveDirection += CalculateRight(camera);
    }

    if (Vector::LengthSquared(moveDirection) > 0) {
        moveDirection = Vector::Normalise(moveDirection);
        Move(physicsObject, moveDirection, moveForce);
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

void InputManager::Move(PaintballPhysicsObject* physicsObject, Vector3 direction, float forceMagnitude) {
    physicsObject->AddForce(Util::NCLToRP3d(direction * forceMagnitude));
}

void InputManager::Jump(PaintballPhysicsObject* physicsObject, float forceMagnitude) {
    if (physicsObject->isStand()) {
        physicsObject->ApplyLinearImpulse(rp3d::Vector3(0, forceMagnitude * 0.025f, 0));
    }
}

void InputManager::GoDown(PaintballPhysicsObject* physicsObject, float forceMagnitude) {
    physicsObject->AddForce(rp3d::Vector3(0, -forceMagnitude, 0));
}

Vector3 InputManager::CalculateForward(PerspectiveCamera* camera) {
    const Matrix4& view = camera->BuildViewMatrix();
    const Matrix4& camWorld = Matrix::Inverse(view);
    Vector3 right = Vector3(camWorld.GetColumn(0));
    Vector3 forward = Vector::Cross(Vector3(0, 1, 0), right);
    forward.y = 0; 
    return Vector::Normalise(forward);
}

Vector3 InputManager::CalculateRight(PerspectiveCamera* camera) {
    const Matrix4& view = camera->BuildViewMatrix();
    const Matrix4& camWorld = Matrix::Inverse(view);
    return Vector3(camWorld.GetColumn(0));
}
