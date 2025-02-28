/*
#pragma once
#include"BTNode.h"

class BTSelector : public BTNode {
public:
    void AddChild(BTNode* child) { children.push_back(child); }

    BTState Execute(float dt) override {
        for (BTNode* child : children) {
            BTState state = child->Execute(dt);
            if (state == BTState::Success || state == BTState::Ongoing) {
                return state; // �ҵ���ִ�е���Ϊ
            }
        }
        return BTState::Failure;
    }
private:
    std::vector<BTNode*> children;
};
*/
