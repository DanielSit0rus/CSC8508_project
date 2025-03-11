#include "GameManager.h"
#include "AudioSystem.h"
#include "SLSystem.h"
#include "EventManager.h"

using namespace NCL;
using namespace CSC8503;

void GameManager::Init(PaintballGameWorld* world,float gameTime)
{
    this->world = world;
    RpWorld = physicsCommon.createPhysicsWorld(RpSettings);
    if (RpWorld == nullptr) {
        std::cerr << "Error: Failed to create Physics World!" << std::endl;
        throw std::runtime_error("Physics World creation failed!");
    }

    totalTime = gameTime;
    leftTime = totalTime;

    EventManager::Subscribe(EventType::Game_Start, [this]() {leftTime = totalTime; canStart_FMOD = true; });

    RpWorld->setEventListener(&bulletlistener);
}

void GameManager::Update(float dt) {
    leftTime -= dt;

    Debug::Print("Left time: "
        + std::to_string(GameManager::GetInstance().GetLeftTime()), Vector2(5, 15));

    if (hasPhys) RpWorld->update(dt);

    //FMOD
    if (canStart_FMOD && leftTime < 118.5f && leftTime > 115) {
        AudioSystem::GetInstance().TriggerEvent("event:/Felicia/Start2");
        canStart_FMOD = false;
    }
}

void GameManager::PostCleanUp() // after (20Hz) server/client update
{
    for (auto object : objectsToDelete) {

        if (object->GetNetworkObject()) {
            networkObjects[object->GetNetworkObject()->GetNetworkID()] = nullptr;
        }

        //std::cout << "name = [" << object->GetName()<<"]" << std::endl;
        world->RemoveGameObject(object, true);
    }
    objectsToDelete.clear();
}

void GameManager::InitWorld(int arg)
{
    GameManager::GetInstance().GetWorld()->ClearAndErase();

    //Pointers - to be private later
    playerObject = nullptr;
    selectionObject = nullptr;
    lockedObject = nullptr;
    enemyObject = nullptr;
    CharacterObject = nullptr;
    pathNodes.clear();
    //FMOD
    speakerObj = nullptr;
    //shoot 
    shoottest = nullptr;
    enemy = nullptr;
    //private
    gameObjects.clear();
    player = nullptr;
    enemies.clear();
    objectsToDelete.clear();


    json j = SLSystem::GetInstance().LoadData("save");

    if (j.contains("objs") && j["objs"].is_array()) {
        for (const auto& obj : j["objs"]) {
            if (obj.contains("type")) {
                AddObject(obj["type"],
                    rp3d::Vector3(obj["pos"][0], obj["pos"][1], obj["pos"][2]),
                    rp3d::Vector3(obj["scale"][0], obj["scale"][1], obj["scale"][2]),
                    rp3d::Quaternion(obj["ori"][0], obj["ori"][1], obj["ori"][2], obj["ori"][3]),
                    ResourceManager::GetInstance().GetMapMesh(),
                    Vector4(obj["color"][0], obj["color"][1], obj["color"][2], obj["color"][3]),
                    obj["mass"]);
            }
        }
    }
}

PaintballGameObject* GameManager::AddObject(GameObjectType type, const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, Mesh* mesh, Vector4 color, float mass)
{
    switch (type)
    {
    case GameObjectType::cube:
        //std::cout << "[GameManager::AddObject] cube." << std::endl;
        return AddCube(position, dimensions * 0.5f, orientation, mass, color);
        break;

    case GameObjectType::sphere:
        //std::cout << "[GameManager::AddObject] cube." << std::endl;
        return AddSphere(position, dimensions, orientation, mass, color);
        break;

    case GameObjectType::concave1:
        //std::cout << "[GameManager::AddObject] cube." << std::endl;
        return AddConcaveMesh(position, dimensions, orientation, mesh, color);
        break;

    default:
        std::cout << "[GameManager::AddObject] Unknown type." << std::endl;
        return nullptr;
        break;
    }
}

PaintballGameObject* GameManager::AddCube(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color) {
    ResourceManager& resources = ResourceManager::GetInstance();

    PaintballGameObject* cube = new PaintballGameObject(GameObjectType::cube);

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 2.0f)
        .SetRatioR(dimensions * 2.0f);

    cube->SetRenderObject(new PaintballRenderObject(&cube->GetTransform(), resources.GetCubeMesh(), resources.GetBasicTex(), resources.GetBasicShader()));
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


PaintballGameObject* GameManager::AddSphere(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color) {

    ResourceManager& resources = ResourceManager::GetInstance();
    PaintballGameObject* cube = new PaintballGameObject(GameObjectType::sphere);

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 1.0f)
        .SetRatioR(dimensions * 1.0f);

    cube->SetRenderObject(new PaintballRenderObject(&cube->GetTransform(), resources.GetSphereMesh(), resources.GetBasicTex(), resources.GetBasicShader()));

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

PaintballGameObject* GameManager::AddFloorToWorld(const rp3d::Vector3& position) {
    return AddCube(position, rp3d::Vector3(200, 2, 200), rp3d::Quaternion(0, 0, 0, 1.0f), 0, Vector4(1, 1, 1, 1));
}

PaintballGameObject* GameManager::AddPlayer(const rp3d::Vector3& position) {

    PaintballGameObject* p = AddCube(position, rp3d::Vector3(0.3f, 1, 0.3f), rp3d::Quaternion(0, 0, 0, 1.0f));
    p->GetPhysicsObject()->GetRigidbody().setAngularLockAxisFactor(rp3d::Vector3(0, 1, 0));
    return p;
}



PaintballPlayer* GameManager::AddPlayerClass(rp3d::Vector3 position) {
    rp3d::Vector3 dimensions = rp3d::Vector3(1.0f, 2.f, 1.0f);
    rp3d::Vector3 ratioRender = rp3d::Vector3(2.1f, 2.1f, 2.1f);
    ResourceManager& resources = ResourceManager::GetInstance();

    // Create the player object
    player = new PaintballPlayer();

    player->GetTransform()
        .SetPosition(position)
        .SetScale(dimensions * 1.0f)
        .SetRatioR(ratioRender)
        .SetOffsetR(rp3d::Vector3(0, -dimensions.y * 0.5f * ratioRender.y, 0));

    // Create render object with animation support
    PaintballRenderObject* renderObj = new PaintballRenderObject(
        &player->GetTransform(),
        resources.GetRoleMesh(),
        resources.GetBasicTex(),
        resources.GetBasicShader(),
        resources.GetIdleanim(),
        resources.GetRolemat()
    );

    renderObj->SetColour(Vector4(0, 0, 1, 1)); // Set player color (blue for example)

    // Attach the animated render object to the player
    player->SetRenderObject(renderObj);

    // Create a rigid body for physics
    // Create rigid body at the player's position
    rp3d::RigidBody* cubeBody = RpWorld->createRigidBody(player->GetTransform().GetRpTransform());

    // Create collision shape
    rp3d::CapsuleShape* shape = physicsCommon.createCapsuleShape(dimensions.x, dimensions.y);

    // Offset the collider UP by half its height
    rp3d::Transform shapeTransform = rp3d::Transform::identity();

    // Bind the shape to the rigid body with an offset
    rp3d::Collider* collider = cubeBody->addCollider(shape, shapeTransform);

    // Attach physics object to player
    player->SetPhysicsObject(new PaintballPhysicsObject(&player->GetTransform(), *cubeBody, *RpWorld));
    player->GetPhysicsObject()->SetMass(1);
    player->GetPhysicsObject()->GetRigidbody().setAngularLockAxisFactor(rp3d::Vector3(0, 1, 0));

    // Set collision category
    collider->setCollisionCategoryBits(PLAYER);

    // Attach camera
    player->SetCamera(&world->GetMainCamera());

    // Add player to the game world
    world->AddGameObject(player);

    return player;
}


PaintballGameObject* GameManager::Addcharacter(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color) {
    ResourceManager& resources = ResourceManager::GetInstance();

    PaintballGameObject* cube = new PaintballGameObject(GameObjectType::test_character);

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 1.0f)
        .SetRatioR(dimensions * 1.0f);
    // Create render object
    PaintballRenderObject* renderObj = new PaintballRenderObject(
        &cube->GetTransform(),
        resources.GetRoleMesh(),
        resources.GetBasicTex(),
        resources.GetBasicShader(),
        resources.GetIdleanim(),
        resources.GetRolemat()
    );

    renderObj->SetColour(color);

    // Attach animation (assuming GetCubeAnimation exists in ResourceManager)

    cube->SetRenderObject(renderObj);

    // Create a rigid body
    rp3d::RigidBody* cubeBody = RpWorld->createRigidBody(cube->GetTransform().GetRpTransform());

    // Create Shape
    rp3d::BoxShape* shape = physicsCommon.createBoxShape(dimensions);

    // Bind Shape to rigid body
    rp3d::Transform shapeTransform = rp3d::Transform::identity();
    rp3d::Collider* collider = cubeBody->addCollider(shape, shapeTransform);

    // Add rigid body to game object
    cube->SetPhysicsObject(new PaintballPhysicsObject(&cube->GetTransform(), *cubeBody, *RpWorld));
    cube->GetPhysicsObject()->SetMass(mass);

    // Add to the game world
    world->AddGameObject(cube);

    return cube;

}


PaintballGameObject* GameManager::AddConcaveMesh(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, Mesh* mesh, Vector4 color)
{

    ResourceManager& resources = ResourceManager::GetInstance();
    PaintballGameObject* concave = new PaintballGameObject(GameObjectType::concave1);

    concave->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 1.0f)
        .SetRatioR(dimensions * 1.0f);

    concave->SetRenderObject(new PaintballRenderObject(&concave->GetTransform(),
        mesh, resources.GetBasicTex(), resources.GetBasicShader()));

    concave->GetRenderObject()->SetColour(color);

    // create a rigid body
    rp3d::RigidBody* concaveBody = RpWorld->createRigidBody(concave->GetTransform().GetRpTransform());
    concaveBody->setType(rp3d::BodyType::STATIC);
    // create Shape
    rp3d::ConcaveMeshShape* shape = CreateConcaveMeshShape(mesh);
    shape->setScale(dimensions);
    // bind Shape to rigid body
    rp3d::Transform shapeTransform = rp3d::Transform::identity();

    rp3d::Collider* collider = concaveBody->addCollider(shape, shapeTransform);
    //add rigid body to gameobject
    concave->SetPhysicsObject(new PaintballPhysicsObject(&concave->GetTransform(), *concaveBody, *RpWorld));

    world->AddGameObject(concave);
    return concave;
}

PaintballBullet* NCL::CSC8503::GameManager::AddBullet(rp3d::Vector3 ori3, bool isenemy, const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, Vector4 color, float mass)
{
    //AudioSystem::GetInstance().TriggerEvent("event:/Effect/GunShoot", position);

    ResourceManager& resources = ResourceManager::GetInstance();
    PaintballBullet* cube = new PaintballBullet();

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 1.0f)
        .SetRatioR(dimensions * 1.0f);

    cube->SetRenderObject(new PaintballRenderObject(&cube->GetTransform(), resources.GetSphereMesh(), resources.GetBasicTex(), resources.GetBasicShader()));

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

    //      ?   ?       ?           ?  
    collider->setCollisionCategoryBits(BULLET);
    collider->setIsTrigger(true);

    cube->InitBullet(ori3, isenemy);

    world->AddGameObject(cube);

    return cube;
}

reactphysics3d::ConcaveMeshShape* GameManager::CreateConcaveMeshShape(Mesh* mesh) {

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

PaintballGameObject* NCL::CSC8503::GameManager::AddTriggerCube()
{
    PaintballGameObject* trip1 =  AddCube(rp3d::Vector3(2, 35, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    PaintballGameObject* trip2 = AddCube(rp3d::Vector3(2, 35, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    // Anchor point in world-space
    const rp3d::Vector3 anchorPoint(2.0, 4.0, 0.0);

  
    // Create the joint info object
    reactphysics3d::BallAndSocketJointInfo jointInfo(&trip1->GetPhysicsObject()->GetRigidbody(), &trip2->GetPhysicsObject()->GetRigidbody(), anchorPoint);
    return nullptr;
}

Vector3 GameManager::GetCameraFront()
{
    Vector3 camPos = world->GetMainCamera().GetPosition();
    float yaw = DegreesToRadians(world->GetMainCamera().GetYaw());
    float pitch = DegreesToRadians(-world->GetMainCamera().GetPitch());
    Vector3 front(cos(pitch) * sin(yaw), sin(pitch), cos(pitch) * cos(yaw));
    return     front = -Vector::Normalise(front);
}

