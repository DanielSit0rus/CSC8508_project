#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"
#include "GameManager.h"

namespace NCL::CSC8503 {
	class PaintballBullet :public PaintballGameObject
	{
	public:
		PaintballBullet(const std::string& name = "");
		~PaintballBullet();

		void Update(float dt) override;

		Vector4 GetColor() const;
		void SetColor(const Vector4& newColor);

		void InitBullet(rp3d::Vector3 direction, bool isEnemy);
		void Destroy();
	private:
		Vector4 color;
		bool isEnemy;
		float speed;
		float lifeTime; // 以秒为单位
		std::chrono::high_resolution_clock::time_point startTime;
	};
}
