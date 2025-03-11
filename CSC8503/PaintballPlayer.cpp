#include "PaintballPlayer.h"
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
	//delete networkObject;
	//delete renderObject;
	//delete physicsObject;
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

void NCL::CSC8503::PaintballPlayer::SwitchState(PlayerState newState)
{
	currentState = newState;
	std::cout << "Switched to State: " << static_cast<int>(newState) << std::endl;
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
		nullptr, bulletColor, 1, false, Util::NCLToRP3d(GameManager::GetInstance().GetCameraFront()));

	//Disused way
	/*GameObjectFreeList::GetInstance().GetBullet(Util::NCLToRP3d(GameManager::GetInstance().GetCameraFront()), false,
		transform.GetPosition() + rp3d::Vector3(0, 4, 0), rp3d::Vector3(1, 1, 1),
		rp3d::Quaternion().identity(), bulletColor);*/
}

void NCL::CSC8503::PaintballPlayer::UpdatePlayerRotation()
{
	if (!camera) return; // ȷ   camera     
	float yaw = DegreesToRadians(camera->GetYaw()+180.f); 
	rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0.0f, yaw, 0.0f); // ֻ ޸  Y     ת
	this->GetTransform().SetOrientation(newRotation);
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

	if (isControl)
	{
		UpdatePlayerRotation();
		//Move(10.0f); //      
	}
	UpdateWeaponSelection();
}
