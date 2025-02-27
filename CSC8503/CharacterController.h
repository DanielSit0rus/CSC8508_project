#pragma once
#include "PaintballPhysicsObject.h";
#include "Camera.h";


namespace NCL::CSC8503 {
    class CharacterController {
    public:
        CharacterController(PaintballPhysicsObject* physObj, PerspectiveCamera* cam);
        void MoveForward(float forceMagnitude);
        void MoveBackward(float forceMagnitude);
        void MoveRight(float forceMagnitude);
        void MoveLeft(float forceMagnitude);
        void Jump(float forceMagnitude);
        void GoDown(float forceMagnitude);
        Vector3 CalculateForward();
        Vector3 CalculateRight();


    private:
        PaintballPhysicsObject* physicsObject;
        PerspectiveCamera* camera;
    };
}
