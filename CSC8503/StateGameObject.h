#pragma once
#include "GameObject.h"

namespace NCL {
    namespace CSC8503 {
        class StateMachine;
        class StateGameObject : public GameObject  {
        public:
            StateGameObject(const std::string& objectName = "", GameObject* playerObj = nullptr);
            //StateGameObject(const std::string& name = "");
            ~StateGameObject();
            StateMachine* stateMachine;
            virtual void Update(float dt);
            StateMachine* getstateMachine();
            void setstateMachine(StateMachine* StateMachine);
            

        protected:
            void MoveLeft(float dt);
            void MoveRight(float dt);
            //std::string	name;
            
            float counter;
          
        };
    }
}
