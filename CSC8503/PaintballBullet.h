#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"


namespace NCL::CSC8503 {
	class PaintballBullet :public PaintballGameObject
	{
	public:
		PaintballBullet();
		~PaintballBullet();
		void GetColor();
		void SetColor();
		void InitBullet(rp3d::Quaternion direction,bool Isenemy);
		void Distoty();
	private:
		Vector4 color;
		bool isenemy;
		float speed = 100;
		float livetime = 2;
	};
}
