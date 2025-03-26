#include "TutorialGame.h"
#include "GameWorld.h"
#include "TextureLoader.h"
#include "Vector.h"
#include"PaintballGameWorld.h"
#include"InputManager.h"
#include"UI.h"
#include"GameTechRenderer.h"
#include"Window.h"
#include <thread>


using namespace NCL;
using namespace CSC8503;


TutorialGame::TutorialGame() : controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse()) {
	world		= new PaintballGameWorld();
	
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
	renderer->Init();
	renderer->InitStructures();
#else 
	renderer = new GameTechRenderer(*world);
#endif	
	
	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	G1.SetGameState(PaintballGameState::LOADING);

	ResourceManager::GetInstance().LoadAssets(renderer);	//cost lot of time

	G1.Init(world, renderer, &controller);

	InitialiseAssets();
	G1.SetGameState(PaintballGameState::MENU);
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	InitWorld();
	InitCamera();
	
}

TutorialGame::~TutorialGame()	{

}


void TutorialGame::UpdateGame(float dt) {

	inputManager.Update();

	Vector2 mousePos = Window::GetMouse()->GetAbsolutePosition();
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Left))
		EventManager::Trigger(EventType::MouseLeftClick, mousePos.x, mousePos.y);

	switch (G1.GetGameState()) {
	case LOADING:
		//UpdateLoading(dt);	//loading is finished in AssetsLoading
		break;
	case PLAYING:
		UpdateGameBody(dt);
		break;
	case SERVERPLAYING:
		UpdateGameBody(dt);
		break;
	case CLIENTPLAYING:
		UpdateGameBody(dt);
		break;
	case CHOOSESERVER:
		UpdateUI(dt);
		
		break;
	case MENU:
		UpdateUI(dt);
		break;
	case SETTING:
		UpdateUI(dt);
		break;
	case PAUSED:
		if (G1.GetNetwork()) UpdateGameBody(dt);
		else UpdateUI(dt);
		break;
	case FAILURE:
		UpdateUI(dt);
		break;
	case FINISH:
		UpdateUI(dt);
		break;
	default:
		break;
	}

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateGameBody(float dt)
{
	const Camera& camera = world->GetMainCamera();
	GameManager::GetInstance().Update(dt);
	UpdateKeys();


	if (renderer->GetUI()->IsDebugMode()) {
		Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));
		Debug::Print("Force/Speed:" + std::to_string((int)G1.forceMagnitude), Vector2(5, 80));
	}

	G1.forceMagnitude += Window::GetMouse()->GetWheelMovement() * 25.0f;

	world->UpdateWorld(dt);

	UpdateUI(dt);

	G1.getRPworld()->update(dt);	

	if(G1.navMesh) G1.navMesh->DrawNavMesh();
	if (G1.lockedObject) {
		Vector3 lockedScale = Util::RP3dToNCL(G1.lockedObject->GetTransform().GetScale());
		LockedObjectMovement();

		if (G1.isView3) {
			world->GetMainCamera().UpdateCameraView3(
				Util::RP3dToNCL(G1.lockedObject->GetTransform().GetPosition()) + Vector3(0, 1, 0),
				rp3d::max3(lockedScale.x, lockedScale.y, lockedScale.z) * 1.1f + 10.0f);
		}
		else
		{
			world->GetMainCamera().UpdateCameraView3(
				Util::RP3dToNCL(G1.lockedObject->GetTransform().GetPosition()) + Vector3(0, 1, 0),
				-rp3d::max3(lockedScale.x, lockedScale.y, lockedScale.z) * 1.1f + 2.0f);
		}

	}
	else
	{
		world->GetMainCamera().UpdateCamera(dt, G1.forceMagnitude * 0.5f);
	}

	const Vector3& pos = camera.GetPosition();
	std::string posString = std::to_string((int)pos.x) + ", "
		+ std::to_string((int)pos.y) + ", " + std::to_string((int)pos.z);
	/*Debug::Print("Pos = " + posString, Vector2(60, 95), Debug::BLUE);*/
	if (renderer->GetUI()->IsDebugMode()) {
		Debug::Print("Pos = " + posString, Vector2(5, 95), Debug::BLUE);
	}



	if (G1.shoottest) { // If the player exists
		Vector3 playerPos = Util::RP3dToNCL(G1.shoottest->GetTransform().GetPosition());

		// Set the spotlight 5 units above the player
		Vector3 lightPos = playerPos + Vector3(0, 5, 0);

		// Always point the spotlight downward
		Vector3 lightDir = Vector3(0, -1, 0);
		//std::cout << "Updating light at: " << lightPos.x << ", " << lightPos.y << ", " << lightPos.z << std::endl;

		// Update the spotlight's position and direction
		renderer->UpdateLight(2, lightPos, lightDir);
	}

}

void TutorialGame::AssetsLoading() {
	std::cout << "[AssetsLoading] Before: assetsLoadedStep = " << assetsLoadedStep << std::endl;
	if (assetsLoadedStep < 5) {
		std::this_thread::sleep_for(std::chrono::milliseconds(500));
		assetsLoadedStep++;
	}
}

void TutorialGame::UpdateLoading(float dt)
{

	UpdateUI(dt);
	renderer->GetUI()->SetLoadingStep(1);
}

void TutorialGame::UpdateUI(float dt)
{
	renderer->Update(dt);
	renderer->GetUI()->Update(dt); //UI
}

void TutorialGame::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);

	world->GetMainCamera().SetPosition(Vector3(0,20,0)); 
	world->GetMainCamera().SetPitch(0.0f);               
	world->GetMainCamera().SetYaw(0.0f);

	G1.lockedObject = nullptr;
	G1.selectionObject = nullptr;
	G1.forceMagnitude = 35.0f;
}

void TutorialGame::InitWorld() {
	
	if (false) {
		G1.InitWorld(1);	//for data-driven
	}
	else {
		G1.InitWorld();		//add objs manually
	}
}

void TutorialGame::UpdateKeys() {
	if (!G1.isPhysEnabled())return;

	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Left)) {
		if (G1.selectionObject) {	//set colour to deselected;
			G1.selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			G1.selectionObject = nullptr;
		}

		rp3d::Vector3 dir = Util::NCLToRP3d(world->GetMainCamera().GetScreenDir(0.5f, 0.5f));
		rp3d::Vector3 pos = Util::NCLToRP3d(world->GetMainCamera().GetPosition());
		rp3d::Ray ray(pos, pos + dir * 1000);
		RaycastHitCallback  callback;
		G1.getRPworld()->raycast(ray, &callback);
		if (callback.rb && callback.rb->getUserData()) {
			//G1.GetInstance().DeleteObject((PaintballGameObject*)callback.rb->getUserData());

			G1.selectionObject = (PaintballGameObject*)callback.rb->getUserData();
			G1.selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
		}

	}

	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Middle)) {
		if (G1.selectionObject) {
			if(true)
				G1.selectionObject->SetActive(false);
			else {
				G1.DeleteObject(G1.selectionObject);
				G1.selectionObject = nullptr;
			}
		}
	}

	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Right)) {
		if (!G1.selectionObject) return;

		rp3d::Vector3 dir = Util::NCLToRP3d(world->GetMainCamera().GetScreenDir(0.5f, 0.5f));
		rp3d::Vector3 pos = Util::NCLToRP3d(world->GetMainCamera().GetPosition());
		rp3d::Ray ray(pos, pos + dir * 1000);
		RaycastHitCallback  callback;
		G1.getRPworld()->raycast(ray, &callback);
		if (callback.rb && callback.rb->getUserData()) {
			if (G1.selectionObject == (PaintballGameObject*)callback.rb->getUserData())
				callback.rb->applyWorldForceAtWorldPosition(dir * G1.forceMagnitude * 100, callback.hitpoint);
		}
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::L)) {
		if (G1.selectionObject) {
			G1.selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			if (G1.lockedObject == G1.selectionObject) {
				G1.lockedObject = nullptr;
			}
			else {
				G1.lockedObject = G1.selectionObject;
			}
		}
		else G1.lockedObject = nullptr;
	}

	//shoot test
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::Q)) {
		if(G1.shoottest) G1.shoottest->Attack();
		else {
			GameManager::GetInstance().AddObject(GameObjectType::bullet,
				Util::NCLToRP3d(GameManager::GetInstance().GetMainCamera().GetPosition() + GameManager::GetInstance().GetCameraFront() * 3.f),
				rp3d::Vector3(1, 1, 1), rp3d::Quaternion().identity(),
				Vector4(1, 1, 1, 1), "", "basic", "", "", "", "", "", "", "", "basic", 1, false, Util::NCLToRP3d(GameManager::GetInstance().GetCameraFront()));
		}
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); 
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); 
	}

}

void TutorialGame::LockedObjectMovement() {
	const Matrix4& view = world->GetMainCamera().BuildViewMatrix();
	const Matrix4& camWorld = Matrix::Inverse(view);

	const Vector3& rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis = Vector::Normalise(fwdAxis);

	PaintballGameObject* target = G1.lockedObject;

	const float& mass = target->GetPhysicsObject()->GetMass();
	float camYaw = world->GetMainCamera().GetYaw();
	
	if (G1.shoottest) G1.shoottest->isControl = G1.lockedObject == G1.shoottest;

	if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
		target->GetPhysicsObject()->AddForce(Util::NCLToRP3d(fwdAxis * G1.forceMagnitude));
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
		target->GetPhysicsObject()->AddForce(Util::NCLToRP3d(-fwdAxis * G1.forceMagnitude));
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::LEFT)) {
		target->GetPhysicsObject()->AddForce(Util::NCLToRP3d(-rightAxis * G1.forceMagnitude));
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
		target->GetPhysicsObject()->AddForce(Util::NCLToRP3d(rightAxis * G1.forceMagnitude));
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::N) && target->GetPhysicsObject()->isStand()) {
		target->GetPhysicsObject()->ApplyLinearImpulse(rp3d::Vector3(0, G1.forceMagnitude * 0.025f, 0));
	}
	if (Window::GetKeyboard()->KeyDown(KeyCodes::M)) {
		target->GetPhysicsObject()->AddForce(rp3d::Vector3(0, -G1.forceMagnitude, 0));
	}
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/


void TutorialGame::InitDefaultFloor() {
	G1.AddFloorToWorld(rp3d::Vector3(0, -2, 0));
	
}


/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 
letting you move the camera around. 

*/

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/


void TutorialGame::CalculatePathToPlayer() {
	if (!G1.playerObject || !G1.enemyObject) {
		Debug::Print("Player or Enemy object is not initialized!", Vector2(10, 20), Debug::RED);
		return;
	}

	rp3d::Vector3 startPos = G1.enemyObject->GetTransform().GetPosition();
	rp3d::Vector3 endPos = G1.playerObject->GetTransform().GetPosition();

	NavigationPath outPath;
	G1.pathNodes.clear();

	if (G1.navMesh->FindPath(Util::RP3dToNCL(startPos), Util::RP3dToNCL(endPos), outPath)) {
		Vector3 pos;
		while (outPath.PopWaypoint(pos)) {
			G1.pathNodes.push_back(pos);
		}
	}

}

void TutorialGame::DisplayPath() {
	for (size_t i = 1; i < G1.pathNodes.size(); ++i) {
		Vector3 a = G1.pathNodes[i - 1];
		Vector3 b = G1.pathNodes[i];
			Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}

void TutorialGame::MoveEnemyAlongPath() {
	if (!G1.enemyObject) return;

	if (G1.pathNodes.empty()) {
		G1.enemyObject->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
		return;
	}

	// Get current position and the next target position
	rp3d::Vector3 currentPos = G1.enemyObject->GetTransform().GetPosition();
	rp3d::Vector3 targetPos = G1.pathNodes.size() > 1 ? Util::NCLToRP3d(G1.pathNodes[G1.pathNodes.size() - 2])
		: Util::NCLToRP3d(G1.pathNodes.back());
	targetPos.y = currentPos.y; // Keep enemy on the same Y level

	// Compute direction and distance to the target node
	rp3d::Vector3 direction = targetPos - currentPos;
	float distanceToTarget = direction.length();

	// Movement parameters
	float moveSpeed = 4.0f;
	float arrivalThreshold = 0.5f; // Distance at which a node is considered reached

	if (distanceToTarget < arrivalThreshold) {
		// Remove the reached node
		G1.pathNodes.erase(G1.pathNodes.begin());
		if (G1.pathNodes.empty()) {
			G1.enemyObject->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
			return;
		}
		// Update the target position to the new front of the path
		targetPos = G1.pathNodes.size() > 1 ? Util::NCLToRP3d(G1.pathNodes[G1.pathNodes.size() - 2])
			: Util::NCLToRP3d(G1.pathNodes.back());
		targetPos.y = currentPos.y;
		direction = targetPos - currentPos;
	}

	// Normalize direction and apply speed
	direction.normalize();
	rp3d::Vector3 velocity = direction * moveSpeed;

	// Set linear velocity
	G1.enemyObject->GetPhysicsObject()->SetLinearVelocity(velocity);
}

void TutorialGame::ShowMainPage() {
	Debug::UpdateRenderables(0.1f);
	Debug::DrawTex(*ResourceManager::GetInstance().GetTexture("basic"), Vector2(0, 0), Vector2(100, 100), Debug::WHITE);
	Debug::Print("Main Page : Press SPACE to start, ESCAPE to quit", Vector2(5, 65), Debug::RED);
	renderer->Render();
}

void TutorialGame::ShowPausedPage() {
	Debug::Print("Paused : Press U to unpause game", Vector2(20, 50), Debug::WHITE);
	renderer->Render();
}
void TutorialGame::ShowMenuPage() {
	Vector4 titleColour = Debug::GREEN;

	Debug::Print("Press ESCAPE to quit game", Vector2(20, 45), Debug::BLACK);
	Debug::Print("Press N for a main game", Vector2(20, 50), Debug::BLACK);

	Debug::Print("MENU", Vector2(20, 35), Debug::BLACK);
	Debug::Print("Press U to close menu", Vector2(20, 55), Debug::BLACK);
}