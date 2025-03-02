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

void PaintballPlayer::InitController(PaintballPhysicsObject* physObj, PerspectiveCamera* cam) {
	controller = new CharacterController(physObj, cam);
}

void PaintballPlayer::GetColor()
{
}
void PaintballPlayer::SetColor()
{
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


void PaintballPlayer::MoveForward(float force) {
	if (controller) controller->MoveForward(force);
}
void PaintballPlayer::MoveBackward(float force) {
	if (controller) controller->MoveBackward(force);
}
void PaintballPlayer::MoveLeft(float force) {
	if (controller) controller->MoveLeft(force);
}
void PaintballPlayer::MoveRight(float force) {
	if (controller) controller->MoveRight(force);
}
void PaintballPlayer::Jump(float force) {
	if (controller) controller->Jump(force);
}
void PaintballPlayer::GoDown(float force) {
	if (controller) controller->GoDown(force);
}



void PaintballPlayer::Attack()
{
	//����һ���ӵ���������
	GameManager::GetInstance().AddBullet(false, GetTransform().GetPosition()+rp3d::Vector3(0,5,0), rp3d::Vector3(1, 1, 1), GetTransform().GetOrientation());
}

void NCL::CSC8503::PaintballPlayer::UpdatePlayerRotation()
{
	if (!camera) return; // ȷ�� camera ����

	float camYaw = camera->GetYaw();  // ��ȡ������� Yaw �Ƕ�
	rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0.0f, camYaw, 0.0f); // ֻ�޸� Y ����ת
	this->GetTransform().SetOrientation(newRotation);
}

void NCL::CSC8503::PaintballPlayer::Update()
{
	transform.SetRpTransform(
		physicsObject->GetRigidbody().getTransform());
	if (isControl)
	{
		UpdatePlayerRotation();
		//Move(10.0f); // ����� 10.0f ֻ��ʾ��
		
	}
}
