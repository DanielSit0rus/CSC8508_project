#include "PaintballPlayer.h"
#include "GameManager.h"
#include <iostream>
#include "GameObjectFreeList.h"

using namespace NCL::CSC8503;

NCL::CSC8503::PaintballPlayer::PaintballPlayer(const std::string& objectName)
	:PaintballGameObject(type = GameObjectType::player, name)
{
	currentState = PlayerState::IdleState;
	name = objectName;
	worldID = -1;
	isActive = true;
	networkObject = nullptr;
	renderObject = nullptr;
	physicsObject = nullptr;
	camera = nullptr;
	stateMachine = new StateMachine();
	SetupStateMachine();
}

void NCL::CSC8503::PaintballPlayer::SetupStateMachine() {

	State* idleState = new State([&](float dt) {
		this->GetRenderObject()->SetAnimation(ResourceManager::GetInstance().GetIdleanim());
		});

	State* runState = new State([&](float dt) {
		this->GetRenderObject()->SetAnimation(ResourceManager::GetInstance().GetMoveanim());
		});

	// Add states to the machine
	stateMachine->AddState(idleState);
	stateMachine->AddState(runState);

	// Transitions
	stateMachine->AddTransition(new StateTransition(idleState, runState, [&]() -> bool {
		return this->currentState == PlayerState::RunState;
		}));

	stateMachine->AddTransition(new StateTransition(runState, idleState, [&]() -> bool {
		return this->currentState == PlayerState::IdleState;
		}));

}



NCL::CSC8503::PaintballPlayer::~PaintballPlayer()
{
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
	//std::cout << "Switched to weapon: " << static_cast<int>(newWeapon) << std::endl;
}

void NCL::CSC8503::PaintballPlayer::UpdateWeaponSelection()
{
	if (GameManager::GetInstance().GetNetwork()) return;
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM1)) {
		SwitchWeapon(WeaponType::RedGun);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM2)) {
		SwitchWeapon(WeaponType::GreenGun);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM3)) {
		SwitchWeapon(WeaponType::BlueGun);
	}
}

void NCL::CSC8503::PaintballPlayer::SwitchState(PlayerState newState)
{
	currentState = newState;
}





void PaintballPlayer::Move(rp3d::Vector3 dir, float forceMagnitude)
{
	const Matrix4& view = camera->BuildViewMatrix();
	const Matrix4& camWorld = Matrix::Inverse(view);
	const Vector3& rightAxis = Vector3(camWorld.GetColumn(0));
	Vector3 fwdAxis = Vector::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis = Vector::Normalise(fwdAxis);

	physicsObject->AddForce(Util::NCLToRP3d(fwdAxis * dir[0] + rightAxis * dir[1]) * forceMagnitude);

	if (dir[2] > 0 && physicsObject->isStand()) physicsObject->ApplyLinearImpulse(rp3d::Vector3(0, forceMagnitude * 0.3f, 0));
	else if (dir[2] < 0) physicsObject->AddForce(rp3d::Vector3(0, -forceMagnitude, 0));

	if (dir[0] != 0 || dir[1] != 0) Ismove = true;
	else Ismove = false;
}

void PaintballPlayer::Move(rp3d::Vector3 dir, float forceMagnitude, Vector3 camFront)
{
	Vector3  fwdAxis = Vector::Normalise(camFront);
	Vector3 rightAxis = Vector::Cross(fwdAxis, Vector3(0, 1, 0));

	physicsObject->AddForce(Util::NCLToRP3d(fwdAxis * dir[0] + rightAxis * dir[1]) * forceMagnitude);

	if (dir[2] > 0) physicsObject->ApplyLinearImpulse(rp3d::Vector3(0, forceMagnitude * 0.03f, 0));
	else if (dir[2] < 0) physicsObject->AddForce(rp3d::Vector3(0, -forceMagnitude, 0));

	if (dir[0] != 0 || dir[1] != 0) Ismove = true;
	else Ismove = false;
}

void PaintballPlayer::Attack()
{
	Vector4 bulletColor;

	switch (currentWeapon) {
	case WeaponType::RedGun:
		bulletColor = Vector4(1, 0, 0, 1); // 红色
		//std::cout << "red " << std::endl;
		break;
	case WeaponType::BlueGun:
		bulletColor = Vector4(0, 0, 1, 1); // 蓝色
		//std::cout << "blue " << std::endl;
		break;
	case WeaponType::GreenGun:
		bulletColor = Vector4(0, 1, 0, 1); // 绿色
		//std::cout << "green " << std::endl;
		break;
	}


	GameManager::GetInstance().AddObject(GameObjectType::bullet,
		transform.GetPosition() + rp3d::Vector3(0, 4, 0), rp3d::Vector3(1, 1, 1), rp3d::Quaternion().identity(),
		bulletColor, "", "basic", "", "", "", "", "", "", "", "basic", 1, false, Util::NCLToRP3d(GameManager::GetInstance().GetCameraFront()));
}

void PaintballPlayer::Attack(Vector3 front, Vector4 color)
{
	GameManager::GetInstance().AddObject(GameObjectType::bullet,
		transform.GetPosition() + rp3d::Vector3(0, 4, 0), rp3d::Vector3(1, 1, 1), rp3d::Quaternion().identity(),
		color, "", "basic", "", "", "", "", "", "", "", "basic", 1, false, Util::NCLToRP3d(front));
}

void NCL::CSC8503::PaintballPlayer::UpdatePlayerRotation()
{
	if (!camera) return; // ȷ   camera     
	float yaw = DegreesToRadians(camera->GetYaw()+180.f); 
	rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0.0f, yaw, 0.0f); // ֻ ޸  Y     ת
	this->GetTransform().SetOrientation(newRotation);
	//std::cout << "yaw1: " << yaw << ",peer = "<<GameManager::GetInstance().GetThisPeer() << std::endl;
}

void NCL::CSC8503::PaintballPlayer::UpdatePlayerRotation(Vector3 camFront)
{ 
	float yaw = DegreesToRadians(RadiansToDegrees(atan2(-camFront.x, -camFront.z)) + 180.f);
	rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0.0f, yaw, 0.0f); // ֻ ޸  Y     ת
	this->GetTransform().SetOrientation(newRotation);
	//std::cout << "yaw2: " << yaw << std::endl;
}

void NCL::CSC8503::PaintballPlayer::TakeDamage(int damage) {
	health -= damage;
	

	if (health <= 0) {

		// Notify GameManager to remove player and handle game exit
		GameManager::GetInstance().ShowExitScreen();
	}
	UpdateHealthBar();
}




void NCL::CSC8503::PaintballPlayer::UpdateHealthBar() {
	DrawHealthBar();
}

void NCL::CSC8503::PaintballPlayer::DrawHealthBar() {
	float healthPercentage = (float)health / 100.0f;
	int barLength = 20;
	int healthBarFilled = (int)(healthPercentage * barLength);

	std::cout << "Health: [";
	for (int i = 0; i < healthBarFilled; ++i) {
		std::cout << "#";
	}
	for (int i = healthBarFilled; i < barLength; ++i) {
		std::cout << "-";
	}
	std::cout << "] " << healthPercentage * 100 << "%" << std::endl;
}


void NCL::CSC8503::PaintballPlayer::Update(float dt)
{
	PaintballGameObject::Update(dt);

	if (Ismove && currentState != PlayerState::RunState)
	{
		SwitchState(PlayerState::RunState);
	}
	else if (!Ismove && currentState != PlayerState::IdleState)
	{
		SwitchState(PlayerState::IdleState);
	}
	stateMachine->Update(dt);

	if (isControl && GameManager::GetInstance().isPhysEnabled())
	{
		UpdatePlayerRotation();
		//Move(10.0f); //      
	}
	UpdateWeaponSelection();

	if (audioObject && currentState == PlayerState::RunState) {
		float speed = physicsObject->GetRigidbody().getLinearVelocity().length();
		float clampedSpeed = Maths::Clamp(speed * 0.06f, 0.f, 1.f);
		audioObject->PlayEvent("event:/Effect/FootStep", clampedSpeed);
		//std::cout << "speed: " << speed <<", clampedSpeed: " << clampedSpeed << std::endl;
	}
}


