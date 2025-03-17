#include "PaintballGameObject.h"
#include "GameManager.h"

using namespace NCL::CSC8503;

PaintballGameObject::PaintballGameObject(const GameObjectType& objectType, const std::string& objectName) {
	name = objectName;
	type = objectType;
	worldID = -1;
	isActive = true;
	networkObject = nullptr;
	renderObject = nullptr;
	physicsObject = nullptr;
	audioObject = nullptr;
	RegisterSL();
}

PaintballGameObject::~PaintballGameObject() {
	UnRegisterSL();
    delete networkObject;
    delete renderObject;
    delete physicsObject;
	delete audioObject;
}

void PaintballGameObject::Update(float dt) {
	// Sync physics transform to render transform
	if (GameManager::GetInstance().isPhysEnabled() && physicsObject) {
		rp3d::RigidBody& body = physicsObject->GetRigidbody();
		rp3d::Transform physicsTransform = body.getTransform();
		transform.SetRpTransform(physicsTransform);
	}

	// Update animation if applicable
	if (renderObject && renderObject->GetAnimation()) {
		renderObject->UpdateAnimation(dt);
	}

	if (audioObject) audioObject->Update();

	if (transform.GetPosition().y < -10) {
		physicsObject->GetRigidbody().setTransform(
			rp3d::Transform(rp3d::Vector3(0, 20, 0), rp3d::Quaternion().identity())
		);
	}
}

void PaintballGameObject::Delete()
{
	GameManager::GetInstance().DeleteObject(this);
}

void PaintballGameObject::SaveData(nlohmann::json& j) {
	//std::cout << "[obj] Saved" << std::endl;
	nlohmann::json objData;

	objData["type"] = type;

	objData["pos"].push_back(transform.GetPosition().x);
	objData["pos"].push_back(transform.GetPosition().y);
	objData["pos"].push_back(transform.GetPosition().z);

	objData["ori"].push_back(transform.GetOrientation().x);
	objData["ori"].push_back(transform.GetOrientation().y);
	objData["ori"].push_back(transform.GetOrientation().z);
	objData["ori"].push_back(transform.GetOrientation().w);

	objData["scale"].push_back(transform.GetScale().x);
	objData["scale"].push_back(transform.GetScale().y);
	objData["scale"].push_back(transform.GetScale().z);

	objData["mass"] = physicsObject->GetMass();

	objData["color"].push_back(renderObject->GetColour().x);
	objData["color"].push_back(renderObject->GetColour().y);
	objData["color"].push_back(renderObject->GetColour().z);
	objData["color"].push_back(renderObject->GetColour().w);

	objData["mesh"] = renderObject->GetMeshName();
	objData["texture"].push_back(renderObject->GetDefaultTextureName());
	objData["texture"].push_back(renderObject->GetSpecularTextureName());
	objData["texture"].push_back(renderObject->GetNormalTextureName());

	objData["texture"].push_back(renderObject->GetHeightTextureName());
	objData["texture"].push_back(renderObject->GetMetallicTextureName());
	objData["texture"].push_back(renderObject->GetOcclusionTextureName());
	objData["texture"].push_back(renderObject->GetMaskMapTextureName());
	objData["texture"].push_back(renderObject->GetRoughnessTextureName());

	objData["shader"] = renderObject->GetShaderName();

	j["objs"].push_back(objData);
}
