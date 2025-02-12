#pragma once

#include "PaintballTransform.h"

namespace NCL {
	class CollisionVolume;

	namespace CSC8503 {
		class Transform;

		class PaintballPhysicsObject {
		public:
			PaintballPhysicsObject(PaintballTransform* parentTransform, rp3d::RigidBody& rigidbody, rp3d::PhysicsWorld& world);
			~PaintballPhysicsObject();

			rp3d::RigidBody& GetRigidbody()
			{
				return rigidBody;
			}

			rp3d::Vector3 GetLinearVelocity() const {
				return rigidBody.getLinearVelocity();
			}

			rp3d::Vector3 GetAngularVelocity() const {
				return rigidBody.getAngularVelocity();
			}

			rp3d::Vector3 GetTorque() const {
				return rigidBody.getTorque();
			}

			rp3d::Vector3 GetForce() const {
				return rigidBody.getForce();
			}

			void SetMass(float mass)
			{
				if (mass == 0) {
					rigidBody.setType(rp3d::BodyType::STATIC);
					rigidBody.setMass(mass);
				}
				else {
					rigidBody.setType(rp3d::BodyType::DYNAMIC);
				}
			}

			float GetMass() const {
				return rigidBody.getMass();
			}

			void SetElasticity(float e = 0.8) {
				rigidBody.getCollider(0)->getMaterial().setBounciness(e);
			}
			void SetElasticity(int idx, float e = 0.8) {
				rigidBody.getCollider(idx)->getMaterial().setBounciness(e);
			}

			float GetElasticity() const {
				return rigidBody.getCollider(0)->getMaterial().getBounciness();
			}
			float GetElasticity(int idx) const {
				return rigidBody.getCollider(idx)->getMaterial().getBounciness();
			}

			void SetFriction(float f) {
				rigidBody.getCollider(0)->getMaterial().setFrictionCoefficient(f);
			}
			void SetFriction(int idx, float f) {
				rigidBody.getCollider(idx)->getMaterial().setFrictionCoefficient(f);
			}

			float GetFriction() const {
				return rigidBody.getCollider(0)->getMaterial().getFrictionCoefficient();
			}
			float GetFriction(int idx) const {
				return rigidBody.getCollider(idx)->getMaterial().getFrictionCoefficient();
			}

			void ApplyAngularImpulse(const Vector3& force);
			void ApplyLinearImpulse(const Vector3& force);

			void AddForce(const Vector3& force);

			void AddForceAtPosition(const Vector3& force, const Vector3& position);

			void AddTorque(const Vector3& torque);


			void ClearForces();

			void SetLinearVelocity(const rp3d::Vector3& v) {
				rigidBody.setLinearVelocity(v);
			}

			void SetAngularVelocity(const rp3d::Vector3& v) {
				rigidBody.setAngularVelocity(v);
			}

		protected:
			PaintballTransform* transform;

			rp3d::RigidBody& rigidBody; // multiple colliders included

			rp3d::PhysicsWorld& physicsWorld;

		};
	}
}

