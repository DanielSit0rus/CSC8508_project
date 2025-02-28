/*
#pragma once
#include"BTNode.h"

class BTSequence : public BTNode {
public:
    void AddChild(BTNode* child) { children.push_back(child); }

    BTState Execute(float dt) override {
        for (BTNode* child : children) {
            BTState state = child->Execute(dt);
            if (state == BTState::Failure) {
                return BTState::Failure;
            }
            if (state == BTState::Ongoing) {
                return BTState::Ongoing;
            }
        }
        return BTState::Success;
    }
private:
    std::vector<BTNode*> children;
};
*/
