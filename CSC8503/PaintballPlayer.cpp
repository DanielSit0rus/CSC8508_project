#include "PaintballPlayer.h"
#include "PaintballBullet.h"

using namespace NCL::CSC8503;

NCL::CSC8503::PaintballPlayer::PaintballPlayer(PerspectiveCamera* parentCamera)
{
	camera = parentCamera;
	
}

NCL::CSC8503::PaintballPlayer::~PaintballPlayer()
{
	delete camera;
}

void PaintballPlayer::GetColor()
{
}
void PaintballPlayer::SetColor()
{
}

void PaintballPlayer::Move(float forceMagnitude)
{

	const Matrix4& view = camera->BuildViewMatrix();
	const Matrix4& camWorld = Matrix::Inverse(view);

	const Vector3& rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis = Vector::Normalise(fwdAxis);

	PaintballGameObject* target = this;

	const float& mass = target->GetPhysicsObject()->GetMass();
	float camYaw = camera->GetYaw();
	if (target->GetName() == "player" || target->GetName() == "kitten") camYaw += 180.0f;

	if (Window::GetKeyboard()->KeyDown(KeyCodes::W)) {
		target->GetPhysicsObject()->AddForce(Util::NCLToRP3d(fwdAxis * forceMagnitude));
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::S)) {
		target->GetPhysicsObject()->AddForce(Util::NCLToRP3d(-fwdAxis * forceMagnitude));
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::A)) {
		target->GetPhysicsObject()->AddForce(Util::NCLToRP3d(-rightAxis * forceMagnitude));
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::D)) {
		target->GetPhysicsObject()->AddForce(Util::NCLToRP3d(rightAxis * forceMagnitude));
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::SPACE) && target->GetPhysicsObject()->isStand()) {
		target->GetPhysicsObject()->ApplyLinearImpulse(rp3d::Vector3(0, forceMagnitude * 0.025f, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::SHIFT)) {
		target->GetPhysicsObject()->AddForce(rp3d::Vector3(0, -forceMagnitude, 0));
	}
}

void PaintballPlayer::Attack()
{
	//生成一个子弹，并发射
}
