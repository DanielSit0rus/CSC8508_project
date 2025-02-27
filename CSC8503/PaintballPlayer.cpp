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
	delete controller;
	controller = nullptr;
	camera = nullptr;
}

void PaintballPlayer::GetColor()
{
}
void PaintballPlayer::SetColor()
{
}

void PaintballPlayer::Move(float forceMagnitude)
{

	if (!controller) {
		controller = new CharacterController(physicsObject, camera); // Ensure controller is properly managed
	}

	if (InputManager::IsKeyPressed(KeyCodes::W)) {
		controller->MoveForward(forceMagnitude);
	}
	if (InputManager::IsKeyPressed(KeyCodes::S)) {
		controller->MoveBackward(forceMagnitude);
	}
	if (InputManager::IsKeyPressed(KeyCodes::A)) {
		controller->MoveLeft(forceMagnitude);
	}
	if (InputManager::IsKeyPressed(KeyCodes::D)) {
		controller->MoveRight(forceMagnitude);
	}
	if (InputManager::IsKeyPressed(KeyCodes::SPACE)) {
		controller->Jump(forceMagnitude);
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::SHIFT)) {
		controller->GoDown(forceMagnitude);
		}
	
}




void PaintballPlayer::Attack()
{
	//生成一个子弹，并发射
	GameManager::GetInstance().AddBullet(false, GetTransform().GetPosition()+rp3d::Vector3(0,5,0), rp3d::Vector3(1, 1, 1), GetTransform().GetOrientation());
}

void NCL::CSC8503::PaintballPlayer::UpdatePlayerRotation()
{
	if (!camera) return; // 确保 camera 存在

	float camYaw = camera->GetYaw();  // 获取摄像机的 Yaw 角度
	rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0.0f, camYaw, 0.0f); // 只修改 Y 轴旋转
	this->GetTransform().SetOrientation(newRotation);
}

void NCL::CSC8503::PaintballPlayer::Update()
{
	transform.SetRpTransform(
		physicsObject->GetRigidbody().getTransform());
	if (isControl)
	{
		UpdatePlayerRotation();
		Move(10.0f); // 这里的 10.0f 只是示例
		
	}
}
