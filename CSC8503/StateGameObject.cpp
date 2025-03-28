#include "StateGameObject.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"

using namespace NCL;
using namespace CSC8503;

StateGameObject::StateGameObject()
	:PaintballGameObject(type = GameObjectType::test_stateObj, name)
{
	counter = 0.0f;
	stateMachine = new StateMachine();


}

StateGameObject::~StateGameObject() {
	delete stateMachine;
}

void StateGameObject::Update(float dt) {
	//std::cout << "StateGameObject::Update" << std::endl;
	PaintballGameObject::Update(dt);
	stateMachine->Update(dt);
}


void StateGameObject::MoveLeft(float dt) {
	counter += dt;
}

void StateGameObject::MoveRight(float dt) {
	counter -= dt;
}