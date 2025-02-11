#include "PaintballTransform.h"

using namespace NCL::CSC8503;

PaintballTransform::PaintballTransform() {
	scale = rp3d::Vector3(1, 1, 1);
}

PaintballTransform::~PaintballTransform() {

}

Matrix4 PaintballTransform::GetMatrix() {
	UpdateMatrix();
	return matrix;
}

void PaintballTransform::UpdateMatrix() {
	matrix =
		Matrix::Translation(Vector3(GetPosition().x, GetPosition().y, GetPosition().z)) *
		Quaternion::RotationMatrix<Matrix4>(Quaternion(
			GetOrientation().x, GetOrientation().y, GetOrientation().z, GetOrientation().w)) *
		Matrix::Scale(Vector3(scale.x, scale.y, scale.z));
}

PaintballTransform& PaintballTransform::SetPosition(const rp3d::Vector3& worldPos) {
	transform.setPosition(worldPos);
	UpdateMatrix();
	return *this;
}

PaintballTransform& PaintballTransform::SetScale(const rp3d::Vector3& worldScale) {
	scale = worldScale;
	UpdateMatrix();
	return *this;
}

PaintballTransform& PaintballTransform::SetOrientation(const rp3d::Quaternion& worldOrientation) {
	transform.setOrientation(worldOrientation);
	UpdateMatrix();
	return *this;
}