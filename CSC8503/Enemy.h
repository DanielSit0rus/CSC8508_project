#pragma once
#include <iostream>

class Enemy {
public:
    virtual void attack() = 0; // Pure virtual function
    virtual ~Enemy() {} // Virtual destructor for proper cleanup
};