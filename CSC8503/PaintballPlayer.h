#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"
#include "PaintballBullet.h"
#include "GameManager.h"
#include "Util.h"

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
		void GetColor();
		void SetColor();

		void SwitchWeapon(WeaponType newWeapon);
		void UpdateWeaponSelection();

	
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
		void UpdatePlayerRotation();
		void Update(float dt) override;


		bool isControl = false;
	private:
		Vector4 color;
		PerspectiveCamera* camera;
		WeaponType currentWeapon = WeaponType::RedGun; // ?      

	};
}
