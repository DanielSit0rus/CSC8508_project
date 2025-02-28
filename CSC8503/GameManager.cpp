#include "GameManager.h"

using namespace NCL;
using namespace CSC8503;

void GameManager::Init(PaintballGameWorld* world)
{
    this->world = world;
    RpWorld = physicsCommon.createPhysicsWorld(RpSettings);
    if (RpWorld == nullptr) {
        std::cerr << "Error: Failed to create Physics World!" << std::endl;
        throw std::runtime_error("Physics World creation failed!");
    }
    

    RpWorld->setEventListener(&bulletlistener);
}

void GameManager::Update() {
    for (auto object : objectsToDelete) {
        world->RemoveGameObject(object, true);
    }
    objectsToDelete.clear();
}

PaintballGameObject* GameManager::AddCube(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color) {
    ResourceManager& resources = ResourceManager::GetInstance();

    PaintballGameObject* cube = new PaintballGameObject();

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 2.0f);

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
    PaintballGameObject* cube = new PaintballGameObject();

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 1.0f);

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
    return AddCube(position, rp3d::Vector3(200, 2, 200), rp3d::Quaternion(0, 0, 0, 1.0f),0, Vector4(1,1,1,1));
}

PaintballGameObject* GameManager::AddPlayer(const rp3d::Vector3& position) {
    
    PaintballGameObject* p = AddCube(position, rp3d::Vector3(0.3f, 1, 0.3f), rp3d::Quaternion(0, 0, 0, 1.0f));
    p->GetPhysicsObject()->GetRigidbody().setAngularLockAxisFactor(rp3d::Vector3(0, 1, 0));
    return p;
}

PaintballPlayer* GameManager::AddPlayerClass(rp3d::Vector3 position) {
    
    rp3d::Vector3 dimensions = rp3d::Vector3(0.3f, 1, 0.3f);

    ResourceManager& resources = ResourceManager::GetInstance();

    //PaintballPlayer* cube = new PaintballPlayer();
    player = new PaintballPlayer();

    player->GetTransform()
        .SetPosition(position)
        .SetScale(dimensions * 2.0f);

    player->SetRenderObject(new PaintballRenderObject(&player->GetTransform(), resources.GetCubeMesh(), resources.GetBasicTex(), resources.GetBasicShader()));
    player->GetRenderObject()->SetColour(Vector4(0, 0, 1, 1));

    // create a rigid body
    rp3d::RigidBody* cubeBody = RpWorld->createRigidBody(player->GetTransform().GetRpTransform());
    // create Shape
    rp3d::BoxShape* shape = physicsCommon.createBoxShape(dimensions);
    //rp3d::SphereShape* shape = physicsCommon.createSphereShape(halfExtents.x);
    // bind Shape to rigid body
    rp3d::Transform shapeTransform = rp3d::Transform::identity();
    rp3d::Collider* collider = cubeBody->addCollider(shape, shapeTransform);
    //add rigid body to gameobject
    player->SetPhysicsObject(new PaintballPhysicsObject(&player->GetTransform(), *cubeBody, *RpWorld));
    player->GetPhysicsObject()->SetMass(1);
    player->GetPhysicsObject()->GetRigidbody().setAngularLockAxisFactor(rp3d::Vector3(0, 1, 0));

    collider->setCollisionCategoryBits(PLAYER);

    player->SetCamera(&world->GetMainCamera());

    world->AddGameObject(player);

    return player;

}

PaintballGameObject* GameManager::AddConcaveMesh(const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color)
{

    ResourceManager& resources = ResourceManager::GetInstance();
    PaintballGameObject* concave = new PaintballGameObject();

    concave->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions);

    concave->SetRenderObject(new PaintballRenderObject(&concave->GetTransform(),resources.GetMapMesh(), resources.GetBasicTex(),resources.GetBasicShader()));

    concave->GetRenderObject()->SetColour(color);

    rp3d::Vector3 pos(position.x, position.y, position.z);
    rp3d::Quaternion ori = rp3d::Quaternion(orientation.x, orientation.y, orientation.z, orientation.w);


    // create a rigid body
    rp3d::Transform transform(pos, ori);
    rp3d::RigidBody* concaveBody = RpWorld->createRigidBody(transform);
    concaveBody->setType(rp3d::BodyType::STATIC);
    // create Shape
    rp3d::ConcaveMeshShape* shape = CreateConcaveMeshShape(resources.GetMapMesh()); // scale?
    // bind Shape to rigid body
    rp3d::Transform shapeTransform = rp3d::Transform::identity();
    rp3d::Collider* collider = concaveBody->addCollider(shape, shapeTransform);
    //add rigid body to gameobject
    concave->SetPhysicsObject(new PaintballPhysicsObject(&concave->GetTransform(), *concaveBody, *RpWorld));

    world->AddGameObject(concave);
    return concave;
}

PaintballGameObject* NCL::CSC8503::GameManager::AddBullet(bool isenemy, const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation, float mass, Vector4 color)
{
    ResourceManager& resources = ResourceManager::GetInstance();
    PaintballBullet* cube = new PaintballBullet("bullet");

    cube->GetTransform()
        .SetPosition(position)
        .SetOrientation(orientation)
        .SetScale(dimensions * 1.0f);

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

    // 设置子弹（只与玩家碰撞，不与敌人碰撞）
    collider->setCollisionCategoryBits(BULLET);

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