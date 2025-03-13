#include "PaintballGameWorld.h"
#include "Camera.h"

using namespace NCL;
using namespace NCL::CSC8503;

PaintballGameWorld::PaintballGameWorld() {
	shuffleObjects = false;
	worldIDCounter = 0;
	worldStateCounter = 0;
}

PaintballGameWorld::~PaintballGameWorld() {
}

void PaintballGameWorld::Clear() {
	gameObjects.clear();
	worldIDCounter = 0;
	worldStateCounter = 0;
}

void PaintballGameWorld::ClearAndErase() {
	for (auto& i : gameObjects) {
		delete i;
	}
	Clear();
}

void PaintballGameWorld::AddGameObject(PaintballGameObject* o) {
	gameObjects.emplace_back(o);
	o->SetWorldID(worldIDCounter++);
	worldStateCounter++;
}

void PaintballGameWorld::RemoveGameObject(PaintballGameObject* o, bool andDelete) {
	gameObjects.erase(std::remove(gameObjects.begin(), gameObjects.end(), o), gameObjects.end());
	if (andDelete) {
		delete o;
	}
	worldStateCounter++;
}

void PaintballGameWorld::GetObjectIterators(
	PaintballGameObjectIterator& first,
	PaintballGameObjectIterator& last) const {

	first = gameObjects.begin();
	last = gameObjects.end();
}

void PaintballGameWorld::OperateOnContents(PaintballGameObjectFunc f) {
	for (PaintballGameObject* g : gameObjects) {
		f(g);
	}
}

void PaintballGameWorld::UpdateWorld(float dt) {
	//// Only update physics and game objects in playing states
	//if (currentstate != PaintballGameState::PLAYING &&
	//	currentstate != PaintballGameState::SERVERPLAYING &&
	//	currentstate != PaintballGameState::CLIENTPLAYING) {
	//	return;
	//}

	auto rng = std::default_random_engine{};
	unsigned seed = std::chrono::system_clock::now().time_since_epoch().count();
	std::default_random_engine e(seed);

	for (auto& obj : gameObjects) {
		// Sync physics transform to render transform
		PaintballPhysicsObject* physicsObj = obj->GetPhysicsObject();
		if (physicsObj) {
			rp3d::RigidBody& body = obj->GetPhysicsObject()->GetRigidbody();
			rp3d::Transform physicsTransform = body.getTransform();

			// Convert physics transform to game transform
			if (!obj->GetRenderObject() || !obj->GetRenderObject()->GetAnimation())
			{
				obj->GetTransform().SetRpTransform(physicsTransform);
			}
		}

		// Update animation if applicable
		PaintballRenderObject* renderObj = obj->GetRenderObject();
		if (renderObj && renderObj->GetAnimation()) {
			renderObj->UpdateAnimation(dt);
		}
	}

	if (shuffleObjects) {
		std::shuffle(gameObjects.begin(), gameObjects.end(), e);
	}
}

void PaintballGameWorld::SetGameState(PaintballGameState state) { 
	
	currentstate = state; 
	
}