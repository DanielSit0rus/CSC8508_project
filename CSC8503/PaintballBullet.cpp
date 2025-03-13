#include "PaintballBullet.h"
#include "GameObjectFreeList.h"

using namespace NCL::CSC8503;

PaintballBullet::PaintballBullet(const std::string& objectName)
	:PaintballGameObject(type = GameObjectType::bullet, name)
{
	name = objectName;
	speed = 300.0f;
	lifeTime = 0.3f;

	startTime = std::chrono::high_resolution_clock::now();
}

PaintballBullet::~PaintballBullet()
{
	//delete networkObject;
	//delete renderObject;
	//delete physicsObject;
}

void PaintballBullet::Update(float dt)
{
	PaintballGameObject::Update(dt);

	auto now = std::chrono::high_resolution_clock::now();
	float elapsed = std::chrono::duration<float>(now - startTime).count();
	if (elapsed >= lifeTime) Destroy();
}

Vector4 PaintballBullet::GetColor() const
{
	return color;
}

void PaintballBullet::SetColor(const Vector4& newColor)
{
	color = newColor;
}

void PaintballBullet::InitBullet(rp3d::Vector3 direction, bool arg)
{
	direction.normalize();
	rp3d::Vector3 velocity = direction * speed; // 计算速度向量
	physicsObject->GetRigidbody().setLinearVelocity(velocity); // 设置初速度
	isEnemy = arg;
	startTime = std::chrono::high_resolution_clock::now();
}

void PaintballBullet::Destroy()
{
	if (isActive) GameObjectFreeList::GetInstance().PushObject(this);
}
