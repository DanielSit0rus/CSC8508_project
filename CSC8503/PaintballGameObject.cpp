#include "PaintballGameObject.h"

using namespace NCL::CSC8503;

PaintballGameObject::PaintballGameObject(const GameObjectType& objectType, const std::string& objectName) {
	name = objectName;
	type = objectType;
	worldID = -1;
	isActive = true;
	networkObject = nullptr;
	renderObject = nullptr;
	physicsObject = nullptr;
	audioObject = nullptr;
}

PaintballGameObject::~PaintballGameObject() {
    delete networkObject;
    delete renderObject;
    delete physicsObject;
	delete audioObject;
}

void PaintballGameObject::Update(float dt) {
	// Sync physics transform to render transform
	if (physicsObject) {
		rp3d::RigidBody& body = physicsObject->GetRigidbody();
		rp3d::Transform physicsTransform = body.getTransform();
		transform.SetRpTransform(physicsTransform);
	}

	// Update animation if applicable
	if (renderObject && renderObject->GetAnimation()) {
		renderObject->UpdateAnimation(dt);
	}

	if (audioObject) audioObject->Update();
}
