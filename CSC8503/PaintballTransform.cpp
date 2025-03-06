#include "PaintballTransform.h"

using namespace NCL::CSC8503;

PaintballTransform::PaintballTransform() {
	scale = rp3d::Vector3(1, 1, 1);
	ratioR = rp3d::Vector3(1, 1, 1);
	offsetR = rp3d::Vector3(0, 0, 0);
}

PaintballTransform::~PaintballTransform() {

}

Matrix4 PaintballTransform::GetMatrix() {
	UpdateMatrix();
	return matrix;
}

void PaintballTransform::UpdateMatrix() {
	matrix =
		Matrix::Translation(Util::RP3dToNCL(GetPosition() + offsetR)) *
		Quaternion::RotationMatrix<Matrix4>(Util::RP3dToNCL(GetOrientation())) *
		Matrix::Scale(Util::RP3dToNCL(ratioR));
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
PaintballTransform& PaintballTransform::SetRatioR(const rp3d::Vector3& r) {
	ratioR = r;
	UpdateMatrix();
	return *this;
}
PaintballTransform& PaintballTransform::SetOffsetR(const rp3d::Vector3& offset) {
	offsetR = offset;
	UpdateMatrix();
	return *this;
}
PaintballTransform& PaintballTransform::SetOrientation(const rp3d::Quaternion& worldOrientation) {
	transform.setOrientation(worldOrientation);
	UpdateMatrix();
	return *this;
}