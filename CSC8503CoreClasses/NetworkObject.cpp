#include "NetworkObject.h"
#include "./enet/enet.h"
using namespace NCL;
using namespace CSC8503;

NetworkObject::NetworkObject(PaintballGameObject& o, int id) : object(o)	{
	deltaErrors = 0;
	fullErrors  = 0;
	networkID   = id;
}

NetworkObject::~NetworkObject()	{
}

bool NetworkObject::ReadPacket(GamePacket& p) {
	if (p.type == Delta_State) {
		return ReadDeltaPacket((DeltaPacket&)p);
	}
	if (p.type == Full_State) {
		return ReadFullPacket((FullPacket&)p);
	}
	return false; //this isn't a packet we care about!
}

bool NetworkObject::WritePacket(GamePacket** p, bool deltaFrame, int stateID) {
	if (deltaFrame) {
		if (!WriteDeltaPacket(p, stateID)) {
			return WriteFullPacket(p);
		}
		return true;
	}
	return WriteFullPacket(p);
}
//Client objects recieve these packets
bool NetworkObject::ReadDeltaPacket(DeltaPacket &p) {
	if (p.fullID != lastFullState.stateID) {
		return false;
	}
	UpdateStateHistory(p.fullID);

	rp3d::Vector3 fullPos = lastFullState.position;
	rp3d::Quaternion fullOrientation = lastFullState.orientation;

	fullPos.x += p.pos[0];
	fullPos.y += p.pos[1];
	fullPos.z += p.pos[2];

	fullOrientation.x += ((float)p.orientation[0]) / 127.0f;
	fullOrientation.y += ((float)p.orientation[1]) / 127.0f;
	fullOrientation.z += ((float)p.orientation[2]) / 127.0f;
	fullOrientation.w += ((float)p.orientation[3]) / 127.0f;

	object.GetTransform().SetPosition(fullPos);
	object.GetTransform().SetOrientation(fullOrientation);

	return true;
}

bool NetworkObject::ReadFullPacket(FullPacket &p) {
	if (p.fullState.toDelete == true) {
		//std::cout << "[delete]" << std::endl;
		object.Delete();
		return true;
	}

	if (p.fullState.stateID < lastFullState.stateID) {
		return false;
	}
	lastFullState = p.fullState;
	object.SetActive(p.fullState.isActive);

	if (object.IsActive()) {
		object.GetTransform().SetPosition(lastFullState.position);
		object.GetTransform().SetOrientation(lastFullState.orientation);
		object.GetRenderObject()->SetColour(lastFullState.color);
	}

	stateHistory.emplace_back(lastFullState);

	return true;
}

bool NetworkObject::WriteDeltaPacket(GamePacket**p, int stateID) {
	DeltaPacket* dp = new DeltaPacket();
	NetworkState state;
	if (!GetNetworkState(stateID, state)) {
		//std::cout << "false" << std::endl;
		return false;
	}
	dp->fullID = stateID;
	dp->objectID = networkID;

	rp3d::Vector3 currentPos = object.GetTransform().GetPosition();
	rp3d::Quaternion currentOrientation = object.GetTransform().GetOrientation();

	currentPos -= state.position;
	currentOrientation -= state.orientation;

	dp->pos[0] = (char)currentPos.x;
	dp->pos[1] = (char)currentPos.y;
	dp->pos[2] = (char)currentPos.z;

	dp->orientation[0] = (char)(currentOrientation.x * 127.0f);
	dp->orientation[1] = (char)(currentOrientation.y * 127.0f);
	dp->orientation[2] = (char)(currentOrientation.z * 127.0f);
	dp->orientation[3] = (char)(currentOrientation.w * 127.0f);
	*p = dp;

	//std::cout << "succeed " << dp->fullID << std::endl;
	UpdateStateHistory(dp->fullID);

	return true;
}

bool NetworkObject::WriteFullPacket(GamePacket**p) {
	FullPacket* fp = new FullPacket();
	fp->objectID = networkID;

	if (toDelete == true) {
		fp->fullState.toDelete = true;
		//std::cout << "[delete]!" << networkID << std::endl;
	}
	else
	{
		fp->fullState.type = object.GetType();
		fp->fullState.position = object.GetTransform().GetPosition();		//GetWorldPosition();
		fp->fullState.scale = object.GetTransform().GetScale();
		fp->fullState.orientation = object.GetTransform().GetOrientation();	//GetWorldOrientation();
		
		fp->fullState.size[0] = (short)object.GetRenderObject()->GetMeshName().length();
		fp->fullState.size[1] = (short)object.GetRenderObject()->GetTextureName().length();
		fp->fullState.size[2] = (short)object.GetRenderObject()->GetShaderName().length();
		Util::StringToCharArray(object.GetRenderObject()->GetMeshName(), fp->fullState.meshName);
		Util::StringToCharArray(object.GetRenderObject()->GetTextureName(), fp->fullState.textureName);
		Util::StringToCharArray(object.GetRenderObject()->GetShaderName(), fp->fullState.shaderName);

		fp->fullState.color = object.GetRenderObject()->GetColour();
		fp->fullState.mass = object.GetPhysicsObject()->GetMass();

		fp->fullState.stateID = lastFullState.stateID++;
		fp->fullState.isActive = object.IsActive();
	}
	*p = fp;

	stateHistory.emplace_back(fp->fullState);
	
	//std::cout << "full packet"  << std::endl;

	return true;
}

NetworkState& NetworkObject::GetLatestNetworkState() {
	return lastFullState;
}

bool NetworkObject::GetNetworkState(int stateID, NetworkState& state) {
	for (auto i = stateHistory.begin(); i < stateHistory.end(); ++i) {
		//std::cout << (*i).stateID << " ";
		if ((*i).stateID == stateID) {
			state = (*i);
			return true;
		}
	}
	//std::cout << std::endl << stateHistory.size() << " not found " << std::endl;
	return false;
}

void NetworkObject::UpdateStateHistory(int minID) {
	for (auto i = stateHistory.begin(); i < stateHistory.end();) {
		if ((*i).stateID < minID) {
			i = stateHistory.erase(i);
		}
		else {
			++i;
		}
	}
}