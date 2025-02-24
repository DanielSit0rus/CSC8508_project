#include "PaintballBullet.h"

using namespace NCL::CSC8503;

NCL::CSC8503::PaintballBullet::PaintballBullet(const std::string& objectName)
{
	name = objectName;

	speed = 100.0f;
	lifeTime = 2.0f;
}

NCL::CSC8503::PaintballBullet::~PaintballBullet()
{
}

Vector4 NCL::CSC8503::PaintballBullet::GetColor() const
{
	return color;
}

void NCL::CSC8503::PaintballBullet::SetColor(const Vector4& newColor)
{
	color = newColor;
}

void NCL::CSC8503::PaintballBullet::InitBullet(const rp3d::Quaternion& direction, bool isEnemy)
{
   
}

void NCL::CSC8503::PaintballBullet::Destroy()
{
}
