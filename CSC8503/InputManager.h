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

            void HandleGameInput(rp3d::Vector3 dir, int playerID, Vector3 camFront);
        private:
            void HandleMainMenuInput();
            void HandleGameInput();

            Vector3 CalculateForward(PerspectiveCamera* camera);
            Vector3 CalculateRight(PerspectiveCamera* camera);
        };
    }
}

