#include "TutorialGame.h"
#include "GameWorld.h"
#include "TextureLoader.h"
#include "Vector.h"
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
	
	
	
	//renderer->AddLight(light1);
#endif	
	

	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");
	ResourceManager::GetInstance().LoadAssets(renderer);
	G1.Init(world);


	InitialiseAssets();
	

	
	
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {

	
	navMesh = new NavigationMesh("Map1Navmesh","SecondMapMesh");

	InitWorld();
	InitCamera();
	
}

TutorialGame::~TutorialGame()	{

}

bool TutorialGame::pauseGame(){
	if (!pause) EventManager::Trigger(EventType::Game_Pause);
	return pause = true;
}
bool TutorialGame::UnpauseGame() {
	if (pause) EventManager::Trigger(EventType::Game_Resume);
	return pause = false;
}

void TutorialGame::UpdateGame(float dt) {
	if (pause) {
		// Only update the PushMachine and render the current frame.
		renderer->Render();
		return; // Skip the rest of the game updates.
	}

	const Camera& camera = world->GetMainCamera();
	inputManager.Update();
	GameManager::GetInstance().Update();
	UpdateKeys();

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

	Debug::Print("Force/Speed:" + std::to_string((int)forceMagnitude), Vector2(5, 80));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 25.0f;

	

	world->UpdateWorld(dt);

	G1.getRPworld()->update(dt);	//rp3d

	navMesh->DrawNavMesh();

	if (lockedObject) {
		Vector3 lockedScale = Util::RP3dToNCL(lockedObject->GetTransform().GetScale());
		LockedObjectMovement();

		world->GetMainCamera().UpdateCameraView3(
			Util::RP3dToNCL(lockedObject->GetTransform().GetPosition()) + Vector3(0, 1, 0),
			rp3d::max3(lockedScale.x, lockedScale.y, lockedScale.z) * 1.1f + 10.0f);
	}
	else
	{
		world->GetMainCamera().UpdateCamera(dt, forceMagnitude * 0.5f);
	}

	const Vector3& pos = camera.GetPosition();
	std::string posString = std::to_string((int)pos.x) + ", "
		+ std::to_string((int)pos.y) + ", " + std::to_string((int)pos.z);
	Debug::Print("Pos = " + posString, Vector2(60, 95), Debug::BLUE);
	if (false) {
		Vector3 camPos = camera.GetPosition();
		float yaw = DegreesToRadians(camera.GetYaw());
		float pitch = DegreesToRadians(-camera.GetPitch());
		Vector3 front(cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw));
		front = -Vector::Normalise(front);
		Vector3 target = camPos + front * 20.0f;
		Debug::DrawLine(target, target + Vector3(1, 0, 0), Debug::RED);
		Debug::DrawLine(target, target + Vector3(0, 1, 0), Debug::GREEN);
		Debug::DrawLine(target, target + Vector3(0, 0, 1), Debug::BLUE);
	}
	else
	{
		Debug::Print("+", Vector2(49, 51));
	}

	CalculatePathToPlayer();
	DisplayPath();
	MoveEnemyAlongPath();
	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);

	// Set initial top-down position and orientation
	world->GetMainCamera().SetPosition(Vector3(0, 20, 0)); // Initial position
	world->GetMainCamera().SetPitch(0.0f);               // Look straight down
	world->GetMainCamera().SetYaw(0.0f);

	lockedObject = nullptr;
	selectionObject = nullptr;
	forceMagnitude = 60.0f;
}

void TutorialGame::InitWorld() {
	EventManager::Trigger(EventType::Game_End);

	lockedObject = nullptr;
	selectionObject = nullptr;

	world->ClearAndErase();

	//ResourceManager::GetInstance().ReloadAnimations();

	playerObject = G1.AddPlayerClass(rp3d::Vector3(1, 52, -21));
	
	enemyObject = G1.AddPlayerClass(rp3d::Vector3(5, 1, -1));

	CharacterObject = G1.AddPlayerClass(rp3d::Vector3(0, 8, -30));

	forceMagnitude = 60.0f;



	Light light2(Vector3(12, 10, -5), Vector3(0, -1, 0), Vector4(0, 1, 0, 1), 1.0f, 45.0f);
	renderer->AddLight(light2);

	//FMOD
	speakerObj = G1.AddSphere(rp3d::Vector3(0, 25, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	speakerObj->SetAudioObject(new PaintballAudioObject(&speakerObj->GetTransform(),
		AudioSystem::GetInstance().GetEvent("event:/Congzheng/BGM2_3D")));
	speakerObj->GetAudioObject()->Play(true);

	//rp3d
	objList_pb.clear();
	float angleInRadians = 10.0f * PI / 180.0f;
	rp3d::Quaternion rotation = rp3d::Quaternion::fromEulerAngles(angleInRadians, 0.0f, angleInRadians);
	objList_pb.push_back(G1.AddCube(rp3d::Vector3(0, 25, -30), rp3d::Vector3(10, 1, 10), rp3d::Quaternion(0, 0, 0, 1.0f), 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
	objList_pb.push_back(G1.AddCube(rp3d::Vector3(1, 30, -30), rp3d::Vector3(5, 1, 5), rotation, 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
	objList_pb.push_back(G1.AddCube(rp3d::Vector3(2, 35, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));


	objList_pb.push_back(G1.AddCube(rp3d::Vector3(-10, 32, -11), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 1, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
	objList_pb.push_back(G1.AddCube(rp3d::Vector3(-10, 20, -7), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 1, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));


	objList_pb.push_back(G1.AddConcaveMesh(rp3d::Vector3(-100, 1, 0), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f), 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
	objList_pb.push_back(G1.AddSecondConcaveMesh(rp3d::Vector3(-50, 1, 500), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f), 0, Vector4(1.0f, 1.0f, 1.0f, 1.0f)));


	shoottest = G1.AddPlayerClass(rp3d::Vector3(13, 5, 10.f));

	GameManager::GetInstance().SetPlayer(shoottest);
	G1.SetGameState(GameState::InGame);
	
	//InitDefaultFloor();

	EventManager::Trigger(EventType::Game_Start);
}

void TutorialGame::UpdateKeys() {
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Left)) {
		if (selectionObject) {	//set colour to deselected;
			selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			selectionObject = nullptr;
		}

		rp3d::Vector3 dir = Util::NCLToRP3d(world->GetMainCamera().GetScreenDir(0.5f, 0.5f));
		rp3d::Vector3 pos = Util::NCLToRP3d(world->GetMainCamera().GetPosition());
		rp3d::Ray ray(pos, pos + dir * 1000);
		RaycastHitCallback  callback;
		G1.getRPworld()->raycast(ray, &callback);
		if (callback.rb && callback.rb->getUserData()) {
			selectionObject = (PaintballGameObject*)callback.rb->getUserData();
			selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
		}

	}


	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Right)) {
		if (!selectionObject) return;

		rp3d::Vector3 dir = Util::NCLToRP3d(world->GetMainCamera().GetScreenDir(0.5f, 0.5f));
		rp3d::Vector3 pos = Util::NCLToRP3d(world->GetMainCamera().GetPosition());
		rp3d::Ray ray(pos, pos + dir * 1000);
		RaycastHitCallback  callback;
		G1.getRPworld()->raycast(ray, &callback);
		if (callback.rb && callback.rb->getUserData()) {
			if (selectionObject == (PaintballGameObject*)callback.rb->getUserData())
				callback.rb->applyWorldForceAtWorldPosition(dir * forceMagnitude * 100, callback.hitpoint);
		}
	}
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::L)) {
		if (selectionObject) {
			selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			if (lockedObject == selectionObject) {
				lockedObject = nullptr;
			}
			else {
				lockedObject = selectionObject;
			}
		}
		else lockedObject = nullptr;
	}

	//shoot test
	if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::Q)) {
		shoottest->Attack();
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
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

	PaintballGameObject* target = lockedObject;

	const float& mass = target->GetPhysicsObject()->GetMass();
	float camYaw = world->GetMainCamera().GetYaw();
	
	shoottest->isControl = lockedObject == shoottest;

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
	//test
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
	if (!playerObject || !enemyObject) {
		Debug::Print("Player or Enemy object is not initialized!", Vector2(10, 20), Debug::RED);
		return;
	}

	rp3d::Vector3 startPos = enemyObject->GetTransform().GetPosition();
	rp3d::Vector3 endPos = playerObject->GetTransform().GetPosition();

	NavigationPath outPath;
	pathNodes.clear();

	if (navMesh->FindPath(Util::RP3dToNCL(startPos), Util::RP3dToNCL(endPos), outPath)) {
		Vector3 pos;
		while (outPath.PopWaypoint(pos)) {
			pathNodes.push_back(pos);
		}
	}

}

void TutorialGame::DisplayPath() {
	for (size_t i = 1; i < pathNodes.size(); ++i) {
		Vector3 a = pathNodes[i - 1];
		Vector3 b = pathNodes[i];
			Debug::DrawLine(a, b, Vector4(0, 1, 0, 1));
	}
}

void TutorialGame::MoveEnemyAlongPath() {
	if (pathNodes.empty()) {
		enemyObject->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
		return;
	}

	// Get current position and the next target position
	rp3d::Vector3 currentPos = enemyObject->GetTransform().GetPosition();
	rp3d::Vector3 targetPos = pathNodes.size() > 1 ? Util::NCLToRP3d(pathNodes[pathNodes.size() - 2])
		: Util::NCLToRP3d(pathNodes.back());
	targetPos.y = currentPos.y; // Keep enemy on the same Y level

	// Compute direction and distance to the target node
	rp3d::Vector3 direction = targetPos - currentPos;
	float distanceToTarget = direction.length();

	// Movement parameters
	float moveSpeed = 4.0f;
	float arrivalThreshold = 0.5f; // Distance at which a node is considered reached

	if (distanceToTarget < arrivalThreshold) {
		// Remove the reached node
		pathNodes.erase(pathNodes.begin());
		if (pathNodes.empty()) {
			enemyObject->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
			return;
		}
		// Update the target position to the new front of the path
		targetPos = pathNodes.size() > 1 ? Util::NCLToRP3d(pathNodes[pathNodes.size() - 2])
			: Util::NCLToRP3d(pathNodes.back());
		targetPos.y = currentPos.y;
		direction = targetPos - currentPos;
	}

	// Normalize direction and apply speed
	direction.normalize();
	rp3d::Vector3 velocity = direction * moveSpeed;

	// Set linear velocity
	enemyObject->GetPhysicsObject()->SetLinearVelocity(velocity);
}

