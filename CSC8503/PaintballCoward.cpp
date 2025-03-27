#include "PaintballCoward.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include <iostream>
#include <cmath>
#include "PaintballGameObject.h"
#include "StateGameObject.h"
#include "GameManager.h"
#include "EventManager.h"

using namespace NCL::CSC8503;

PaintballCoward::PaintballCoward(const std::string& name, Vector4 color) : enemyColor(color)
{
    type = GameObjectType::enemy;

    stateMachine = new StateMachine();

    // ״̬1��Ѳ��
    State* patrolling = new State([&](float dt) -> void {
        if (health > 0)
        {
            this->GetRenderObject()->SetAnimation(ResourceManager::GetInstance().GetMoveanim());
            Patrol(dt);
        }
        else
        {
            this->GetRenderObject()->SetAnimation(nullptr);
        }
        });

    // ״̬2������/����
    State* attacking = new State([&](float dt) -> void {
        if (health > 0)
        {
            this->GetRenderObject()->SetAnimation(ResourceManager::GetInstance().GetIdleanim());
            Attack(enemyColor);

            // ������̫��������
            if (distanceToPlayer < fleeDistance) {
                Flee(dt);
            }
        }
        else
        {
            this->GetRenderObject()->SetAnimation(nullptr);
        }
        });

    stateMachine->AddState(patrolling);
    stateMachine->AddState(attacking);

    // ��Ѳ�ߵ�������ת��
    stateMachine->AddTransition(new StateTransition(patrolling, attacking, [&]() -> bool {
        return canSeeTest && (distanceToPlayer <= attackRange);
        }));

    // �ӹ����ص�Ѳ�ߵ�ת��
    stateMachine->AddTransition(new StateTransition(attacking, patrolling, [&]() -> bool {
        return !canSeeTest || (distanceToPlayer > attackRange);
        }));
}

PaintballCoward::~PaintballCoward()
{
}

void NCL::CSC8503::PaintballCoward::TakeDamage(int damage, Vector4 bulletColor) {
    if (IsOppositeColor(bulletColor)) {
        health -= damage;
        std::cout << "Coward enemy hit! Health: " << health << std::endl;
    }

    if (health <= 0) {
        std::cout << "Coward enemy eliminated!" << std::endl;

        if (indicatorSphere) {
            GameManager::GetInstance().DeleteObject(indicatorSphere);
            indicatorSphere = nullptr;
        }

        GameManager::GetInstance().DecreaseEnemyCount();
        std::cout << "Enemies Left: " << GameManager::GetInstance().GetEnemyCount() << std::endl;
    }
}

bool NCL::CSC8503::PaintballCoward::IsOppositeColor(const Vector4& bulletColor) {
    // �����ɫ�Ƿ��෴
    if ((enemyColor.x == 1 && enemyColor.y == 0 && enemyColor.z == 0 && // ��ɫ
        bulletColor.x == 0 && bulletColor.y == 1 && bulletColor.z == 0) || // ��ɫvs��ɫ

        (enemyColor.x == 0 && enemyColor.y == 1 && enemyColor.z == 0 && // ��ɫ
            bulletColor.x == 0 && bulletColor.y == 0 && bulletColor.z == 1) || // ��ɫvs��ɫ

        (enemyColor.x == 0 && enemyColor.y == 0 && enemyColor.z == 1 && // ��ɫ
            bulletColor.x == 1 && bulletColor.y == 0 && bulletColor.z == 0)) // ��ɫvs��ɫ
    {
        return true;
    }
    return false;
}

void NCL::CSC8503::PaintballCoward::Update(float dt)
{
    if (!IsActive()) {
        return;
    }

    // ����״̬������
    StateGameObject::Update(dt);

    // ����Ƿ���Կ������
    canSeeTest = CanSeePlayer();

    // ��������ҵľ���
    if (player) {
        distanceToPlayer = (player->GetTransform().GetPosition() - GetTransform().GetPosition()).length();
    }

    // ���¹�����ȴ
    leftCD -= dt;

    // ������
    if (canSeeTest) {
        rp3d::Vector3 enemyPos = GetTransform().GetPosition();
        rp3d::Vector3 playerPos = player->GetTransform().GetPosition();
        rp3d::Vector3 direction = playerPos - enemyPos;
        direction.y = 0;
        direction.normalize();

        float angle = atan2(direction.x, direction.z);
        rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0, angle, 0);
        GetTransform().SetOrientation(newRotation);
    }

    // ����ָʾ��λ��
    if (indicatorSphere) {
        rp3d::Vector3 enemyPos = GetTransform().GetPosition();
        indicatorSphere->GetTransform().SetPosition(enemyPos + rp3d::Vector3(0, 4, 0));
    }

    // ��Ƶ����
    if (audioObject) {
        float speed = physicsObject->GetRigidbody().getLinearVelocity().length();
        float clampedSpeed = Maths::Clamp(speed * 0.06f, 0.f, 1.f);
        audioObject->PlayEvent("event:/Effect/FootStep", clampedSpeed);
    }

    // �������� - ʹ����PaintballEnemy��ȫ��ͬ���߼�
    if (health <= 0)
    {
        rp3d::Vector3 currentPos = this->GetTransform().GetPosition();
        this->GetPhysicsObject()->AddForce(reactphysics3d::Vector3(0, 30, 0));

        // ����Ƿ�ﵽָ���߶�
        if (currentPos.y >= 10.0f) {
            SetActive(false); // ͣ�ö���
        }
    }
}

void NCL::CSC8503::PaintballCoward::Patrol(float dt) {
    if (canSeeTest) return;

    rp3d::Vector3 enemyPos = GetTransform().GetPosition();

    if (pathNodes.empty() || (enemyPos - patrolTarget).length() < 1.0f) {
        bool foundValidTarget = false;

        for (int i = 0; i < 5; ++i) {
            float randomX = (rand() % 40) - 20;
            float randomZ = (rand() % 40) - 20;

            rp3d::Vector3 candidate = enemyPos + rp3d::Vector3(randomX, 0, randomZ);

            rp3d::Ray ray(enemyPos, candidate);
            RaycastHitCallback callback;

            GameManager::GetInstance().getRPworld()->raycast(ray, &callback);

            if (!callback.rb) {
                patrolTarget = candidate;
                CalculatePath(patrolTarget);
                foundValidTarget = true;
                break;
            }
        }
    }
    MoveEnemyAlongPath();
}

void NCL::CSC8503::PaintballCoward::Flee(float dt) {
    if (!player) return;

    rp3d::Vector3 enemyPos = GetTransform().GetPosition();
    rp3d::Vector3 playerPos = player->GetTransform().GetPosition();

    // Direction away from player
    rp3d::Vector3 fleeDirection = enemyPos - playerPos;
    fleeDirection.y = 0;
    fleeDirection.normalize();

    // Calculate flee target position - �������ܾ���
    float fleeDistance = 30.0f; // ����Ŀ��������ӵ�30��λ
    rp3d::Vector3 fleeTarget = enemyPos + fleeDirection * fleeDistance;

    // ֱ�����ø��ߵ��ƶ��ٶȽ�������
    float emergencySpeed = 10.0f; // ����ʱ�ٶȼӱ�

    // ����Ѱ����Ч·������
    CalculatePath(fleeTarget);

    // ���û���ҵ���Ч·����·���̣ܶ�ֱ�������ܷ���ʩ����
    if (pathNodes.empty() || pathNodes.size() < 2) {
        // ֱ��ʩ��������
        this->GetPhysicsObject()->SetLinearVelocity(fleeDirection * emergencySpeed);
    }
    else {
        // ʹ�ø�����ٶ���·���ƶ�
        MoveEnemyAlongPathWithSpeed(emergencySpeed);
    }
}

void NCL::CSC8503::PaintballCoward::MoveEnemyAlongPathWithSpeed(float moveSpeed) {
    if (pathNodes.empty()) {
        this->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
        return;
    }

    rp3d::Vector3 currentPos = this->GetTransform().GetPosition();
    rp3d::Vector3 targetPos = Util::NCLToRP3d(pathNodes.front());
    targetPos.y = currentPos.y; // Keep enemy on the same Y level

    // Compute direction and distance to the target node
    rp3d::Vector3 direction = targetPos - currentPos;
    float distanceToTarget = direction.length();

    // Movement parameters
    float arrivalThreshold = 0.3f; // Distance at which a node is considered reached

    // ����ʱ�����̫������������ڵ㣬�����Զ��
    if (moveSpeed > 5.0f && pathNodes.size() > 2 && distanceToTarget < arrivalThreshold) {
        // ����ʱ��������ڵ㣬�������Ŀ���ƶ�
        pathNodes.erase(pathNodes.begin());
        if (!pathNodes.empty()) {
            pathNodes.erase(pathNodes.begin()); // ������һ���ڵ�
        }
    }
    else if (distanceToTarget < arrivalThreshold) {
        // �����ƶ�ʱֻ�Ƴ���ǰ�ڵ�
        pathNodes.erase(pathNodes.begin());
    }

    if (pathNodes.empty()) {
        this->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
        return;
    }

    // ����Ŀ��λ��
    targetPos = Util::NCLToRP3d(pathNodes.front());
    targetPos.y = currentPos.y;
    direction = targetPos - currentPos;

    if (direction.length() > 0.01f) {
        direction.normalize();
        float angle = atan2(direction.x, direction.z);
        rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0, angle, 0);
        GetTransform().SetOrientation(newRotation);
    }

    // Normalize direction and apply speed
    direction.normalize();
    rp3d::Vector3 velocity = direction * moveSpeed;

    // Set linear velocity
    this->GetPhysicsObject()->SetLinearVelocity(velocity);

    // Debug draw path
    for (size_t i = 1; i < pathNodes.size(); ++i) {
        const Vector3& from = pathNodes[i - 1];
        const Vector3& to = pathNodes[i];
        Debug::DrawLine(from, to, Vector4(0, 1, 1, 1), 0.01f); // Cyan lines for coward path
    }
}

void NCL::CSC8503::PaintballCoward::Attack(Vector4 color)
{
    if (!player || !player->IsActive()) {
        return;
    }

    if (leftCD < 0) {
        GameManager::GetInstance().AddObject(GameObjectType::bullet,
            transform.GetPosition() + rp3d::Vector3(0, 4, 0), rp3d::Vector3(1, 1, 1), rp3d::Quaternion().identity(),
            color, "basic", "basic", "", "", "", "", "", "", "", "basic", 1, false, player->GetTransform().GetPosition() - transform.GetPosition());
        leftCD = totalCD;
    }
}

bool NCL::CSC8503::PaintballCoward::CanSeePlayer() {
    if (player == nullptr) return false;

    rp3d::Vector3 enemyPos = GetTransform().GetPosition();
    rp3d::Vector3 playerPos = player->GetTransform().GetPosition();

    float distanceToPlayer = (playerPos - enemyPos).length();
    if (distanceToPlayer > attackRange) {
        return false;
    }

    rp3d::Ray ray(enemyPos, playerPos);
    RaycastHitCallback callback;
    GameManager::GetInstance().getRPworld()->raycast(ray, &callback);

    if (callback.rb && callback.rb->getUserData()) {
        return (PaintballGameObject*)callback.rb->getUserData() == player;
    }
    return false;
}

void NCL::CSC8503::PaintballCoward::MoveEnemyAlongPath() {
    if (pathNodes.empty()) {
        this->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
        return;
    }

    rp3d::Vector3 currentPos = this->GetTransform().GetPosition();
    rp3d::Vector3 targetPos = Util::NCLToRP3d(pathNodes.front());
    targetPos.y = currentPos.y; // ���ֵ�����ͬһ�߶�

    // ���㵽Ŀ��ڵ�ķ���;���
    rp3d::Vector3 direction = targetPos - currentPos;
    float distanceToTarget = direction.length();

    // �ƶ�����
    float moveSpeed = 5.0f;
    float arrivalThreshold = 0.3f; // �ﵽ�ڵ����ֵ����

    if (distanceToTarget < arrivalThreshold) {
        // �Ƴ��ѵ���Ľڵ�
        pathNodes.erase(pathNodes.begin());
        if (pathNodes.empty()) {
            this->GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
            return;
        }
        // ���µ��µ�·��ǰ�˵�Ŀ��λ��
        targetPos = Util::NCLToRP3d(pathNodes.front());
        targetPos.y = currentPos.y;
        direction = targetPos - currentPos;
    }

    if (direction.length() > 0.01f) {
        direction.normalize();
        float angle = atan2(direction.x, direction.z);
        rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0, angle, 0);
        GetTransform().SetOrientation(newRotation);
    }

    // ��׼������Ӧ���ٶ�
    direction.normalize();
    rp3d::Vector3 velocity = direction * moveSpeed;

    // ���������ٶ�
    this->GetPhysicsObject()->SetLinearVelocity(velocity);

    // ���Ի���·��
    for (size_t i = 1; i < pathNodes.size(); ++i) {
        const Vector3& from = pathNodes[i - 1];
        const Vector3& to = pathNodes[i];
        Debug::DrawLine(from, to, Vector4(0, 1, 1, 1), 0.01f); // ��ɫ�����ڵ�С����·��
    }
}

void NCL::CSC8503::PaintballCoward::CalculatePath(rp3d::Vector3 pos) {
    pathNodes.clear();
    if (navMesh == nullptr)return;

    rp3d::Vector3 startPos = this->GetTransform().GetPosition();
    rp3d::Vector3 endPos = pos;

    NavigationPath outPath;

    if (navMesh->FindPath(Util::RP3dToNCL(startPos), Util::RP3dToNCL(endPos), outPath)) {
        Vector3 pathPos;
        int count = 0;
        while (outPath.PopWaypoint(pathPos)) {
            if (count % 2 == 0) {
                pathNodes.push_back(pathPos);
            }
            count++;
        }
    }
}