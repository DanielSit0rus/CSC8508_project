#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "PaintballPhysicsObject.h"
#include "PaintballRenderObject.h"
#include "PaintballTransform.h"
#include <NetworkObject.h>


using std::vector;

namespace NCL::CSC8503 {
    //class NetworkObject;
    class PaintballRenderobject;
    class PaintballPhysicsObject;

    class PaintballGameObject
    {
    public:
        PaintballGameObject(const std::string& name = "");
        ~PaintballGameObject();

        virtual void Update();

        bool IsActive() const {
            return isActive;
        }

        PaintballTransform& GetTransform() {
            return transform;
        }

        PaintballRenderObject* GetRenderObject() const {
            return renderObject;
        }

        PaintballPhysicsObject* GetPhysicsObject() const {
            return physicsObject;
        }

        NetworkObject* GetNetworkObject() const {
            return networkObject;
        }

        void SetRenderObject(PaintballRenderObject* newObject) {
            renderObject = newObject;
        }

        void SetPhysicsObject(PaintballPhysicsObject* newObject) {
            physicsObject = newObject;
            physicsObject->GetRigidbody().setUserData(this);
        }

        const std::string& GetName() const {
            return name;
        }

        
		void SetWorldID(int newID) {
			worldID = newID;
		}

		int GetWorldID() const {
			return worldID;
		}

    protected:
        PaintballTransform transform;

        PaintballRenderObject* renderObject;
        PaintballPhysicsObject* physicsObject;

        NetworkObject* networkObject;

        bool		isActive;
        int			worldID;
        std::string	name;
    };

}