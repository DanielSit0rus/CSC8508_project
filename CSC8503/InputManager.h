#pragma once
#include "GameManager.h"
#include "Vector.h"
#include "PaintballPhysicsObject.h"
#include "Camera.h"

namespace NCL {
    namespace CSC8503 {
        class InputManager {
        public:
            void Update();

        private:
            void HandleMainMenuInput();
            void HandleGameInput();
            bool IsKeyPressed(KeyCodes::Type key);

            // Movement logic from CharacterController (now inside InputManager)
            void MoveForward(PaintballPhysicsObject* physicsObject, PerspectiveCamera* camera, float forceMagnitude);
            void MoveBackward(PaintballPhysicsObject* physicsObject, PerspectiveCamera* camera, float forceMagnitude);
            void MoveRight(PaintballPhysicsObject* physicsObject, PerspectiveCamera* camera, float forceMagnitude);
            void MoveLeft(PaintballPhysicsObject* physicsObject, PerspectiveCamera* camera, float forceMagnitude);
            void Jump(PaintballPhysicsObject* physicsObject, float forceMagnitude);
            void GoDown(PaintballPhysicsObject* physicsObject, float forceMagnitude);

            Vector3 CalculateForward(PerspectiveCamera* camera);
            Vector3 CalculateRight(PerspectiveCamera* camera);
        };
    }
}
