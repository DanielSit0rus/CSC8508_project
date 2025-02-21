#pragma once
#include "GameManager.h"
#include "NormalEnemy.h"
#include "RangedEnemy.h"
#include "StealthEnemy.h"

enum class EnemyType { NORMAL, RANGED, STEALTH };

class EnemyFactory {
public:
    static Enemy* createEnemy(EnemyType type) {
        switch (type) {
        case EnemyType::NORMAL:
            return new NormalEnemy();
        case EnemyType::RANGED:
            return new RangedEnemy();
        case EnemyType::STEALTH:
            return new StealthEnemy();
        default:
            return nullptr;
        }
    }
};