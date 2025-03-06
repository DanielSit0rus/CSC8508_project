#pragma once

#include <reactphysics3d/reactphysics3d.h>
#include "PaintballPhysicsObject.h"
#include "PaintballRenderObject.h"
#include "PaintballTransform.h"
#include "PaintballAudioObject.h"
#include <NetworkObject.h>
#include "ISaveable.h"


using std::vector;

namespace NCL::CSC8503 {
    class NetworkObject;
    class PaintballRenderobject;
    class PaintballPhysicsObject;
    class PaintballAudioObject;

    enum GameObjectType //To record which AddObj() method is used and to be used
    {
        _NULL,

        cube,
        sphere,
        player,
        enemy,
        bullet,

        concave1,
        concave2,

        test_character,
        test_stateObj,
        test_networkPlayer,
    };

    class PaintballGameObject : public ISaveable
    {
    public:
        PaintballGameObject(const GameObjectType& type, const std::string& name = "");
        ~PaintballGameObject();

        virtual void Update(float dt);

        bool IsActive() const {
            return isActive;
        }

        void SetActive(bool active) {
            isActive = active;
            physicsObject->GetRigidbody().setIsActive(isActive);
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

        PaintballAudioObject* GetAudioObject() const {
            return audioObject;
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

        void SetAudioObject(PaintballAudioObject* newObject) {
            audioObject = newObject;
        }

        const std::string& GetName() const {
            return name;
        }

        const GameObjectType& GetType() const {
            return type;
        }

        void SetWorldID(int newID) {
            worldID = newID;
        }

		int GetWorldID() const {
			return worldID;
		}

    protected:
        void SaveData(nlohmann::json& j);
        void LoadData(nlohmann::json& j) {
            //std::cout << "[obj] Loaded" << std::endl;
        }


        PaintballTransform transform;

        PaintballRenderObject*      renderObject;
        PaintballPhysicsObject*     physicsObject;
        PaintballAudioObject*       audioObject;

        NetworkObject* networkObject;

        bool		isActive;
        int			worldID;
        std::string	name;
        GameObjectType type;  //To record which AddObj() method is used and to be used

    };

}