#include "PaintballPlayer.h"

using namespace NCL::CSC8503;

NCL::CSC8503::PaintballPlayer::PaintballPlayer(const std::string& objectName)
{
	name = objectName;
	worldID = -1;
	isActive = true;
	networkObject = nullptr;
	renderObject = nullptr;
	physicsObject = nullptr;
	camera = nullptr;
}

NCL::CSC8503::PaintballPlayer::~PaintballPlayer()
{
	delete networkObject;
	delete renderObject;
	delete physicsObject;
	camera = nullptr;
}



void PaintballPlayer::GetColor()
{
}
void PaintballPlayer::SetColor()
{
}

void NCL::CSC8503::PaintballPlayer::SwitchWeapon(WeaponType newWeapon)
{
	currentWeapon = newWeapon;
	std::cout << "Switched to weapon: " << static_cast<int>(newWeapon) << std::endl;
}

void NCL::CSC8503::PaintballPlayer::UpdateWeaponSelection()
{

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM1)) {
		SwitchWeapon(WeaponType::RedGun);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM2)) {
		SwitchWeapon(WeaponType::BlueGun);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM3)) {
		SwitchWeapon(WeaponType::GreenGun);
	}
}

//void PaintballPlayer::Move(float forceMagnitude)
//{
//
//	if (!controller) {
//		controller = new CharacterController(physicsObject, camera); // Ensure controller is properly managed
//	}
//
//	if (InputManager::IsKeyPressed(KeyCodes::W)) {
//		controller->MoveForward(forceMagnitude);
//	}
//	if (InputManager::IsKeyPressed(KeyCodes::S)) {
//		controller->MoveBackward(forceMagnitude);
//	}
//	if (InputManager::IsKeyPressed(KeyCodes::A)) {
//		controller->MoveLeft(forceMagnitude);
//	}
//	if (InputManager::IsKeyPressed(KeyCodes::D)) {
//		controller->MoveRight(forceMagnitude);
//	}
//	if (InputManager::IsKeyPressed(KeyCodes::SPACE)) {
//		controller->Jump(forceMagnitude);
//	}
//	if (Window::GetKeyboard()->KeyDown(KeyCodes::SHIFT)) {
//		controller->GoDown(forceMagnitude);
//		}
//	
//}


//void PaintballPlayer::MoveForward(float force) {
//	if (controller) controller->MoveForward(force);
//}
//void PaintballPlayer::MoveBackward(float force) {
//	if (controller) controller->MoveBackward(force);
//}
//void PaintballPlayer::MoveLeft(float force) {
//	if (controller) controller->MoveLeft(force);
//}
//void PaintballPlayer::MoveRight(float force) {
//	if (controller) controller->MoveRight(force);
//}
//void PaintballPlayer::Jump(float force) {
//	if (controller) controller->Jump(force);
//}
//void PaintballPlayer::GoDown(float force) {
//	if (controller) controller->GoDown(force);
//}



void PaintballPlayer::Attack()
{
	Vector4 bulletColor;

	switch (currentWeapon) {
	case WeaponType::RedGun:
		bulletColor = Vector4(1, 0, 0, 1); // 红色
		std::cout << "red " << std::endl;
		break;
	case WeaponType::BlueGun:
		bulletColor = Vector4(0, 0, 1, 1); // 蓝色
		std::cout << "blue " << std::endl;

		break;
	case WeaponType::GreenGun:
		bulletColor = Vector4(0, 1, 0, 1); // 绿色
		std::cout << "green " << std::endl;

		break;
	}

	Vector3 camPos = camera->GetPosition();
	float yaw = DegreesToRadians(camera->GetYaw());
	float pitch = DegreesToRadians(-camera->GetPitch());
	Vector3 front(cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw));
	front = -Vector::Normalise(front);

	GameManager::GetInstance().AddBullet(Util::NCLToRP3d(front), false, GetTransform().GetPosition() + rp3d::Vector3(0, 4, 0), rp3d::Vector3(1, 1, 1), Util::NCLToRP3d(Quaternion(camera->BuildViewMatrix())), bulletColor);
}

void NCL::CSC8503::PaintballPlayer::UpdatePlayerRotation()
{
	if (!camera) return; // ȷ   camera     

	float camYaw = camera->GetYaw();  //   ȡ        Yaw  Ƕ 
	rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0.0f, camYaw, 0.0f); // ֻ ޸  Y     ת
	this->GetTransform().SetOrientation(newRotation);
}

void NCL::CSC8503::PaintballPlayer::Update(float dt)
{
	PaintballGameObject::Update(dt);

	if (isControl)
	{
		UpdatePlayerRotation();
		//Move(10.0f); //       10.0f ֻ  ʾ  
	}
	UpdateWeaponSelection(); // 添加武器切换

}
