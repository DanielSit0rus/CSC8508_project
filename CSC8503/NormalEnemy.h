#pragma once
#include "Enemy.h"

class NormalEnemy : public Enemy {
public:
    void attack() override {
        std::cout << "Attacks with a normal GUN" << std::endl;
    }
};