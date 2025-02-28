#pragma once
#include "PaintballGameObject.h"
#include "PaintballGameWorld.h"
#include "GameManager.h"

namespace NCL::CSC8503 {
	//class BulletContactListener : public rp3d::EventListener {
	//public:
	//	void onContact(const rp3d::CollisionCallback::CallbackData& callbackData) override {
	//		for (uint32_t i = 0; i < callbackData.getNbContactPairs(); i++) {
	//			const rp3d::CollisionCallback::ContactPair& contactPair = callbackData.getContactPair(i);
	//			rp3d::RigidBody* body1 = static_cast<rp3d::RigidBody*>(contactPair.getBody1());
	//			rp3d::RigidBody* body2 = static_cast<rp3d::RigidBody*>(contactPair.getBody2());

	//			std::string name1 = ((PaintballBullet*)body1->getUserData())->GetName(); 
	//			std::string name2 = ((PaintballGameObject*)body2->getUserData())->GetName();
	//			// 识别子弹和物体
	//			if ((name1 == "bullet")) {
	//				std::cout << "子弹击中目标！" << std::endl;
	//				GameManager::GetInstance().RemoveBullet((PaintballBullet*)body1->getUserData());
	//			}
	//			if ((name2 == "bullet")) {
	//				std::cout << "子弹击中目标！" << std::endl;
	//				GameManager::GetInstance().RemoveBullet((PaintballBullet*)body2->getUserData());
	//			}
	//		}
	//	}
	//};



	class PaintballBullet :public PaintballGameObject
	{
	public:
		PaintballBullet(const std::string& name = "");
		~PaintballBullet();

		Vector4 GetColor() const;
		void SetColor(const Vector4& newColor);

		void InitBullet(const rp3d::Quaternion& direction, bool isEnemy);
		void Destroy();
	private:
		Vector4 color;
		bool isEnemy;
		float speed = 100.0f;
		float lifeTime = 2.0f; // 以秒为单位
	};
}
