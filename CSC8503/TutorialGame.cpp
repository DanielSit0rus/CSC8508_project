#include "TutorialGame.h"
#include "GameWorld.h"
#include "TextureLoader.h"
#include "Vector.h"


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
	
	//renderer->AddLight(light1);
#endif

	RpWorld = physicsCommon.createPhysicsWorld(RpSettings);		//rp3d

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
	mapMesh = renderer->LoadMesh("SampleMap.msh");
	gooseMesh = renderer->LoadMesh("goose.msh");

	enemyMesh	= renderer->LoadMesh("Keeper.msh");
	bonusMesh	= renderer->LoadMesh("19463_Kitten_Head_v1.msh");
	capsuleMesh = renderer->LoadMesh("capsule.msh");

	//mapMesh = renderer->LoadMesh("SampleMap.msh");

	basicTex	= renderer->LoadTexture("checkerboard.png");
	basicShader = renderer->LoadShader("scene.vert", "scene.frag");

	InitWorld();
	InitCamera();
}

TutorialGame::~TutorialGame()	{
	delete cubeMesh;
	delete sphereMesh;
	delete catMesh;
	delete kittenMesh;
	delete enemyMesh;
	delete bonusMesh;
	delete mapMesh;

	delete basicTex;
	delete basicShader;

	delete renderer;
	delete world;
	physicsCommon.destroyPhysicsWorld(RpWorld);
}

bool TutorialGame::pauseGame(){
	return pause = true;

}
bool TutorialGame::UnpauseGame() {
	return pause = false;
}

void TutorialGame::UpdateGame(float dt) {
	const Camera& camera = world->GetMainCamera();

	const Vector3& pos = camera.GetPosition();
	std::string posString = std::to_string((int)pos.x) + ", "
		+ std::to_string((int)pos.y) + ", " + std::to_string((int)pos.z);
	Debug::Print("Pos = " + posString, Vector2(60, 95), Debug::BLUE);

	if (pause) {
		// Only update the PushMachine and render the current frame.
		renderer->Render();
		return; // Skip the rest of the game updates.
	}


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
	if (lockedObject) {
		Vector3 lockedScale = Util::RP3dToNCL(lockedObject->GetTransform().GetScale());

		world->GetMainCamera().UpdateCameraView3(
			Util::RP3dToNCL(lockedObject->GetTransform().GetPosition()),
			rp3d::max3(lockedScale.x, lockedScale.y, lockedScale.z) * 1.1f + 5.0f);

		LockedObjectMovement();
	}
	else
	{
		world->GetMainCamera().UpdateCamera(dt, forceMagnitude * 0.5f);
	}

	UpdateKeys();

	Debug::DrawLine(Vector3(), Vector3(0, 100, 0), Vector4(1, 0, 0, 1));

	Debug::Print("Force/Speed:" + std::to_string((int)forceMagnitude), Vector2(5, 80));
	forceMagnitude += Window::GetMouse()->GetWheelMovement() * 25.0f;

	world->UpdateWorld(dt);
	renderer->Update(dt);

	RpWorld->update(dt);	//rp3d

	renderer->Render();
	Debug::UpdateRenderables(dt);

	//Fmod
	Matrix4 view = camera.BuildViewMatrix();;
	Vector3 forward = Vector::Normalise(Vector3(view.array[0][2], view.array[1][2], view.array[2][2]));
	Vector3 up = Vector::Normalise(Vector3(view.array[0][1], view.array[1][1], view.array[2][1]));
	listenerAttributes = new FMOD_3D_ATTRIBUTES();
	listenerAttributes->position = { pos.x, pos.y, pos.z };
	listenerAttributes->forward = { forward.x,forward.y,forward.z };
	listenerAttributes->up = { up.x,up.y,up.z };
	AudioSystem::GetInstance().studioSystem->setListenerAttributes(0, listenerAttributes);
	rp3d::Vector3 pos2 = speakerObj->GetTransform().GetPosition();
	AudioSystem::GetInstance().sourceAttributes->position = { pos2.x, pos2.y, pos2.z };
	AudioSystem::GetInstance().eventInstance->set3DAttributes(AudioSystem::GetInstance().sourceAttributes);
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
	lockedObject = nullptr;

	world->ClearAndErase();

	//BridgeConstraintTest();
	//InitMixedGridWorld(15, 15, 3.5f, 3.5f);
	//testStateObject = AddStateObjectToWorld(Vector3(0, 10, -10));
	//InitGameExamples();

	playerObject = AddPlayerToWorld(rp3d::Vector3(2, 2, 2));

	forceMagnitude = 60.0f;


	Light light2(Vector3(14, 4, 7), Vector3(0, -1, 0), Vector4(0, 1, 0, 1), 1.0f, 45.0f);
	renderer->AddLight(light2);


	speakerObj = AddRp3dObjToWorld(rp3d::Vector3(0, 25, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));

	//rp3d
	objList_pb.clear();
	float angleInRadians = 10.0f * PI / 180.0f;
	rp3d::Quaternion rotation = rp3d::Quaternion::fromEulerAngles(angleInRadians, 0.0f, angleInRadians);
	objList_pb.push_back(AddRp3dCubeToWorld(rp3d::Vector3(0, 15, -30), rp3d::Vector3(10, 1, 10), rp3d::Quaternion(0, 0, 0, 1.0f), 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
	objList_pb.push_back(AddRp3dCubeToWorld(rp3d::Vector3(1, 20, -30), rp3d::Vector3(5, 1, 5), rotation, 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
	objList_pb.push_back(AddRp3dCubeToWorld(rp3d::Vector3(2, 25, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));


	objList_pb.push_back(AddRp3dCubeToWorld(rp3d::Vector3(34, 32, -11), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 1, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));
	objList_pb.push_back(AddRp3dCubeToWorld(rp3d::Vector3(32, 20, -7), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 1, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));

	objList_pb.push_back(AddRp3dConcaveToWorld(rp3d::Vector3(20, 10, -10), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f)));

	InitDefaultFloor();
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
		RpWorld->raycast(ray, &callback);
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
		RpWorld->raycast(ray, &callback);
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

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F1)) {
		InitWorld(); //We can reset the simulation at any time with F1
	}

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::F2)) {
		InitCamera(); //F2 will reset the camera to a specific default place
	}

	//Running certain physics updates in a consistent order might cause some
	//bias in the calculations - the same objects might keep 'winning' the constraint
	//allowing the other one to stretch too much etc. Shuffling the order so that it
	//is random every frame can help reduce such bias.

	/*
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
	*/
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
	if (target->GetName() == "player" || target->GetName() == "kitten") camYaw += 180.0f;

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
PaintballGameObject* TutorialGame::AddFloorToWorld(const rp3d::Vector3& position) {	
	return AddRp3dCubeToWorld(position, rp3d::Vector3(200, 2, 200), rp3d::Quaternion(0, 0, 0, 1.0f),0);
}

/*

Builds a game object that uses a sphere mesh for its graphics, and a bounding sphere for its
rigid body representation. This and the cube function will let you build a lot of 'simple' 
physics worlds. You'll probably need another function for the creation of OBB cubes too.

*/

PaintballGameObject* TutorialGame::AddRp3dCubeToWorld(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color) {
	PaintballGameObject* cube = new PaintballGameObject();

	cube->GetTransform()
		.SetPosition(position)
		.SetOrientation(orientation)
		.SetScale(dimensions * 2.0f);

	cube->SetRenderObject(new PaintballRenderObject(&cube->GetTransform(), cubeMesh, basicTex, basicShader));

	cube->GetRenderObject()->SetColour(color);

	// create a rigid body
	rp3d::RigidBody* cubeBody = RpWorld->createRigidBody(cube->GetTransform().GetRpTransform());
	// create Shape
	rp3d::BoxShape* shape = physicsCommon.createBoxShape(dimensions);
	//rp3d::SphereShape* shape = physicsCommon.createSphereShape(halfExtents.x);
	// bind Shape to rigid body
	rp3d::Transform shapeTransform = rp3d::Transform::identity();
	rp3d::Collider* collider = cubeBody->addCollider(shape, shapeTransform);
	//add rigid body to gameobject
	cube->SetPhysicsObject(new PaintballPhysicsObject(&cube->GetTransform(), *cubeBody, *RpWorld));
	cube->GetPhysicsObject()->SetMass(mass);

	world->AddGameObject(cube);

	return cube;
}
PaintballGameObject* TutorialGame::AddRp3dObjToWorld(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color) {
	PaintballGameObject* cube = new PaintballGameObject();

	cube->GetTransform()
		.SetPosition(position)
		.SetOrientation(orientation)
		.SetScale(dimensions * 1.0f);

	cube->SetRenderObject(new PaintballRenderObject(&cube->GetTransform(), sphereMesh, basicTex, basicShader));

	cube->GetRenderObject()->SetColour(color);

	// create a rigid body
	rp3d::RigidBody* cubeBody = RpWorld->createRigidBody(cube->GetTransform().GetRpTransform());
	// create Shape
	//rp3d::BoxShape* shape = physicsCommon.createBoxShape(dimensions);
	rp3d::SphereShape* shape = physicsCommon.createSphereShape(dimensions.x);
	// bind Shape to rigid body
	rp3d::Transform shapeTransform = rp3d::Transform::identity();
	rp3d::Collider* collider = cubeBody->addCollider(shape, shapeTransform);
	cubeBody->setAngularDamping(0.1f);
	//add rigid body to gameobject
	cube->SetPhysicsObject(new PaintballPhysicsObject(&cube->GetTransform(), *cubeBody, *RpWorld));
	cube->GetPhysicsObject()->SetMass(mass);

	world->AddGameObject(cube);

	return cube;
}

PaintballGameObject* TutorialGame::AddPlayerToWorld(const rp3d::Vector3& position) {
	PaintballGameObject* p =
		AddRp3dCubeToWorld(position, rp3d::Vector3(0.3f, 1, 0.3f), rp3d::Quaternion(0, 0, 0, 1.0f));
	p->GetPhysicsObject()->GetRigidbody().setAngularLockAxisFactor(rp3d::Vector3(0, 1, 0));
	return p;
}

void TutorialGame::InitDefaultFloor() {
	AddFloorToWorld(rp3d::Vector3(0, -2, 0));
}


/*
Every frame, this code will let you perform a raycast, to see if there's an object
underneath the cursor, and if so 'select it' into a pointer, so that it can be 
manipulated later. Pressing Q will let you toggle between this behaviour and instead
letting you move the camera around. 

*/

/*
If an object has been clicked, it can be pushed with the right mouse button, by an amount
determined by the scroll wheel. In the first tutorial this won't do anything, as we haven't
added linear motion into our physics system. After the second tutorial, objects will move in a straight
line - after the third, they'll be able to twist under torque aswell.
*/

reactphysics3d::ConcaveMeshShape* TutorialGame::CreateConcaveMeshShape(Mesh* mesh) {
	const void* vertStart = mesh->GetPositionData().data();
	const void* indexStart = mesh->GetIndexData().data();

	unsigned int vertCount = mesh->GetVertexCount();
	unsigned int trianglesCount = mesh->GetIndexCount() / 3;

	// Create the TriangleVertexArray
	reactphysics3d::TriangleVertexArray* triangleArray = new reactphysics3d::TriangleVertexArray(
		vertCount,                       // Number of vertices
		vertStart,                       // Vertex position data
		sizeof(Maths::Vector3),          // Stride between vertices
		trianglesCount,                  // Number of triangles
		indexStart,                      // Index data
		3 * sizeof(int),                 // Stride between indices
		reactphysics3d::TriangleVertexArray::VertexDataType::VERTEX_FLOAT_TYPE,
		reactphysics3d::TriangleVertexArray::IndexDataType::INDEX_INTEGER_TYPE
	);

	// Vector to store messages from triangle mesh creation
	std::vector<reactphysics3d::Message> messages;


	// Create the TriangleMesh
	reactphysics3d::TriangleMesh* triangleMesh = physicsCommon.createTriangleMesh(*triangleArray, messages);
	const Vector3 scaling(1, 1, 1);

	// Create the ConcaveMeshShape using the TriangleMesh
	reactphysics3d::ConcaveMeshShape* concaveMeshShape = physicsCommon.createConcaveMeshShape(triangleMesh);

	return concaveMeshShape;
}


PaintballGameObject* TutorialGame::AddRp3dConcaveToWorld(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float inverseMass, Vector4 color) {
	PaintballGameObject* concave = new PaintballGameObject();

	concave->GetTransform()
		.SetPosition(position)
		.SetOrientation(orientation)
		.SetScale(dimensions);

	concave->SetRenderObject(new PaintballRenderObject(&concave->GetTransform(), mapMesh, basicTex, basicShader));

	concave->GetRenderObject()->SetColour(color);

	rp3d::Vector3 pos(position.x, position.y, position.z);
	rp3d::Quaternion ori = rp3d::Quaternion(orientation.x, orientation.y, orientation.z, orientation.w);


	// create a rigid body
	rp3d::Transform transform(pos, ori);
	rp3d::RigidBody* concaveBody = RpWorld->createRigidBody(transform);
	concaveBody->setType(rp3d::BodyType::STATIC);
	// create Shape
	rp3d::ConcaveMeshShape* shape = CreateConcaveMeshShape(mapMesh); // scale?
	// bind Shape to rigid body
	rp3d::Transform shapeTransform = rp3d::Transform::identity();
	rp3d::Collider* collider = concaveBody->addCollider(shape, shapeTransform);
	//add rigid body to gameobject
	concave->SetPhysicsObject(new PaintballPhysicsObject(&concave->GetTransform(), *concaveBody, *RpWorld));

	world->AddGameObject(concave);



	return concave;
}


