#pragma once

#include <reactphysics3d/reactphysics3d.h>

using namespace reactphysics3d;


class PaintballGameObject
{

protected:
    reactphysics3d::PhysicsWorld& physicsWorld;  // Reference to the physics world for physics interactions
    reactphysics3d::RigidBody* rigidBody;        // Pointer to the rigid body for physics computations
    reactphysics3d::Collider* collider;

    reactphysics3d::Transform transform;

    reactphysics3d::Vector3 GetPosition() const;
    reactphysics3d::Quaternion GetOrientation() const;


public:
    PaintballGameObject(reactphysics3d::PhysicsWorld& physicsWorld); // add game world
    ~PaintballGameObject();

    void update();
    void setRenderObject(); // need rendereing logic
    void setPosition(const Vector3& position);

    RigidBody* GetRigidbody() const { return rigidBody; }
    void SetRigidbody(RigidBody* RP3DRigidbody) { rigidBody = RP3DRigidbody; }
    void AddRigidbody();

    Collider* GetCollider() const { return collider; };
    void SetCollider(CollisionShape* RP3DCollisionShape, Transform collisionTransform = Transform::identity());

    
   
    void SetPosition(const reactphysics3d::Vector3& newPos);
    //void SetPosition(const Vector3& newPos);
    void SetPosition(const float& x, const float& y, const float& z);

    void SetOrientation(const reactphysics3d::Vector3& newRotEulerAngles);
    void SetOrientation(const Quaternion& rotation);


 

};

