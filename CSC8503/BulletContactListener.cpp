#include "BulletContactListener.h"
#include "GameManager.h"

using namespace NCL::CSC8503;

void BulletContactListener::onContact(const rp3d::CollisionCallback::CallbackData& callbackData)
{
	for (uint32_t i = 0; i < callbackData.getNbContactPairs(); i++) {
		const rp3d::CollisionCallback::ContactPair& contactPair = callbackData.getContactPair(i);
		rp3d::RigidBody* body1 = static_cast<rp3d::RigidBody*>(contactPair.getBody1());
		rp3d::RigidBody* body2 = static_cast<rp3d::RigidBody*>(contactPair.getBody2());
		if (!body1->getUserData() || !body2->getUserData()) continue;


		/*
		if (contactPair.getEventType() == rp3d::CollisionCallback::ContactPair::EventType::ContactStart) {
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
		*/
	}
}

void BulletContactListener::onTrigger(const rp3d::OverlapCallback::CallbackData& callbackData)
{
	for (uint32_t i = 0; i < callbackData.getNbOverlappingPairs(); i++) {
		const rp3d::OverlapCallback::OverlapPair& overlapPair = callbackData.getOverlappingPair(i);

		rp3d::RigidBody* body1 = static_cast<rp3d::RigidBody*>(overlapPair.getBody1());
		rp3d::RigidBody* body2 = static_cast<rp3d::RigidBody*>(overlapPair.getBody2());
		if (!body1->getUserData() || !body2->getUserData()) continue;

		// 识别子弹和物体
		if (((PaintballGameObject*)body1->getUserData())->GetType() == GameObjectType::bullet) {
			//std::cout << "子弹击中目标！" << std::endl;
			((PaintballBullet*)body1->getUserData())->Destroy();
			((PaintballGameObject*)body2->getUserData())->GetRenderObject()->SetColour(
				((PaintballGameObject*)body1->getUserData())->GetRenderObject()->GetColour()
			);

			//((PaintballGameObject*)body2->getUserData())->SetActive(false);
		}
		if (((PaintballGameObject*)body2->getUserData())->GetType() == GameObjectType::bullet) {
			//std::cout << "子弹击中目标！" << std::endl;
			((PaintballBullet*)body2->getUserData())->Destroy();
			((PaintballGameObject*)body1->getUserData())->GetRenderObject()->SetColour(
				((PaintballGameObject*)body2->getUserData())->GetRenderObject()->GetColour()
			);

			//((PaintballGameObject*)body1->getUserData())->SetActive(false);
		}

		if (((PaintballGameObject*)body1->getUserData())->GetType() == GameObjectType::bullet&& ((PaintballGameObject*)body2->getUserData())->GetType() == GameObjectType::cube) {
			std::cout << "子弹击中ffffffff！" << std::endl;
			((PaintballBullet*)body1->getUserData())->Destroy();
			((PaintballGameObject*)body2->getUserData())->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 100, 0));

			
		}


		
	}
}
