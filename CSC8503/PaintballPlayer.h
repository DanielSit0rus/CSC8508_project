#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"
#include "PaintballBullet.h"
#include "GameManager.h"
#include "CharacterController.h"
#include "InputManager.h"


namespace NCL::CSC8503 {
	class PaintballPlayer :public PaintballGameObject
	{
	public:
		PaintballPlayer(const std::string& name = "");
		~PaintballPlayer();
		void GetColor();
		void SetColor();

		void InitializeController() {
			if (!physicsObject || !camera) {
				throw std::runtime_error("PhysicsObject and Camera must be set before initializing the controller.");
			}
			controller = new CharacterController(physicsObject, camera);
		}

		void MoveForward(float forceMagnitude) {
			controller->MoveForward(forceMagnitude);
		}

		void MoveBackward(float forceMagnitude) {
			controller->MoveBackward(forceMagnitude);
		}

		void MoveLeft(float forceMagnitude) {
			controller->MoveLeft(forceMagnitude);
		}

		void MoveRight(float forceMagnitude) {
			controller->MoveRight(forceMagnitude);
		}

		void Jump(float forceMagnitude) {
			controller->Jump(forceMagnitude);
		}

		void GoDown(float forceMagnitude) {
			controller->GoDown(forceMagnitude);
		}

		void SetCamera(PerspectiveCamera* cam) {
			camera = cam;
		}

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
		
	};
}
