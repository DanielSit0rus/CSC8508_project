#include "BulletContactListener.h"
#include "GameManager.h"

void NCL::CSC8503::BulletContactListener::onContact(const rp3d::CollisionCallback::CallbackData& callbackData)
{
	for (uint32_t i = 0; i < callbackData.getNbContactPairs(); i++) {
		const rp3d::CollisionCallback::ContactPair& contactPair = callbackData.getContactPair(i);
		rp3d::RigidBody* body1 = static_cast<rp3d::RigidBody*>(contactPair.getBody1());
		rp3d::RigidBody* body2 = static_cast<rp3d::RigidBody*>(contactPair.getBody2());
		if (!body1->getUserData() || !body2->getUserData()) continue;

		// 识别子弹和物体
		if (((PaintballGameObject*)body1->getUserData())->GetType() == GameObjectType::bullet) {
			//std::cout << "子弹击中目标！" << std::endl;
			GameManager::GetInstance().DeleteObject((PaintballBullet*)body1->getUserData());
		}
		if (((PaintballGameObject*)body2->getUserData())->GetType() == GameObjectType::bullet) {
			//std::cout << "子弹击中目标！" << std::endl;
			GameManager::GetInstance().DeleteObject((PaintballBullet*)body2->getUserData());
		}
	}
}
