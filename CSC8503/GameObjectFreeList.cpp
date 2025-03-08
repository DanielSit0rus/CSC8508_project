#include "GameObjectFreeList.h"
#include "AudioSystem.h"
using namespace NCL::CSC8503;

PaintballBullet* GameObjectFreeList::GetBullet(rp3d::Vector3 ori3, bool isenemy,
	const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation,
	Vector4 color, float mass)
{
	PaintballBullet* bullet;

	if (objLists[GameObjectType::bullet].empty())
	{
		std::cout << "[GameObjectFreeList] Create a bullet" << std::endl;
		bullet = (PaintballBullet*)GameManager::GetInstance().AddBullet(ori3, isenemy, position, dimensions, orientation, color, mass);
	}
	else
	{
		std::cout << "[GameObjectFreeList] Pop a bullet" << std::endl;
		bullet = (PaintballBullet*) objLists[GameObjectType::bullet].top();

		bullet->GetPhysicsObject()->GetRigidbody().setTransform(
			rp3d::Transform(position, orientation));

		bullet->GetRenderObject()->SetColour(color);

		//Volume

		bullet->SetActive(true);
		bullet->InitBullet(ori3, isenemy);

		objLists[GameObjectType::bullet].pop();
	}

	AudioSystem::GetInstance().TriggerEvent("event:/Effect/GunShoot", position);
	return bullet;
}

void GameObjectFreeList::PushObject(PaintballGameObject* obj)
{
	obj->SetActive(false);
	objLists[obj->GetType()].push(obj);
}

