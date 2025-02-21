#pragma once
#include "Enemy.h"

class StealthEnemy : public Enemy {
public:
    void attack() override {
        std::cout << "Hides behind obstacles and Attacks" << std::endl;
    }
};