#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PhysicsObject.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject(const std::string& objectName, GameObject* playerObj) {
	counter = 0.0f;
	stateMachine = new StateMachine();
	bool followcmd = false;
	GameObject* player = playerObj;
	/* State* stateA = new State([&](float dt) -> void
	{
		this -> MoveLeft(dt);
	}
	);
	State * stateB = new State([&](float dt) -> void
		{
		 this -> MoveRight(dt);
		}
	); */

	

	/* stateMachine->AddState(stateA);
	stateMachine -> AddState(stateB);
	stateMachine -> AddTransition(new StateTransition(stateA, stateB,[&]() -> bool
		{
		return this -> counter > 3.0f;
		}
	));
	stateMachine -> AddTransition(new StateTransition(stateB, stateA,[&]() -> bool
			{
		return this -> counter < 0.0f;
		}
	)); */
	SetName(objectName);

}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {

	stateMachine -> Update(dt);
}
StateMachine* StateGameObject::getstateMachine()
{
	return stateMachine;
}

void StateGameObject::setstateMachine(StateMachine* StateMachine)
{
	stateMachine = StateMachine;
}


void StateGameObject::MoveLeft(float dt) {

	GetPhysicsObject() -> AddForce({ -100 , 0 , 0 });
	counter += dt;

}

void StateGameObject::MoveRight(float dt) {

	GetPhysicsObject() -> AddForce({ 100 , 0 , 0 });
	counter -= dt;
}