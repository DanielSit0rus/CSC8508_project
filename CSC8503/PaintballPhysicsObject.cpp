#include "PaintballPhysicsObject.h"
using namespace NCL;
using namespace CSC8503;

PaintballPhysicsObject::PaintballPhysicsObject(PaintballTransform* parentTransform, rp3d::RigidBody& rigidbody, rp3d::PhysicsWorld& world)
	:physicsWorld(world), rigidBody(rigidbody) {
	transform = parentTransform;
}

PaintballPhysicsObject::~PaintballPhysicsObject() {

}

void PaintballPhysicsObject::ApplyAngularImpulse(const Vector3& force) {
	std::cout << "\nPaintballPhysicsObject::ApplyAngularImpulse unfinished!\n";
	//angularVelocity += inverseInteriaTensor * force;
}

void PaintballPhysicsObject::ApplyLinearImpulse(const Vector3& force) {
	std::cout << "\nPaintballPhysicsObject::ApplyLinearImpulse unfinished!\n";
	//linearVelocity += force * inverseMass;
}

void PaintballPhysicsObject::AddForce(const Vector3& addedForce) {
	std::cout << "\nPaintballPhysicsObject::AddForce unfinished!\n";
	//force += addedForce;
}

void PaintballPhysicsObject::AddForceAtPosition(const Vector3& addedForce, const Vector3& position) {
	std::cout << "\nPaintballPhysicsObject::AddForceAtPosition unfinished!\n";
	//Vector3 localPos = position - transform->GetPosition();

	//force += addedForce;
	//torque += Vector::Cross(localPos, addedForce);
}

void PaintballPhysicsObject::AddTorque(const Vector3& addedTorque) {
	std::cout << "\nPaintballPhysicsObject::AddTorque unfinished!\n";
	//torque += addedTorque;
}

void PaintballPhysicsObject::ClearForces() {
	rigidBody.resetForce();
	rigidBody.resetTorque();
}