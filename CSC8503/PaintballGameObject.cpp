#include "PaintballGameObject.h"

using namespace NCL::CSC8503;

PaintballGameObject::PaintballGameObject(const std::string& objectName) {
    name = objectName;
    worldID = -1;
    isActive = true;
    networkObject = nullptr;
    renderObject = nullptr;
    physicsObject = nullptr;
}

PaintballGameObject::~PaintballGameObject() {
    delete networkObject;
    delete renderObject;
    delete physicsObject;
}

void PaintballGameObject::Update() {
    transform.SetRpTransform(
        physicsObject->GetRigidbody().getTransform());

}
