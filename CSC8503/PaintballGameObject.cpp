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

void PaintballGameObject::Update(float dt) {
	// Sync physics transform to render transform
	if (physicsObject) {
		rp3d::RigidBody& body = physicsObject->GetRigidbody();
		rp3d::Transform physicsTransform = body.getTransform();

		// Convert physics transform to game transform
		if (!renderObject || !renderObject->GetAnimation())
		{
			transform.SetRpTransform(physicsTransform);
		}

	}

	// Update animation if applicable
	if (renderObject && renderObject->GetAnimation()) {
		renderObject->UpdateAnimation(dt);
	}

}
