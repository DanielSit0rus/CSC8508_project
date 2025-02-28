#pragma once
#include <vector>

enum class BTState {
    Success,
    Failure,
    Ongoing
};

class BTNode {
public:
    virtual ~BTNode() {}
    virtual bool Execute() = 0; // Ö´ÐÐÐÐÎª
};