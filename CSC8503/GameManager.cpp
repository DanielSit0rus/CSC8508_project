#include "GameManager.h"
#include "AudioSystem.h"
#include "SLSystem.h"
#include "EventManager.h"
#include "GameObjectFreeList.h"

using namespace NCL;
using namespace CSC8503;

void GameManager::Init(PaintballGameWorld* world, GameTechRenderer* renderer, float gameTime)
{
    this->world = world;
    this->renderer = renderer;

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
    //std::cout << "1";
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
            networkObjects.find(object->GetNetworkObject()->GetNetworkID())->second = nullptr;
        }

        //std::cout << "name = [" << object->GetName()<<"]" << std::endl;
        world->RemoveGameObject(object, true);
    }
    objectsToDelete.clear();

    if (toRebuild != -1) InitWorld(toRebuild);
}

void GameManager::InitWorld() {
    leftTime = totalTime;
    lockedObject = nullptr;
    selectionObject = nullptr;
    world->ClearAndErase();

    //ResourceManager::GetInstance().ReloadAnimations();

    playerObject = AddPlayerClass(rp3d::Vector3(1, 52, -21));

    enemyObject = AddPlayerClass(rp3d::Vector3(5, 1, -1));

    CharacterObject = AddPlayerClass(rp3d::Vector3(0, 8, -30));

    forceMagnitude = 60.0f;



    Light light2(Vector3(12, 10, -5), Vector3(0, -1, 0), Vector4(1.0f, 0.95f, 0.8f, 1.0f), 1.0f, 45.0f, LightType::Spot);
    renderer->AddLight(light2);

    //FMOD
    speakerObj = AddSphere(rp3d::Vector3(0, 25, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    speakerObj->SetAudioObject(new PaintballAudioObject(&speakerObj->GetTransform(),
        AudioSystem::GetInstance().GetEvent("event:/Congzheng/BGM2_3D")));
    //speakerObj->GetAudioObject()->Play(true);

    //rp3d
    float angleInRadians = 10.0f * PI / 180.0f;
    rp3d::Quaternion rotation = rp3d::Quaternion::fromEulerAngles(angleInRadians, 0.0f, angleInRadians);
    /*AddCube(rp3d::Vector3(0, 25, -30), rp3d::Vector3(10, 1, 10), rp3d::Quaternion(0, 0, 0, 1.0f), 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddCube(rp3d::Vector3(1, 30, -30), rp3d::Vector3(5, 1, 5), rotation, 0, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddCube(rp3d::Vector3(2, 35, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));


    AddCube(rp3d::Vector3(-10, 32, -11), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 1, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddCube(rp3d::Vector3(-10, 20, -7), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 1, Vector4(1.0f, 0.0f, 0.0f, 1.0f));*/


    //FIRST MAP
    AddConcaveMesh(rp3d::Vector3(-80, -4, -32), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "floor_1", "floor", "basic");
    AddConcaveMesh(rp3d::Vector3(-117, 1, -119), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "mainWall_1", "mainwall", "basic");
    AddConcaveMesh(rp3d::Vector3(-215, 1, -25), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "doorWay_1", "doorway", "basic");
    AddConcaveMesh(rp3d::Vector3(-5, 0, -55), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "oppDoorWay_1", "doorway", "basic");
    AddConcaveMesh(rp3d::Vector3(-121, 2, -55), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "midDoor", "mainwall", "basic");
    AddConcaveMesh(rp3d::Vector3(-62, 7, -78), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "wall_1", "wall123", "basic");
    AddConcaveMesh(rp3d::Vector3(-94, 2, -78), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "wall_2", "wall123", "basic");
    AddConcaveMesh(rp3d::Vector3(-150, 2, -78), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "openWall", "openwall", "basic");
    AddConcaveMesh(rp3d::Vector3(-153, 2, 88), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "wall_3", "wall123", "basic");
    AddConcaveMesh(rp3d::Vector3(-31, -2, -38), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "wall_4", "wall4", "basic");
    AddConcaveMesh(rp3d::Vector3(-41, 1, 53), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "cylinder", "wall4", "basic");
    AddConcaveMesh(rp3d::Vector3(-173, 2, -19), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "bigCylinder", "wall4", "basic");
    AddConcaveMesh(rp3d::Vector3(-194, 2, 123), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "target", "floor", "basic");
    AddConcaveMesh(rp3d::Vector3(-96, 1, 70), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "bigCube", "wall4", "basic");



    //SECOND MAP
    AddConcaveMesh(rp3d::Vector3(-50, 0, 169), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "floor_2", "basic", "basic", Vector4(1.0f, 1.0f, 1.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-200, 4, 109), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "mainWall_2", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-353, -4, -77), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "doorWay_2", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(60, 2, 290), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "oppDoorWay_2", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-492, -154, -462), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "wall_2_1", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-61, 2, 305), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "wall_2_2", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-328, 2, -193), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "wall_2_3", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-285, 2, -250), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "columns_2", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-253, 1, -350), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "midTunnel", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-5, 2, 465), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "elevation", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-22, 2, 248), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "bridge", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-37, 21, 384), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "elevRail", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-61.5, 26, 297.5), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "midRail", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    AddConcaveMesh(rp3d::Vector3(-88, 22, 187), rp3d::Vector3(5, 5, 5), rp3d::Quaternion(0, 0, 0, 1.0f),
        "bridgeRail", "basic", "basic", Vector4(1.0f, 0.0f, 0.0f, 1.0f));


    shoottest = AddPlayerClass(rp3d::Vector3(13, 5, 10.f));
    AddTrap();
    SetPlayer(shoottest);
    //InitDefaultFloor();
}



void GameManager::InitWorld(int arg)
{
    leftTime = totalTime;
    lockedObject = nullptr;
    selectionObject = nullptr;
    world->ClearAndErase();

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
                    Vector4(obj["color"][0], obj["color"][1], obj["color"][2], obj["color"][3]),
                    obj["mesh"], obj["texture"], obj["shader"],
                    obj["mass"]);
            }
        }
    }

    toRebuild = -1;
}

PaintballGameObject* GameManager::AddObject(GameObjectType type, const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation,
    Vector4 color, const string& meshName, const string& textureName, const string& shaderName,
    float mass, bool isEnemy, rp3d::Vector3 oriV3, int networkID)
{
    PaintballGameObject* obj = nullptr;
    switch (type)
    {
    case GameObjectType::cube:
        //std::cout << "[GameManager::AddObject] cube." << std::endl;
        obj = AddCube(position, dimensions * 0.5f, orientation, mass, color);
        break;

    case GameObjectType::sphere:
        //std::cout << "[GameManager::AddObject] cube." << std::endl;
        obj = AddSphere(position, dimensions, orientation, mass, color);
        break;
    case GameObjectType::bullet:
        obj = GameObjectFreeList::GetInstance().GetBullet(oriV3, isEnemy, position, dimensions, orientation, color, mass);
        break;
    case GameObjectType::concave:
        //std::cout << "[GameManager::AddObject] cube." << std::endl;
        obj = AddConcaveMesh(position, dimensions, orientation, meshName, textureName, shaderName, color);
        break;

    default:
        std::cout << "[GameManager::AddObject] Unsupported type : " << type << std::endl;
        return obj;
    }

    if (obj->GetNetworkObject() == nullptr) {
        NetworkObject* netObj = new NetworkObject(*obj, networkID == -1 ? networkObjects.size() : networkID);
        networkObjects[networkObjects.size()] = netObj;
        obj->SetNetworkObject(netObj);
    }

    return obj;
}

PaintballGameObject* GameManager::AddCube(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color) {
    ResourceManager& resources = ResourceManager::GetInstance();

    PaintballGameObject* cube = new PaintballGameObject(GameObjectType::cube);

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 2.0f)
        .SetRatioR(dimensions * 2.0f);

    cube->SetRenderObject(new PaintballRenderObject(&cube->GetTransform(), "cube", "basic", "basic"));
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

    cube->SetRenderObject(new PaintballRenderObject(&cube->GetTransform(), "sphere", "basic", "basic"));

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
        "role",
        "basic",
        "basic",
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
        "role",
        "basic",
        "basic",
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


PaintballGameObject* GameManager::AddConcaveMesh(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation,
    const string& meshName, const string& textureName, const string& shaderName, Vector4 color)
{

    ResourceManager& resources = ResourceManager::GetInstance();
    PaintballGameObject* concave = new PaintballGameObject(GameObjectType::concave);

    concave->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 1.0f)
        .SetRatioR(dimensions * 1.0f);

    concave->SetRenderObject(new PaintballRenderObject(&concave->GetTransform(),
        meshName, textureName, shaderName));

    concave->GetRenderObject()->SetColour(color);

    // create a rigid body
    rp3d::RigidBody* concaveBody = RpWorld->createRigidBody(concave->GetTransform().GetRpTransform());
    concaveBody->setType(rp3d::BodyType::STATIC);
    // create Shape
    rp3d::ConcaveMeshShape* shape = CreateConcaveMeshShape(resources.GetMesh(meshName));
    shape->setScale(dimensions);
    // bind Shape to rigid body
    rp3d::Transform shapeTransform = rp3d::Transform::identity();

    rp3d::Collider* collider = concaveBody->addCollider(shape, shapeTransform);
    //add rigid body to gameobject
    concave->SetPhysicsObject(new PaintballPhysicsObject(&concave->GetTransform(), *concaveBody, *RpWorld));

    world->AddGameObject(concave);
    return concave;
}

PaintballBullet* CSC8503::GameManager::AddBullet(rp3d::Vector3 ori3, bool isenemy, const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, Vector4 color, float mass)
{
    //AudioSystem::GetInstance().TriggerEvent("event:/Effect/GunShoot", position);

    ResourceManager& resources = ResourceManager::GetInstance();
    PaintballBullet* cube = new PaintballBullet();

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 1.0f)
        .SetRatioR(dimensions * 1.0f);

    cube->SetRenderObject(new PaintballRenderObject(&cube->GetTransform(), "sphere", "basic", "basic"));

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
    collider->setIsTrigger(true);
    collider->setCollisionCategoryBits(BULLET);

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

PaintballGameObject* CSC8503::GameManager::AddTrigger(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color)
{
    ResourceManager& resources = ResourceManager::GetInstance();

    PaintballGameObject* cube = new PaintballGameObject(GameObjectType::trigger1);

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 2.0f)
        .SetRatioR(dimensions * 2.0f);

    cube->SetRenderObject(new PaintballRenderObject(&cube->GetTransform(), "cube", "basic", "basic"));
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
   
    collider->setIsTrigger(true);
    world->AddGameObject(cube);

    return cube;
}

PaintballGameObject* CSC8503::GameManager::AddTrap()
{
    PaintballGameObject* tripcube1 =  AddCube(rp3d::Vector3(2, 35, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));
    PaintballGameObject* tripcube2 = AddCube(rp3d::Vector3(2, 35, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.01f, Vector4(1.0f, 0.0f, 0.0f, 1.0f));

    PaintballGameObject* trigger = AddTrigger(rp3d::Vector3(2, 15, -30), rp3d::Vector3(1, 1, 1), rp3d::Quaternion(0, 0, 0, 1.0f), 0.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f));
    trigger->target = tripcube1;
    // Anchor point on world-space
    const rp3d::Vector3 anchorPoint(2.0, 4.0, 0.0);

  
    // Create the joint info object
    reactphysics3d::BallAndSocketJointInfo jointInfo(&tripcube1->GetPhysicsObject()->GetRigidbody(), &tripcube2->GetPhysicsObject()->GetRigidbody(), anchorPoint);
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

void GameManager::SetGameState(PaintballGameState state) {
    //std::cout << "curState = " << state << std::endl;

    lastState = curState;
    curState = state;
    switch (state)
    {
    case LOADING:
        break;
    case PLAYING:
        if (leftTime == totalTime) {
            EventManager::Trigger(EventType::Game_Start);
        }
        else {
            EventManager::Trigger(EventType::Game_Resume);
        }
        break;
    case SERVERPLAYING:
        break;
    case CLIENTPLAYING:
        break;
    case PAUSED:
        EventManager::Trigger(EventType::Game_Pause);
        break;
    case FAILURE:
        break;
    case FINISH:
        break;
    case MENU:
        if (lastState == PLAYING || lastState == PAUSED) {
            InitWorld();
        }
        break;
    case SETTING:
        break;
    case CHOOSESERVER:
        break;
    case EXIT:
        break;
    default:
        break;
    }
}