#include "InputManager.h"

using namespace NCL::CSC8503;

bool InputManager::IsKeyPressed(KeyCodes::Type key) {
    return Window::GetKeyboard()->KeyDown(key);
}
