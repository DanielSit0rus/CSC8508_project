#include "InputManager.h"
#include "GameManager.h"
#include "Window.h"
#include <iostream>

using namespace NCL::CSC8503;

void InputManager::Update() {
    PaintballGameState currentState = GameManager::GetInstance().GetGameState();
    switch (currentState) {
    case PLAYING:
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

    if (!GameManager::GetInstance().isPhysEnabled() ||
        (GameManager::GetInstance().lockedObject != GameManager::GetInstance().GetPlayer()))return;

    PaintballPlayer* player;
    if (GameManager::GetInstance().GetNetwork())
        player = GameManager::GetInstance().GetNetworkPlayers()[-1];
    else
        player = GameManager::GetInstance().GetPlayer();

    if (!player) {
        if (isDebug) std::cout << " No player to control!" << std::endl;
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

    //float moveForce = 10.0f;

    rp3d::Vector3 moveDirection(GameManager::GetInstance().GetController()->GetAxis(2),
        GameManager::GetInstance().GetController()->GetAxis(0),
        GameManager::GetInstance().GetController()->GetAxis(1));

    player->Move(moveDirection, GameManager::GetInstance().forceMagnitude);
}
void InputManager::HandleGameInput(rp3d::Vector3 dir, int playerID, Vector3 camFront) {


    PaintballPlayer* player = GameManager::GetInstance().GetNetworkPlayers()[playerID];

    player->Move(dir, GameManager::GetInstance().forceMagnitude, camFront);
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
