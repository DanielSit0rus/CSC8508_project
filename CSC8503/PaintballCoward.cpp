#include "PaintballCoward.h"
#include "GameManager.h"
#include "Debug.h"
#include "StateTransition.h"
#include "StateMachine.h"
#include "State.h"
#include "PaintballGameObject.h"
#include "StateGameObject.h"
#include "EventManager.h"


using namespace NCL::CSC8503;

PaintballCoward::PaintballCoward(const std::string& name, Vector4 color)
    : PaintballEnemy(name, color),
    optimalDistance(35.0f),  // Ideal distance, farther than normal enemies
    minDistance(20.0f),      // Minimum distance that triggers fleeing
    maxAttackRange(50.0f),   // Can attack from long distance
    evadeSpeed(15.0f)        // Fleeing speed
{
    // Override parent class state machine setup
    SetupStateMachine();

    // Initialize behavior tree
    InitBehaviorTree();

    // Mark as cowardly enemy type
    type = GameObjectType::enemy;

    // Update attack range and cooldown
    attackRange = 45.0f;      // Long-range attack
    stopchaseRange = 60.0f;   // Abandons chase earlier
    totalCD = 2.5f;           // Longer attack cooldown
    leftCD = 0.0f;
}

PaintballCoward::~PaintballCoward() {
    // Clean up resources
}

void PaintballCoward::SetupStateMachine() {
    // Delete state machine created by parent class
    delete stateMachine;
    stateMachine = new StateMachine();

    // Create new states
    State* patrolling = new State([&](float dt) -> void {
        Patrol(dt);
        });

    State* keepingDistance = new State([&](float dt) -> void {
        KeepDistance(dt);
        });

    State* attacking = new State([&](float dt) -> void {
        Attack(enemyColor);
        });

    // Add states
    stateMachine->AddState(patrolling);
    stateMachine->AddState(keepingDistance);
    stateMachine->AddState(attacking);

    // From patrolling to keeping distance
    stateMachine->AddTransition(new StateTransition(patrolling, keepingDistance, [&]() -> bool {
        return canSeeTest && (distanceToPlayer <= stopchaseRange);
        }));

    // From keeping distance to attacking
    stateMachine->AddTransition(new StateTransition(keepingDistance, attacking, [&]() -> bool {
        return canSeeTest && (distanceToPlayer <= maxAttackRange) && (distanceToPlayer >= minDistance);
        }));

    // From attacking to keeping distance
    stateMachine->AddTransition(new StateTransition(attacking, keepingDistance, [&]() -> bool {
        return canSeeTest && (distanceToPlayer > maxAttackRange || distanceToPlayer < minDistance);
        }));

    // From any state back to patrolling
    stateMachine->AddTransition(new StateTransition(keepingDistance, patrolling, [&]() -> bool {
        return !canSeeTest || distanceToPlayer > stopchaseRange;
        }));

    stateMachine->AddTransition(new StateTransition(attacking, patrolling, [&]() -> bool {
        return !canSeeTest || distanceToPlayer > stopchaseRange;
        }));
}

void PaintballCoward::Update(float dt) {
    if (!IsActive()) {
        return;
    }

    // Update behavior tree or state machine
    if (behaviorTree) {
        behaviorTree->Execute();  // If behavior tree exists, execute it
    }
    else {
        PaintballEnemy::Update(dt); // Otherwise fall back to state machine logic
    }

    // Keep updating canSeeTest and distanceToPlayer as they are needed in both behavior modes
    canSeeTest = CanSeePlayer();
    if (player) distanceToPlayer = (player->GetTransform().GetPosition() - GetTransform().GetPosition()).length();

    // Debug information
    std::string stateInfo = "Coward State: ";
    // Determine current state by checking state machine behavior
    if (!canSeeTest) {
        stateInfo += "Patrolling";
    }
    else if (leftCD > 0 || distanceToPlayer > maxAttackRange || distanceToPlayer < minDistance) {
        stateInfo += "KeepingDistance";
    }
    else {
        stateInfo += "Attacking";
    }

    // Update audio and other components
    if (audioObject) {
        float speed = physicsObject->GetRigidbody().getLinearVelocity().length();
        float clampedSpeed = Maths::Clamp(speed * 0.06f, 0.f, 1.f);
        audioObject->PlayEvent("event:/Effect/FootStep", clampedSpeed);
    }

    // Update indicator sphere position
    if (indicatorSphere) {
        rp3d::Vector3 enemyPos = GetTransform().GetPosition();
        indicatorSphere->GetTransform().SetPosition(enemyPos + rp3d::Vector3(0, 4, 0));
    }
}

void PaintballCoward::KeepDistance(float dt) {
    if (!player) return;

    rp3d::Vector3 currentPos = GetTransform().GetPosition();
    rp3d::Vector3 playerPos = player->GetTransform().GetPosition();

    // Calculate direction vector
    rp3d::Vector3 dirToPlayer = playerPos - currentPos;
    float currentDistance = dirToPlayer.length();

    // Adjust direction to maintain optimal distance
    if (currentDistance < optimalDistance) {
        // Too close, need to move away
        dirToPlayer.normalize();
        rp3d::Vector3 targetPos = currentPos - dirToPlayer * (optimalDistance - currentDistance);

        // Set higher fleeing speed
        float speed = (currentDistance < minDistance) ? evadeSpeed : 10.0f;
        rp3d::Vector3 velocity = -dirToPlayer * speed;

        // Apply velocity
        GetPhysicsObject()->SetLinearVelocity(velocity);
    }
    else if (currentDistance > optimalDistance + 10.0f) {
        // Too far, need to approach but maintain distance
        dirToPlayer.normalize();
        rp3d::Vector3 targetPos = currentPos + dirToPlayer * (currentDistance - optimalDistance);

        CalculatePath(targetPos);
        MoveEnemyAlongPath();
    }
    else {
        // Within ideal range, maintain position but face player
        GetPhysicsObject()->SetLinearVelocity(rp3d::Vector3(0, 0, 0));
    }

    // Always face the player
    if (canSeeTest) {
        rp3d::Vector3 direction = playerPos - currentPos;
        direction.y = 0;
        direction.normalize();

        float angle = atan2(direction.x, direction.z);
        rp3d::Quaternion newRotation = rp3d::Quaternion::fromEulerAngles(0, angle, 0);
        GetTransform().SetOrientation(newRotation);
    }
}

void PaintballCoward::Chase(float dt) {
    // Cowardly enemies don't directly chase the player, they maintain distance instead
    KeepDistance(dt);
}

void PaintballCoward::Attack(Vector4 color) {
    if (!player || !player->IsActive()) {
        return;
    }

    // Only attack when cooldown is complete and distance is appropriate
    if (leftCD < 0 && distanceToPlayer <= maxAttackRange && distanceToPlayer >= minDistance) {
        GameManager::GetInstance().AddObject(GameObjectType::bullet,
            transform.GetPosition() + rp3d::Vector3(0, 4, 0), rp3d::Vector3(1, 1, 1), rp3d::Quaternion().identity(),
            color, "basic", "basic", "", "", "", "", "", "", "", "basic", 1, false, player->GetTransform().GetPosition() - transform.GetPosition());

        // Reset cooldown time
        leftCD = totalCD;
    }
}

void PaintballCoward::InitBehaviorTree() {
    // Create behavior tree root node (selector)
    BTSelector* rootSelector = new BTSelector();

    // Create "flee when threatened" sequence
    BTSequence* evadeSequence = new BTSequence();
    evadeSequence->AddChild(new ConditionIsThreatened(this));  // Check if threatened
    evadeSequence->AddChild(new ActionMaintainDistance(this)); // Maintain distance (flee)

    // Create "attack from safe distance" sequence
    BTSequence* attackSequence = new BTSequence();
    attackSequence->AddChild(new ConditionCanSeePlayer(this));   // Check if can see player
    attackSequence->AddChild(new ConditionInAttackRange(this));  // Check if in attack range
    attackSequence->AddChild(new ActionAttackFromDistance(this)); // Launch attack

    // Create "maintain optimal distance" sequence
    BTSequence* adjustDistanceSequence = new BTSequence();
    adjustDistanceSequence->AddChild(new ConditionCanSeePlayer(this)); // Check if can see player
    adjustDistanceSequence->AddChild(new ActionMaintainDistance(this)); // Adjust distance

    // Set behavior tree priorities
    rootSelector->AddChild(evadeSequence);           // Highest priority: If threatened, flee immediately
    rootSelector->AddChild(attackSequence);          // Second priority: If at suitable distance, attack
    rootSelector->AddChild(adjustDistanceSequence);  // Third priority: Adjust to optimal distance
    rootSelector->AddChild(new ActionPatrol(this));  // Lowest priority: Patrol

    // Set root node
    behaviorTree = rootSelector;
}