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
		rp3d::RigidBody* body3 = static_cast<rp3d::RigidBody*>(overlapPair.getBody1());

		if (!body1->getUserData() || !body2->getUserData() || !body3->getUserData()) continue;

		PaintballGameObject* object1 = static_cast<PaintballGameObject*>(body1->getUserData());
		PaintballGameObject* object2 = static_cast<PaintballGameObject*>(body2->getUserData());
		PaintballGameObject* object3 = static_cast<PaintballGameObject*>(body3->getUserData());


		// Make sure both objects have valid data and check if one is a bullet and the other is a player
		if (object1->GetType() == GameObjectType::bullet && object2->GetType() == GameObjectType::player) {
			PaintballPlayer* player = static_cast<PaintballPlayer*>(object2);
			player->TakeDamage(10);  // Apply 10 damage to the player

			player->GetRenderObject()->SetColour(object1->GetRenderObject()->GetColour());
			// Destroy the bullet after impact
			((PaintballBullet*)object1)->Destroy();
		}
		else if (object2->GetType() == GameObjectType::bullet && object1->GetType() == GameObjectType::player) {
			PaintballPlayer* player = static_cast<PaintballPlayer*>(object1);
			player->TakeDamage(10);  // Apply 10 damage to the player

			player->GetRenderObject()->SetColour(object2->GetRenderObject()->GetColour());
			// Destroy the bullet after impact
			((PaintballBullet*)object2)->Destroy();
		}
		else if (object1->GetType() == GameObjectType::bullet && object2->GetType() == GameObjectType::enemy) {
			PaintballEnemy* enemy = static_cast<PaintballEnemy*>(object2);
			enemy->TakeDamage(25);  // Apply 10 damage to the enemy

			enemy->GetRenderObject()->SetColour(object1->GetRenderObject()->GetColour());
			// Destroy the bullet after impact
			((PaintballBullet*)object1)->Destroy();
		}
		else if (object2->GetType() == GameObjectType::bullet && object1->GetType() == GameObjectType::enemy) {
			PaintballEnemy* enemy = static_cast<PaintballEnemy*>(object1);
			enemy->TakeDamage(25);  // Apply 10 damage to the enemy

			enemy->GetRenderObject()->SetColour(object2->GetRenderObject()->GetColour());
			// Destroy the bullet after impact
			((PaintballBullet*)object2)->Destroy();

		}

			if (((PaintballGameObject*)body1->getUserData())->GetType() == GameObjectType::bullet && ((PaintballGameObject*)body2->getUserData())->GetType() == GameObjectType::trigger1) {
				std::cout << "子弹击中ffffffff！" << std::endl;
				((PaintballBullet*)body1->getUserData())->Destroy();
				((PaintballGameObject*)body2->getUserData())->TriggerAction();

			}

	}


}
