#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "PaintballPhysicsObject.h"
#include "PaintballRenderObject.h"
#include "PaintballTransform.h"


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

        void Update();

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

        void SetRenderObject(PaintballRenderObject* newObject) {
            renderObject = newObject;
        }

        void SetPhysicsObject(PaintballPhysicsObject* newObject) {
            physicsObject = newObject;
        }

        const std::string& GetName() const {
            return name;
        }

        
		void SetWorldID(int newID) {
			worldID = newID;
		}

		int		GetWorldID() const {
			return worldID;
		}

    protected:
        PaintballTransform transform;

        PaintballRenderObject* renderObject;
        PaintballPhysicsObject* physicsObject;

        //NetworkObject* networkObject;

        bool		isActive;
        int			worldID;
        std::string	name;
    };

}