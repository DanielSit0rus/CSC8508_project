#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"
#include "PaintballBullet.h"
#include "GameManager.h"

namespace NCL::CSC8503 {
	class PaintballPlayer :public PaintballGameObject
	{
	public:
		PaintballPlayer(const std::string& name = "");
		~PaintballPlayer();
		void GetColor();
		void SetColor();

		void SetCamera(PerspectiveCamera* cam) {
			camera = cam;
		}

		void Move(float forceMagnitude);
		void Attack();
		void UpdatePlayerRotation();
		void Update() override;


		bool isControl = false;
	private:
		Vector4 color;
		PerspectiveCamera* camera;
		
	};
}
