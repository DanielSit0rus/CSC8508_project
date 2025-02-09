#include "PaintballGameObject.h"


PaintballGameObject::PaintballGameObject(PhysicsWorld& world)
    : physicsWorld(world){
    rigidBody = nullptr;
    collider = nullptr;
 
}

PaintballGameObject::~PaintballGameObject() {
    if (rigidBody) {
        if (collider) {
            rigidBody->removeCollider(collider); // Not clear if need to delete Collider separately, doing for safety
        }
        physicsWorld.destroyRigidBody(rigidBody);
    }
}

void PaintballGameObject::setRenderObject() {
    // Placeholder for setting up rendering logic
}

void PaintballGameObject::setPosition(const Vector3& position) {
    SetPosition(reactphysics3d::Vector3(position.x, position.y, position.z));
}

reactphysics3d::Vector3 PaintballGameObject::GetPosition() const {
    if (rigidBody) {
        return rigidBody->getTransform().getPosition();
    }
    return reactphysics3d::Vector3(0, 0, 0);
}

reactphysics3d::Quaternion PaintballGameObject::GetOrientation() const {
    if (rigidBody) {
        return rigidBody->getTransform().getOrientation();
    }
    return reactphysics3d::Quaternion::identity();
}

void PaintballGameObject::AddRigidbody() {
    Transform transform = reactphysics3d::Transform::identity(); // Default transform, replace as needed
    rigidBody = physicsWorld.createRigidBody(transform);
}

void PaintballGameObject::SetCollider(CollisionShape* RP3DCollisionShape, Transform collisionTransform) {
    if (!rigidBody) return;

    collider = rigidBody->addCollider(RP3DCollisionShape, collisionTransform);
}

void PaintballGameObject::SetPosition(const reactphysics3d::Vector3& newPos) {
    if (rigidBody) {
        reactphysics3d::Transform currentTransform = rigidBody->getTransform();
        currentTransform.setPosition(newPos);
        rigidBody->setTransform(currentTransform);
    }
}
void PaintballGameObject::SetPosition(const float& x, const float& y, const float& z) {
    SetPosition(reactphysics3d::Vector3(x, y, z));
}

void PaintballGameObject::SetOrientation(const reactphysics3d::Vector3& newRotEulerAngles) {
    reactphysics3d::Quaternion quat = reactphysics3d::Quaternion::fromEulerAngles(newRotEulerAngles.x, newRotEulerAngles.y, newRotEulerAngles.z);
    if (rigidBody) {
        reactphysics3d::Transform currentTransform = rigidBody->getTransform();
        currentTransform.setOrientation(quat);
        rigidBody->setTransform(currentTransform);
    }
}

void PaintballGameObject::SetOrientation(const Quaternion& rotation) {
    reactphysics3d::Quaternion quat(rotation.x, rotation.y, rotation.z, rotation.w);
    if (rigidBody) {
        reactphysics3d::Transform currentTransform = rigidBody->getTransform();
        currentTransform.setOrientation(quat);
        rigidBody->setTransform(currentTransform);
    }
}

