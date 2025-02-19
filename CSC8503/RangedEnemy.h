#pragma once
#include "Enemy.h"

class RangedEnemy : public Enemy {
public:
    void attack() override {
        std::cout << "Attacks with a Ranged GUN" << std::endl;
    }
};