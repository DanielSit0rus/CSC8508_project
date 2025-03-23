#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"
#include "PaintballBullet.h"
#include "GameManager.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"

namespace NCL::CSC8503 {
	enum class WeaponType {
		RedGun,
		BlueGun,
		GreenGun
	};
	enum class PlayerState {
		IdleState,
		RunState,
		JumpState,
		AttackState
	};

	class PaintballPlayer :public PaintballGameObject
	{
	public:
		PaintballPlayer(const std::string& name = "");
		~PaintballPlayer();
		void GetColor();
		void SetColor();

		void SwitchWeapon(WeaponType newWeapon);
		void UpdateWeaponSelection();

		void TakeDamage(int damage);
		int GetHealth() const { return health; }
		int GetMaxHealth() const { return maxHealth; }
		void UpdateHealthBar();
		void DrawHealthBar();

		void SwitchState(PlayerState newState);
		// Called by InputManager
		/*void MoveForward(float force);
		void MoveBackward(float force);
		void MoveLeft(float force);
		void MoveRight(float force);
		void Jump(float force);
		void GoDown(float force);*/


		void SetCamera(PerspectiveCamera* cam) {
			camera = cam;
		}

		PerspectiveCamera* GetCamera() const { return camera; }


		

		void Move(float forceMagnitude);
		void Attack();
		void Attack(Vector3 front, Vector4 color);
		void UpdatePlayerRotation();
		void Update(float dt) override;

		bool Ismove = false;

		bool isControl = false;

	private:
		Vector4 color;
		PerspectiveCamera* camera;
		WeaponType currentWeapon = WeaponType::RedGun; // ?      
		PlayerState  currentState;
		StateMachine* stateMachine;

		int health = 100;
		int maxHealth = 100;

		void SetupStateMachine();
	};
}
