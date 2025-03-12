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

	
	G1.navMesh = new NavigationMesh("Map1Navmesh","SecondMapMesh");

	InitWorld();
	InitCamera();
	
}

TutorialGame::~TutorialGame()	{

}


void TutorialGame::UpdateGame(float dt) {
	const Camera& camera = world->GetMainCamera();
	inputManager.Update();
	GameManager::GetInstance().Update(dt);
	UpdateKeys();

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

	Debug::Print("Force/Speed:" + std::to_string((int)forceMagnitude), Vector2(5, 80));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 25.0f;
	Debug::Print("Press P to pause or M to open menu", Vector2(5, 85));

	

	world->UpdateWorld(dt);

	G1.navMesh->DrawNavMesh();

	if (G1.lockedObject) {
		Vector3 lockedScale = Util::RP3dToNCL(G1.lockedObject->GetTransform().GetScale());
		LockedObjectMovement();

		world->GetMainCamera().UpdateCameraView3(
			Util::RP3dToNCL(G1.lockedObject->GetTransform().GetPosition()) + Vector3(0, 1, 0),
			rp3d::max3(lockedScale.x, lockedScale.y, lockedScale.z) * 1.1f + 10.0f);
	}
	else
	{
		world->GetMainCamera().UpdateCamera(dt, forceMagnitude * 0.5f);
	}

	const Vector3& pos = camera.GetPosition();
	std::string posString = std::to_string((int)pos.x) + ", "
		+ std::to_string((int)pos.y) + ", " + std::to_string((int)pos.z);
	Debug::Print("Pos = " + posString, Vector2(60, 95));
	if (true) {
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

	G1.lockedObject = nullptr;
	G1.selectionObject = nullptr;
	forceMagnitude = 60.0f;
}

void TutorialGame::InitWorld() {
	//return;

	G1.lockedObject = nullptr;
	G1.selectionObject = nullptr;

	world->ClearAndErase();

	if (false) {
		G1.InitWorld(1);
	}
	else {
		//ResourceManager::GetInstance().ReloadAnimations();

		G1.playerObject = G1.AddPlayerClass(rp3d::Vector3(1, 52, -21));

		G1.enemyObject = G1.AddPlayerClass(rp3d::Vector3(5, 1, -1));

		G1.CharacterObject = G1.AddPlayerClass(rp3d::Vector3(0, 8, -30));

		forceMagnitude = 60.0f;



		Light light2(Vector3(12, 10, -5), Vector3(0, -1, 0), Vector4(1, 1, 1, 1), 1.0f, 45.0f, LightType::Spot);
		renderer->AddLight(light2);

		//FMOD
		G1.speakerObj = G1.AddSphere(rp3d::Vector3(0, 25, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
		G1.speakerObj->SetAudioObject(new PaintballAudioObject(&G1.speakerObj->GetTransform(),
			AudioSystem::GetInstance().GetEvent("event:/Congzheng/BGM2_3D")));
		//G1.speakerObj->GetAudioObject()->Play(true);

		//rp3d
		float angleInRadians = 10.0f * PI / 180.0f;
		rp3d::Quaternion rotation = rp3d::Quaternion::fromEulerAngles(angleInRadians, 0.0f, angleInRadians);
		/*G1.AddCube(rp3d::Vector3(0, 25, -30), rp3d::Vector3(10, 1, 10), rp3d::Quaternion(0, 0, 0, 1.0f), 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddCube(rp3d::Vector3(1, 30, -30), rp3d::Vector3(5, 1, 5), rotation, 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddCube(rp3d::Vector3(2, 35, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));


		G1.AddCube(rp3d::Vector3(-10, 32, -11), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 1, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddCube(rp3d::Vector3(-10, 20, -7), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 1, Vector4(1.0f, 0.0f, 0.0f, 1.0f));*/


		//FIRST MAP
		G1.AddConcaveMesh(rp3d::Vector3(-80, -4, -32), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetFloor1Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-117, 1, -119), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetMainWallMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-215, 1, -25), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetDoorWayMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-5, 0, -55), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetOppDoorWayMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-121, 2, -55), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetMidDoorMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-62, 7, -78), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetWallMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-94, 2, -78), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetWall2Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-150, 2, -78), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetOpenWallMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-153, 2, 88), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetWall3Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-31, -2, -38), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetWall4Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-41, 1, 53), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetCylinderMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-173, 2, -19), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetBigCylinderMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-194, 2, 123), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetTargetMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-96, 1, 70), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetBigCubeMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));



		//SECOND MAP
		G1.AddConcaveMesh(rp3d::Vector3(-50, 0, 169), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetFloor2Mesh(), Vector4(1.0f, 1.0f, 1.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-200, 4, 109), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetMainWall2Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-353, -4, -77), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetDoorWayM2Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(60, 2, 290), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetOppDoorWayM2Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-492, -154, -462), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetWall1M2Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-61, 2, 305), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetWall2M2Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-328, 2, -193), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetWall3M2Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-285, 2, -250), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetColumnsM2Mesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-253, 1, -350), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetMidTunnelMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-5, 2, 465), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetElevationMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-22, 2, 248), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetBridgeMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-37, 21, 384), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetElevRailMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-61.5, 26, 297.5), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetMidRailMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));
		G1.AddConcaveMesh(rp3d::Vector3(-88, 22, 187), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
			ResourceManager::GetInstance().GetBridgeRailMesh(), Vector4(1.0f, 0.0f, 0.0f, 1.0f));


		G1.shoottest = G1.AddPlayerClass(rp3d::Vector3(13, 5, 10.f));
		G1.AddTrap();
		GameManager::GetInstance().SetPlayer(G1.shoottest);
		G1.SetGameState(GameState::InGame);

		//InitDefaultFloor();


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
				callback.rb->applyWorldForceAtWorldPosition(dir * forceMagnitude * 100, callback.hitpoint);
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
				Vector4(1, 1, 1, 1), nullptr, 1, false, Util::NCLToRP3d(GameManager::GetInstance().GetCameraFront()));
		}
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

	PaintballGameObject* target = G1.lockedObject;

	const float& mass = target->GetPhysicsObject()->GetMass();
	float camYaw = world->GetMainCamera().GetYaw();
	
	if (G1.shoottest) G1.shoottest->isControl = G1.lockedObject == G1.shoottest;

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
	Debug::DrawTex(*ResourceManager::GetInstance().GetBasicTex(), Vector2(0, 0), Vector2(100, 100), Debug::WHITE);
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

