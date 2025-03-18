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

            void Move(PaintballPhysicsObject* physicsObject, Vector3 direction, float forceMagnitude);
            void Jump(PaintballPhysicsObject* physicsObject, float forceMagnitude);
            void GoDown(PaintballPhysicsObject* physicsObject, float forceMagnitude);
            void Getkeybord();
            Vector3 CalculateForward(PerspectiveCamera* camera);
            Vector3 CalculateRight(PerspectiveCamera* camera);
        };
    }
}

