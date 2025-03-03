#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"
#include "PaintballBullet.h"
#include "GameManager.h"
#include "CharacterController.h"
#include "InputManager.h"


namespace NCL::CSC8503 {
	enum class WeaponType {
		RedGun,
		BlueGun,
		GreenGun
	};
	class PaintballPlayer :public PaintballGameObject
	{
	public:
		PaintballPlayer(const std::string& name = "");
		~PaintballPlayer();
		void InitController(PaintballPhysicsObject* physObj, PerspectiveCamera* cam);
		void GetColor();
		void SetColor();

		void SwitchWeapon(WeaponType newWeapon);
		void UpdateWeaponSelection();

		void InitializeController() {
			if (!physicsObject || !camera) {
				throw std::runtime_error("PhysicsObject and Camera must be set before initializing the controller.");
			}
			controller = new CharacterController(physicsObject, camera);
		}
		// Called by InputManager
		void MoveForward(float force);
		void MoveBackward(float force);
		void MoveLeft(float force);
		void MoveRight(float force);
		void Jump(float force);
		void GoDown(float force);


		void SetCamera(PerspectiveCamera* cam) {
			camera = cam;
		}

		PerspectiveCamera* GetCamera() const { return camera; }


		CharacterController* GetController() const {
			return controller;
		}

		void Move(float forceMagnitude);
		void Attack();
		void UpdatePlayerRotation();
		void Update() override;


		bool isControl = false;
	private:
		Vector4 color;
		PerspectiveCamera* camera;
		CharacterController* controller;
		WeaponType currentWeapon = WeaponType::RedGun; // Ä¬ÈÏÎäÆ÷
		
	};
}
