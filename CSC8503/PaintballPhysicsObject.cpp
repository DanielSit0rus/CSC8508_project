#include "PaintballPhysicsObject.h"
using namespace NCL;
using namespace CSC8503;

PaintballPhysicsObject::PaintballPhysicsObject(PaintballTransform* parentTransform, rp3d::RigidBody& rigidbody, rp3d::PhysicsWorld& world)
	:physicsWorld(world), rigidBody(rigidbody) {
	transform = parentTransform;
}

PaintballPhysicsObject::~PaintballPhysicsObject() {
	physicsWorld.destroyRigidBody(&rigidBody);
}

void PaintballPhysicsObject::ApplyAngularImpulse(const rp3d::Vector3& force) {
	rigidBody.setAngularVelocity(
		rigidBody.getAngularVelocity() + Util::NCLToRP3d(GetInertiaTensor() * Util::RP3dToNCL(force))
	);
}

void PaintballPhysicsObject::ApplyLinearImpulse(const rp3d::Vector3& force) {
	rigidBody.setLinearVelocity(rigidBody.getLinearVelocity() + force / rigidBody.getMass());
}

void PaintballPhysicsObject::AddForce(const rp3d::Vector3& addedForce) {
	rigidBody.applyWorldForceAtCenterOfMass(addedForce);
}

void PaintballPhysicsObject::AddForceAtPosition(const rp3d::Vector3& addedForce, const rp3d::Vector3& position) {
	rigidBody.applyWorldForceAtWorldPosition(addedForce, position);
	//torque += Vector::Cross(localPos, addedForce);
}

void PaintballPhysicsObject::AddTorque(const rp3d::Vector3& addedTorque) {
	rigidBody.applyWorldTorque(addedTorque);
}

void PaintballPhysicsObject::ClearForces() {
	rigidBody.resetForce();
	rigidBody.resetTorque();
}\
Matrix3 PaintballPhysicsObject::GetInertiaTensor() const {
	rp3d::Vector3 localInertia = rigidBody.getLocalInertiaTensor();

	//Matrix3 inverseInteriaTensor = Matrix3::MatrixTemplate();
	Matrix3 inverseInteriaTensor = Matrix3();  // Calls default constructor

	inverseInteriaTensor.array[0][0] = 1.0f / localInertia.x;
	inverseInteriaTensor.array[1][1] = 1.0f / localInertia.y;
	inverseInteriaTensor.array[2][2] = 1.0f / localInertia.z;

	return inverseInteriaTensor;
}