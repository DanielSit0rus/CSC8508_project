#include "TutorialGame.h"
#include "GameWorld.h"
#include "PhysicsObject.h"
#include "RenderObject.h"
#include "TextureLoader.h"
#include "Vector.h"

#include "PositionConstraint.h"
#include "OrientationConstraint.h"
#include "StateGameObject.h"



using namespace NCL;
using namespace CSC8503;



TutorialGame::TutorialGame() : controller(*Window::GetWindow()->GetKeyboard(), *Window::GetWindow()->GetMouse()) {
	world		= new GameWorld();
#ifdef USEVULKAN
	renderer	= new GameTechVulkanRenderer(*world);
	renderer->Init();
	renderer->InitStructures();
#else 
	renderer = new GameTechRenderer(*world);
#endif
	physics		= new PhysicsSystem(*world);
	//PushMachine = new PushdownMachine(new gameScreen());

	forceMagnitude	= 10.0f;
	useGravity		= false;
	inSelectionMode = false;

	world->GetMainCamera().SetController(controller);

	controller.MapAxis(0, "Sidestep");
	controller.MapAxis(1, "UpDown");
	controller.MapAxis(2, "Forward");

	controller.MapAxis(3, "XLook");
	controller.MapAxis(4, "YLook");

	InitialiseAssets();
}

/*

Each of the little demo scenarios used in the game uses the same 2 meshes, 
and the same texture and shader. There's no need to ever load in anything else
for this module, even in the coursework, but you can add it if you like!

*/
void TutorialGame::InitialiseAssets() {
	cubeMesh	= renderer->LoadMesh("cube.msh");
	sphereMesh	= renderer->LoadMesh("sphere.msh");
	catMesh		= renderer->LoadMesh("ORIGAMI_Chat.msh");
	kittenMesh	= renderer->LoadMesh("Kitten.msh");
	gooseMesh = renderer->LoadMesh("goose.msh");

	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	bonusMesh	= renderer->LoadMesh("19463_Kitten_Head_v1.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitCamera();
	InitWorld();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete catMesh;
	delete kittenMesh;
	delete enemyMesh;
	delete bonusMesh;

	delete basicTex;
	delete basicShader;

	delete physics;
	delete renderer;
	delete world;
}

bool TutorialGame::pauseGame(){

	return pause = true;

}
bool TutorialGame::UnpauseGame() {

	return pause = false;

}

void TutorialGame::UpdateGame(float dt) {


	Debug::Print("Score : " + std::to_string(physics->score), Vector2(5, 90));
	if (pause) {
		// Only update the PushMachine and render the current frame.
		//PushMachine->Update(dt);
		renderer->Render();
		return; // Skip the rest of the game updates.
	}


	if (!inSelectionMode) {
		world->GetMainCamera().UpdateCamera(dt);
	}
	if (lockedObject != nullptr) {
		Vector3 objPos = lockedObject->GetTransform().GetPosition();
		Vector3 camPos = objPos + lockedOffset;

		Matrix4 temp = Matrix::View(camPos, objPos, Vector3(0,1,0));

		Matrix4 modelMat = Matrix::Inverse(temp);

		Quaternion q(modelMat);
		Vector3 angles = q.ToEuler(); //nearly there now!

		world->GetMainCamera().SetPosition(camPos);
		world->GetMainCamera().SetPitch(angles.x);
		world->GetMainCamera().SetYaw(angles.y);
	}

	if (!kittens.empty()) {
		for (StateGameObject* kitten : kittens) {
			if (kitten->getstateMachine()) {
				kitten->getstateMachine()->Update(dt);
			}
		}
	}
	if (GooseObject)
	{
		GooseObject->Update(dt);
	}
	
	//PushMachine->Update(dt);

	UpdateKeys();
	
	if (playerObject) {
		Vector3 playerPos = playerObject->GetTransform().GetPosition();
		Vector2 mouseDelta = Window::GetMouse()->GetRelativePosition();
		// Set the camera directly above the player with a fixed offset
		Vector3 camPos = playerPos + Vector3(0, 2, 0); // Adjust height (Y) as needed
		world->GetMainCamera().SetPosition(camPos);
		float yaw = 0.0f;
		float pitch = 0.0f;

		yaw -= mouseDelta.x * 1.1f;
		pitch -= mouseDelta.y * 1.1f;
		pitch = std::clamp(pitch, -89.0f, 89.0f);

		Quaternion yawRotation = Quaternion::AxisAngleToQuaterion(Vector3(0, 1, 0), yaw);
		Quaternion pitchRotation = Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), pitch);
		Quaternion finalRotation = yawRotation * pitchRotation;
		playerObject->GetTransform().SetOrientation(yawRotation);
		//world->GetMainCamera().SetViewMatrix(playerTransform.GetMatrix());
		// Orient the camera to look straight down
		//world->GetMainCamera().SetPitch(0.0f); // Look straight down
		//world->GetMainCamera().SetYaw(0.0f);
	} 

	if (useGravity) {
		Debug::Print("(G)ravity on", Vector2(5, 95), Debug::RED);
	}
	else {
		Debug::Print("(G)ravity off", Vector2(5, 95), Debug::RED);
	}
	//This year we can draw debug textures as well!
	//Debug::DrawTex(*basicTex, Vector2(10, 10), Vector2(5, 5), Debug::MAGENTA);

	RayCollision closestCollision;
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::K) && selectionObject) {
		Vector3 rayPos;
		Vector3 rayDir;

		rayDir = selectionObject->GetTransform().GetOrientation() * Vector3(0, 0, -1);

		rayPos = selectionObject->GetTransform().GetPosition();

		Ray r = Ray(rayPos, rayDir);

		if (world->Raycast(r, closestCollision, true, selectionObject)) {
			if (objClosest) {
				objClosest->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
			}
			objClosest = (GameObject*)closestCollision.node;

			objClosest->GetRenderObject()->SetColour(Vector4(1, 0, 1, 1));
		}
	}

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));
	
	SelectObject();
	MoveSelectedObject();

	world->UpdateWorld(dt);
	renderer->Update(dt);
	physics->Update(dt);

	renderer->Render();
	Debug::UpdateRenderables(dt);
}

void TutorialGame::UpdateKeys() {
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
		selectionObject = nullptr;
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::G)) {
		useGravity = !useGravity; //Toggle gravity!
		physics->UseGravity(useGravity);
	}
	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F9)) {
		world->ShuffleConstraints(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F10)) {
		world->ShuffleConstraints(false);
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F7)) {
		world->ShuffleObjects(true);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F8)) {
		world->ShuffleObjects(false);
	}

	if (lockedObject) {
		LockedObjectMovement();
	}
	else {
		DebugObjectMovement();
	}

	if (playerObject) {
<<<<<<< Updated upstream
=======


		Vector3 playerPos = playerObject->GetTransform().GetPosition();
		Vector2 mouseDelta = Window::GetMouse()->GetRelativePosition();
		// Set the camera directly above the player with a fixed offset
		Vector3 camPos = playerPos + Vector3(0, 1, 1.0f); // Adjust height (Y) as needed
		world->GetMainCamera().SetPosition(camPos);
		float yaw = 0.0f;
		float pitch = 0.0f;

		yaw -= mouseDelta.x * 1.1f;
		pitch -= mouseDelta.y * 1.1f;
		pitch = std::clamp(pitch, -89.0f, 89.0f);

		Quaternion yawRotation = Quaternion::Quaternion(Vector3(0, 1, 0), yaw);
		Quaternion pitchRotation = Quaternion::AxisAngleToQuaterion(Vector3(1, 0, 0), pitch);
		//Quaternion finalRotation = yawRotation * pitchRotation;
		playerObject->GetTransform().SetOrientation(yawRotation);

>>>>>>> Stashed changes
		Matrix4 view = world->GetMainCamera().BuildViewMatrix();
		Matrix4 camWorld = Matrix::Inverse(view);

		Vector3 rightAxis = Vector3(camWorld.GetColumn(0));
		//Vector3 right = Vector3(camWorld.GetColumn(0));
		//
		Vector3 fwdAxis = Vector::Cross(Vector3(0, 1, 0), rightAxis);

		//Vector3 forward = Vector::Cross(Vector3(0, 1, 0), right);
		

		Transform playerTransform = playerObject->GetTransform();

		// Extract the right and forward vectors from the player's orientation
		Quaternion playerOrientation = playerTransform.GetOrientation();
		//Vector3 rightAxis = playerOrientation * Vector3(1, 0, 0); // Local right
		//Vector3 fwdAxis = playerOrientation * Vector3(0, 0, -1);

		fwdAxis.y = 0; // Keep movement in the horizontal plane
		rightAxis.y = 0;

		fwdAxis = Vector::Normalise(fwdAxis);
		rightAxis = Vector::Normalise(rightAxis);
		Vector3 movement = Vector3(0, 0, 0);
		if (Window::GetKeyboard()->KeyDown(KeyCodes::W)) {
			movement += fwdAxis * 20.0f;
		}
		if (Window::GetKeyboard()->KeyDown(KeyCodes::S)) {
			movement -= fwdAxis * 20.0f;
		}
		if (Window::GetKeyboard()->KeyDown(KeyCodes::A)) {
			movement -= rightAxis * 20.0f;
		}
		if (Window::GetKeyboard()->KeyDown(KeyCodes::D)) {
			movement += rightAxis * 20.0f;
		}
		
		// Apply movement force
		if (Vector::Length(movement) > 0.0f) {
			playerObject->GetPhysicsObject()->AddForce(movement);

			// Turn the cat to face the movement direction
		//Vector3 velocity = playerObject->GetPhysicsObject()->GetLinearVelocity();
		//	if (Vector::Length(velocity) > 0.01f) { // Only update if there's significant velocity
		//		 // Current forward vector
		//		Vector3 desiredDirection = Vector::Normalise(velocity); // Normalize velocity
		//		Vector3 forwarddir = playerObject->GetTransform().GetOrientation() * Vector3(0, 0, 1);
		//		Vector3 rotationAxis = Vector::Cross(forwarddir, desiredDirection);
		//		float angle = acos(Vector::Dot(forwarddir, desiredDirection)); // Angle in radians

		//		if (angle > 0.01f) { // Apply torque only if there's a meaningful angle
		//			float torqueScale = 2.0f; // Arbitrary scale for control
		//			Vector3 torque = rotationAxis * angle * torqueScale;
		//			playerObject->GetPhysicsObject()->AddTorque(torque);
		//		}
		//	}

		}
		else {

			playerObject->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
			playerObject->GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));

		}
	} 
}

void TutorialGame::LockedObjectMovement() {
	Matrix4 view		= world->GetMainCamera().BuildViewMatrix();
	Matrix4 camWorld	= Matrix::Inverse(view);

	Vector3 rightAxis = Vector3(camWorld.GetColumn(0)); //view is inverse of model!

	//forward is more tricky -  camera forward is 'into' the screen...
	//so we can take a guess, and use the cross of straight up, and
	//the right axis, to hopefully get a vector that's good enough!

	Vector3 fwdAxis = Vector::Cross(Vector3(0, 1, 0), rightAxis);
	fwdAxis.y = 0.0f;
	fwdAxis = Vector::Normalise(fwdAxis);

	if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
		selectionObject->GetPhysicsObject()->AddForce(fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
		selectionObject->GetPhysicsObject()->AddForce(-fwdAxis);
	}

	if (Window::GetKeyboard()->KeyDown(KeyCodes::NEXT)) {
		selectionObject->GetPhysicsObject()->AddForce(Vector3(0,-10,0));
	}
}

void TutorialGame::DebugObjectMovement() {
//If we've selected an object, we can manipulate it with some key presses
	if (inSelectionMode && selectionObject) {
		//Twist the selected object!
		if (Window::GetKeyboard()->KeyDown(KeyCodes::LEFT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(-10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM7)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, 10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM8)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(0, -10, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::RIGHT)) {
			selectionObject->GetPhysicsObject()->AddTorque(Vector3(10, 0, 0));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::UP)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, -10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::DOWN)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, 0, 10));
		}

		if (Window::GetKeyboard()->KeyDown(KeyCodes::NUM5)) {
			selectionObject->GetPhysicsObject()->AddForce(Vector3(0, -10, 0));
		}
	}
}

void TutorialGame::InitCamera() {
	world->GetMainCamera().SetNearPlane(0.1f);
	world->GetMainCamera().SetFarPlane(500.0f);

	// Set initial top-down position and orientation
	world->GetMainCamera().SetPosition(Vector3(0, 20, 0)); // Initial position
	world->GetMainCamera().SetPitch(-90.0f);               // Look straight down
	world->GetMainCamera().SetYaw(0.0f);
	lockedObject = nullptr;
}

void TutorialGame::runKittenAI()
{
	
	for (StateGameObject* kitten : kittens) {
		State* stateIdle = new State([kitten](float dt) -> void {
			//std::cout << "\nKitten is idle\n";
			});

		State* stateFollow = new State([kitten, this](float dt) -> void {
			Vector3 playerPos = playerObject->GetTransform().GetPosition();
			Vector3 kittenPos = kitten->GetTransform().GetPosition();
			Vector3 direction = playerPos - kittenPos;

			if (Vector::Length(direction) > 0.8f) {
				direction = Vector::Normalise(direction);
				kitten->GetPhysicsObject()->AddForce(direction * 25.0f);
			}
			else {
				kitten->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
				kitten->GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));
			}

			//std::cout << "\nKitten is following the player\n";
			});

		State* stateHome = new State([kitten](float dt) -> void {
			kitten->GetPhysicsObject()->SetLinearVelocity(Vector3(0, 0, 0));
			kitten->GetPhysicsObject()->SetAngularVelocity(Vector3(0, 0, 0));
			kitten->GetTransform().SetPosition(Vector3(4, 0, -2.0f));
			});

		StateMachine* kittenStateMachine = new StateMachine();

		kittenStateMachine->AddState(stateIdle);
		kittenStateMachine->AddState(stateFollow);
		kittenStateMachine->AddState(stateHome);

		kittenStateMachine->AddTransition(new StateTransition(stateIdle, stateFollow, [kitten]() -> bool {
			return kitten->isFollowing;
			}));

		kittenStateMachine->AddTransition(new StateTransition(stateFollow, stateHome, [kitten]() -> bool {
			return !kitten->isFollowing;
			}));

		// Assign this state machine to the kitten
		kitten->setstateMachine(kittenStateMachine);
	}

}


void TutorialGame::InitWorld() {
	world->ClearAndErase();
	physics->Clear();
	//BridgeConstraintTest();
	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);
	//testStateObject = AddStateObjectToWorld(Vector3(0, 10, -10));
	//InitGameExamples();
	playerObject = AddPlayerToWorld(Vector3(2, 1, 2));
	//kittenObject1 = AddKitttenToWorld(Vector3(2, 2, 5));
	AddStateObjectToWorld(Vector3(6, 1, 22), playerObject);
	AddStateObjectToWorld(Vector3(18, 1, 2), playerObject);
	AddStateObjectToWorld(Vector3(20, 1, 26), playerObject);

<<<<<<< Updated upstream
<<<<<<< Updated upstream
	AddcylinderToWorld(Vector3(2, 7, 5));
=======
	AddcylinderToWorld(Vector3(1, 6, 7));
>>>>>>> Stashed changes
=======
	AddcylinderToWorld(Vector3(1, 6, 7));
>>>>>>> Stashed changes
	AddSphereToWorld(Vector3(2, 1, 5),1);

	AddCubeToWorld(Vector3(22, 0, 22), Vector3(1, 2, 1), 100.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	GooseObject = AddGooseToWorld(Vector3(10, 2, 10), playerObject);
	//DoorFrame = AddCubeToWorld(Vector3(16, 2, 4), Vector3(1, 2, 1),0.0f, Vector4(1.0f,0.0f,0.0f,1.0f));
	AddGate();
	homecube = AddCubeToWorld(Vector3(4, 0, -2.0f), Vector3(1, 0.1f, 1), 0.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
	homecube->SetName("home");

	std::vector<std::vector<int>> mazePattern = {
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
	{1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 0, 1, 0, 1},
	{1, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1, 0, 1, 0, 1},
	{1, 0, 1, 0, 0, 0, 1, 1, 1, 0, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 0, 0, 0, 0, 1, 1, 1, 1, 0, 0, 0, 0, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1, 1, 1},
	{1, 0, 1, 1, 1, 0, 1, 1, 1, 1, 0, 0, 1, 0, 1},
	{1, 0, 0, 0, 1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 0, 1, 1, 1, 1, 1, 1, 1, 0, 1, 1, 1},
	{1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
	{1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1}

	};

	InitMazeWorld(mazePattern, Vector3(1, 2, 1), 2.0f);
	runKittenAI();
	InitDefaultFloor();
}

void TutorialGame::AddGate() {

	Vector3 gateSize = Vector3(1.0f, 2.0f, 0.3f); // Dimensions of the gate
	float gateMass = 10.0f;                  // Mass of the gate
	Vector3 pivotSize = Vector3(0.3f,0.3f, 0.3f); // Dimensions of the pivot (anchor)

	Vector3 gateStartPos = Vector3(13, 2, 4); // Initial position of the gate
	Vector3 pivotPos = Vector3(16, 2, 3);   // Position of the pivot point

	// Create the pivot point (immovable)
	GameObject* pivot = AddCubeToWorld(pivotPos, pivotSize, 0, Vector4(0.0f,0.0f,1.0f,1.0f)); // 0 mass = static

	// Create the gate object (movable)
	GameObject* gate = AddCubeToWorld(gateStartPos, gateSize, gateMass, Vector4(0.0f, 0.0f, 1.0f, 1.0f));

	// Attach the gate to the pivot with a PositionConstraint
	float hingeDistance = 2.0f; // Distance from pivot to gate (acts as a hinge radius)
	PositionConstraint* gateConstraint = new PositionConstraint(pivot, gate, hingeDistance);
	world->AddConstraint(gateConstraint);



}

void TutorialGame::BridgeConstraintTest() {
	Vector3 cubeSize = Vector3(8, 8, 8);
	float invCubeMass = 5; // how heavy the middle pieces are
	int numLinks = 10;
	float maxDistance = 30; // constraint distance
	float cubeDistance = 20; // distance between links
	Vector3 startPos = Vector3(0, 0, 0);
	GameObject * start = AddCubeToWorld(startPos + Vector3(0, 0, 0), cubeSize, 0);
	GameObject * end = AddCubeToWorld(startPos + Vector3((numLinks + 2) * cubeDistance, 0, 0), cubeSize, 0);
	GameObject * previous = start;
	for (int i = 0; i < numLinks; ++i) {
		GameObject * block = AddCubeToWorld(startPos + Vector3((i + 1) * cubeDistance, 0, 0), cubeSize, invCubeMass);
		PositionConstraint * constraint = new PositionConstraint(previous,block, maxDistance);
		world -> AddConstraint(constraint);
		previous = block;
	}
	PositionConstraint * constraint = new PositionConstraint(previous, end, maxDistance);
	world -> AddConstraint(constraint);
}



StateGameObject* TutorialGame::AddStateObjectToWorld(const Vector3& position, GameObject* player)
{
	StateGameObject* kitten = new StateGameObject("Kitten", player);

	SphereVolume* volume = new SphereVolume(0.2f);
	kitten->SetBoundingVolume((CollisionVolume*)volume);
	kitten->GetTransform()
		.SetScale(Vector3(0.5f, 0.5f, 0.5f))
		.SetPosition(position);

	kitten->SetRenderObject(new RenderObject(&kitten->GetTransform(), catMesh, nullptr, basicShader));
	kitten->SetPhysicsObject(new PhysicsObject(&kitten->GetTransform(), kitten->GetBoundingVolume()));

	kitten->GetPhysicsObject()->SetInverseMass(1.0f);
	kitten->GetPhysicsObject()->InitSphereInertia();
	kitten->GetRenderObject()->SetColour(Vector4(1.0f, 1.0f, 0.0f, 1.0f));

	world->AddGameObject(kitten);
	kittens.push_back(kitten); // Add to the kittens container

	return kitten;
}

/*

A single function to add a large immoveable cube to the bottom of our world

*/
GameObject* TutorialGame::AddFloorToWorld(const Vector3& position) {
	GameObject* floor = new GameObject("Floor");

	Vector3 floorSize = Vector3(200, 2, 200);
	AABBVolume* volume = new AABBVolume(floorSize);
	floor->SetBoundingVolume((CollisionVolume*)volume);
	floor->GetTransform()
		.SetScale(floorSize * 2.0f)
		.SetPosition(position);

	floor->SetRenderObject(new RenderObject(&floor->GetTransform(), cubeMesh, basicTex, basicShader));
	floor->SetPhysicsObject(new PhysicsObject(&floor->GetTransform(), floor->GetBoundingVolume()));

	floor->GetPhysicsObject()->SetInverseMass(0);
	floor->GetPhysicsObject()->InitCubeInertia();

	world->AddGameObject(floor);

	return floor;
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/
GameObject* TutorialGame::AddSphereToWorld(const Vector3& position, float radius, float inverseMass) {
	GameObject* sphere = new GameObject();

	Vector3 sphereSize = Vector3(radius, radius, radius);
	SphereVolume* volume = new SphereVolume(radius);
	sphere->SetBoundingVolume((CollisionVolume*)volume);

	sphere->GetTransform()
		.SetScale(sphereSize)
		.SetPosition(position);

	sphere->SetRenderObject(new RenderObject(&sphere->GetTransform(), sphereMesh, basicTex, basicShader));
	sphere->SetPhysicsObject(new PhysicsObject(&sphere->GetTransform(), sphere->GetBoundingVolume()));

	sphere->GetPhysicsObject()->SetInverseMass(inverseMass);
	sphere->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(sphere);

	return sphere;
}

GameObject* TutorialGame::AddCubeToWorld(const Vector3& position, Vector3 dimensions, float inverseMass, Vector4 color) {
	GameObject* cube = new GameObject();	
	AABBVolume* volume = new AABBVolume(dimensions);
	cube->SetBoundingVolume((CollisionVolume*)volume);

	cube->GetTransform()
		.SetPosition(position)
		.SetScale(dimensions * 2.0f);

	cube->SetRenderObject(new RenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));
	cube->SetPhysicsObject(new PhysicsObject(&cube->GetTransform(), cube->GetBoundingVolume()));

	cube->GetPhysicsObject()->SetInverseMass(inverseMass);
	cube->GetPhysicsObject()->InitCubeInertia();
	cube->GetRenderObject()->SetColour(color);

	world->AddGameObject(cube);

	return cube;
}

GameObject* TutorialGame::AddPlayerToWorld(const Vector3& position) {
	float meshSize		= 1.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject("MAMA_CAT");
	SphereVolume* volume  = new SphereVolume(0.4f);

	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), catMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}


GameObject* TutorialGame::AddKitttenToWorld(const Vector3& position) {
	float meshSize = 0.5f;
	float inverseMass = 0.2f;

	GameObject* kitten = new GameObject("Kitten");
	SphereVolume* volume = new SphereVolume(0.2f);

	kitten->SetBoundingVolume((CollisionVolume*)volume);

	kitten->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	kitten->SetRenderObject(new RenderObject(&kitten->GetTransform(), catMesh, nullptr, basicShader));
	kitten->SetPhysicsObject(new PhysicsObject(&kitten->GetTransform(), kitten->GetBoundingVolume()));

	kitten->GetRenderObject()->SetColour(Vector4(0.0f, 0.0f, 1.0f, 1.0f));

	kitten->GetPhysicsObject()->SetInverseMass(inverseMass);
	kitten->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(kitten);

	return kitten;
}

Goose* TutorialGame::AddGooseToWorld(const Vector3& position, GameObject* player) {
	float meshSize = 0.30f;
	float inverseMass = 0.5f;

	std::vector<Vector3> patrolNodes = { Vector3(10, 0, 10), Vector3(15, 0, 10), Vector3(19, 0, 10)};

	Goose* goose = new Goose(player, patrolNodes, world);
	SphereVolume* volume = new SphereVolume(0.5f);

	goose->SetBoundingVolume((CollisionVolume*)volume);

	goose->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	goose->SetRenderObject(new RenderObject(&goose->GetTransform(), gooseMesh, nullptr, basicShader));
	goose->SetPhysicsObject(new PhysicsObject(&goose->GetTransform(), goose->GetBoundingVolume()));
	
	goose->GetRenderObject()->SetColour(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	goose->GetPhysicsObject()->SetInverseMass(inverseMass);
	goose->GetPhysicsObject()->InitSphereInertia();
	goose->SetName("Goose");

	world->AddGameObject(goose);

	return goose;
}


GameObject* TutorialGame::AddEnemyToWorld(const Vector3& position) {
	float meshSize		= 3.0f;
	float inverseMass	= 0.5f;

	GameObject* character = new GameObject();

	AABBVolume* volume = new AABBVolume(Vector3(0.3f, 0.9f, 0.3f) * meshSize);
	character->SetBoundingVolume((CollisionVolume*)volume);

	character->GetTransform()
		.SetScale(Vector3(meshSize, meshSize, meshSize))
		.SetPosition(position);

	character->SetRenderObject(new RenderObject(&character->GetTransform(), enemyMesh, nullptr, basicShader));
	character->SetPhysicsObject(new PhysicsObject(&character->GetTransform(), character->GetBoundingVolume()));

	character->GetPhysicsObject()->SetInverseMass(inverseMass);
	character->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(character);

	return character;
}

GameObject* TutorialGame::AddBonusToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	SphereVolume* volume = new SphereVolume(0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(2, 2, 2))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), bonusMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();

	world->AddGameObject(apple);

	return apple;
}

GameObject* TutorialGame::AddcylinderToWorld(const Vector3& position) {
	GameObject* apple = new GameObject();

	CapsuleVolume* volume = new CapsuleVolume(0.5f,0.5f);
	apple->SetBoundingVolume((CollisionVolume*)volume);
	apple->GetTransform()
		.SetScale(Vector3(1, 1, 1))
		.SetPosition(position);

	apple->SetRenderObject(new RenderObject(&apple->GetTransform(), capsuleMesh, nullptr, basicShader));
	apple->SetPhysicsObject(new PhysicsObject(&apple->GetTransform(), apple->GetBoundingVolume()));

	apple->GetPhysicsObject()->SetInverseMass(1.0f);
	apple->GetPhysicsObject()->InitSphereInertia();
	apple->GetRenderObject()->SetColour(Vector4(1.0f, 0.0f, 0.0f, 1.0f));
	world->AddGameObject(apple);

	return apple;
}





void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitGameExamples() {
	AddPlayerToWorld(Vector3(0, 5, 0));
	AddEnemyToWorld(Vector3(5, 5, 0));
	AddBonusToWorld(Vector3(10, 5, 0));
}

void TutorialGame::InitSphereGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, float radius) {
	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddSphereToWorld(position, radius, 1.0f);
		}
	}
	AddFloorToWorld(Vector3(0, -2, 0));
}

void TutorialGame::InitMixedGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing) {
	float sphereRadius = 1.0f;
	Vector3 cubeDims = Vector3(1, 1, 1);

	for (int x = 0; x < numCols; ++x) {
		for (int z = 0; z < numRows; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);

			if (rand() % 2) {
				AddCubeToWorld(position, cubeDims);
			}
			else {
				AddSphereToWorld(position, sphereRadius);
			}
		}
	}
}

void TutorialGame::InitCubeGridWorld(int numRows, int numCols, float rowSpacing, float colSpacing, const Vector3& cubeDims) {
	for (int x = 1; x < numCols+1; ++x) {
		for (int z = 1; z < numRows+1; ++z) {
			Vector3 position = Vector3(x * colSpacing, 10.0f, z * rowSpacing);
			AddCubeToWorld(position, cubeDims, 1.0f);
		}
	}
}

void TutorialGame::InitMazeWorld(const std::vector<std::vector<int>>& mazePattern, const Vector3& cubeDims, float spacing) {
	// Loop through the 2D maze pattern
	for (size_t x = 0; x < mazePattern.size(); ++x) {
		for (size_t z = 0; z < mazePattern[x].size(); ++z) {
			if (mazePattern[x][z] == 1) { // If the grid cell is 1, add a cube
				Vector3 position = Vector3(x * spacing, cubeDims.y, z * spacing);
				AddCubeToWorld(position, cubeDims, 0.0f); // 0.0f makes the cubes immovable (walls)
			}
		}
	}
}

/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/
bool TutorialGame::SelectObject() {

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::Q)) {
		inSelectionMode = !inSelectionMode;
		if (inSelectionMode) {
			Window::GetWindow()->ShowOSPointer(true);
			Window::GetWindow()->LockMouseToWindow(false);
		}
		else {
			Window::GetWindow()->ShowOSPointer(false);
			Window::GetWindow()->LockMouseToWindow(true);
		}
	}
	if (inSelectionMode) {
		Debug::Print("Press Q to change to camera mode!", Vector2(5, 85));

		if (Window::GetMouse()->ButtonDown(NCL::MouseButtons::Left)) {
			if (selectionObject) {	//set colour to deselected;
				selectionObject->GetRenderObject()->SetColour(Vector4(1, 1, 1, 1));
				selectionObject = nullptr;
			}

			Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

			RayCollision closestCollision;
			if (world->Raycast(ray, closestCollision, true)) {
				selectionObject = (GameObject*)closestCollision.node;

				selectionObject->GetRenderObject()->SetColour(Vector4(0, 1, 0, 1));
				return true;
			}
			else {
				return false;
			}
		}
		if (Window::GetKeyboard()->KeyPressed(NCL::KeyCodes::L)) {
			if (selectionObject) {
				if (lockedObject == selectionObject) {
					lockedObject = nullptr;
				}
				else {
					lockedObject = selectionObject;
				}
			}
		}
	}
	else {
		Debug::Print("Press Q to change to select mode!", Vector2(5, 85));
	}
	return false;
}

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

void TutorialGame::MoveSelectedObject() {
	
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 100.0f;

	if (!selectionObject) {
		return;//we haven't selected anything!
	}
	//Push the selected object!
	if (Window::GetMouse()->ButtonPressed(NCL::MouseButtons::Right)) {
		Ray ray = CollisionDetection::BuildRayFromMouse(world->GetMainCamera());

		RayCollision closestCollision;
		if (world->Raycast(ray, closestCollision, true)) {
			if (closestCollision.node == selectionObject) {
				selectionObject->GetPhysicsObject()->AddForceAtPosition(ray.GetDirection() * forceMagnitude, closestCollision.collidedAt);
			}
		}
	}
}


void Goose::SetupStateMachine() {
	stateMachine = new StateMachine();

	// Patrol State
	State* patrolState = new State([&](float dt) { Patrol(dt); });

	// Chase State
	State* chaseState = new State([&](float dt) { ChasePlayer(dt); });

	stateMachine->AddState(patrolState);
	stateMachine->AddState(chaseState);

	// Transition from Patrol to Chase when player is visible
	stateMachine->AddTransition(new StateTransition(patrolState, chaseState, [&]() -> bool {
		return isPlayerVisible;
		}));

	// Transition from Chase to Patrol when player is not visible
	stateMachine->AddTransition(new StateTransition(chaseState, patrolState, [&]() -> bool {
		return !isPlayerVisible;
		}));
}

void Goose::Patrol(float dt) {
	if (patrolNodes.empty()) return;

	Vector3 currentPos = GetTransform().GetPosition();
	Vector3 targetNode = patrolNodes[currentNodeIndex];
	Vector3 direction = targetNode - currentPos;
	if (Vector::Length(direction) > 1.0f) {
		direction = Vector::Normalise(direction);
		GetPhysicsObject()->AddForce(direction * 10.0f);
	}
	else {
		currentNodeIndex = (currentNodeIndex + 1) % patrolNodes.size(); // Move to next node
	}

	// Check for player's visibility
	Raycast();
}

void Goose::ChasePlayer(float dt) {
	Vector3 goosePos = GetTransform().GetPosition();
	Vector3 playerPos = player->GetTransform().GetPosition();
	Vector3 direction = playerPos - goosePos;

	if (Vector::Length(direction) > 1.0f) {
		direction = Vector::Normalise(direction);
		GetPhysicsObject()->AddForce(direction * 10.0f);
	}

	// Check if player is still visible
	Raycast();
}

void Goose::Raycast() {
	Vector3 goosePos = GetTransform().GetPosition();
	Vector3 playerPos = player->GetTransform().GetPosition();
	Vector3 direction = playerPos - goosePos;

	if (Vector::Length(direction) > 0.01f) {
		direction = Vector::Normalise(direction);
		Ray ray(goosePos, direction);

		RayCollision collision;
		isPlayerVisible = world->Raycast(ray, collision, true, this);

		if (isPlayerVisible && collision.node == player) {
			std::cout << "Player in sight!\n";
		}
		else {
			isPlayerVisible = false;
		}
	}
}

void Goose::VisualizeRay() {
	Vector3 goosePos = GetTransform().GetPosition();
	Vector3 playerPos = player->GetTransform().GetPosition();
	Vector3 direction = playerPos - goosePos;

	if (Vector::Length(direction) > 0.01f) {
		direction = Vector::Normalise(direction);
		Debug::DrawLine(goosePos, goosePos + direction * 50.0f, Vector4(1, 0, 0, 1)); // Draw ray in red
	}
}

