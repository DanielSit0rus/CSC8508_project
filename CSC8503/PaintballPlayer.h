#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"

namespace NCL::CSC8503 {
	class PaintballPlayer:public PaintballGameObject
	{
	public:
		PaintballPlayer(PerspectiveCamera* parentCamera);
		~PaintballPlayer();
		void GetColor();
		void SetColor();

		void Move(float forceMagnitude);
		void Attack();
	private:
		Vector4 color;
		PerspectiveCamera* camera;
	};
}
