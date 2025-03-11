#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "NetworkObject.h"
#include "GameServer.h"
#include "GameClient.h"

#define COLLISION_MSG 30

using namespace NCL::CSC8503;

struct MessagePacket : public GamePacket {
	short playerID;
	short messageID;

	MessagePacket() {
		type = Message;
		size = sizeof(short) * 2;
	}
};

NetworkedGame::NetworkedGame() {
	thisServer = nullptr;
	thisClient = nullptr;

	NetworkBase::Initialise();
	timeToNextPacket = 0.0f;
	packetsToSnapshot = 0;

	EventManager::Subscribe(EventType::Network_StartAsServer, [this]() {StartAsServer(); });
	EventManager::Subscribe(EventType::Network_StartAsClient, [this](std::string& s) {
		std::istringstream stream(s); int a, b, c, d;
		stream >> a; stream >> b; stream >> c; stream >> d;
		StartAsClient(a, b, c, d);
		});
	EventManager::Subscribe(EventType::Network_Test, [this]() {SendPacketTest(); });
	EventManager::Subscribe(EventType::Network_Test, [this](std::string& s) {SendPacketTest(s); });
}

NetworkedGame::~NetworkedGame()	{
	delete thisServer;
	delete thisClient;
}

void NetworkedGame::StartAsServer() {
	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 4);

	thisServer->RegisterPacketHandler(Received_State, this);
	thisServer->RegisterPacketHandler(String_Message, this);

	std::cout << "[NetworkedGame.cpp]Start As Server" << std::endl;

	StartLevel();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	thisClient = new GameClient();

	bool isConnected = thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());
	if (isConnected) std::cout << "[NetworkedGame.cpp]Connected" << std::endl;
	else std::cout << "[NetworkedGame.cpp]Error in connection" << std::endl;

	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	thisClient->RegisterPacketHandler(String_Message, this);

	GameManager::GetInstance().EnablePhys(false);

	StartLevel();
}

void NetworkedGame::UpdateGame(float dt) {
	timeToNextPacket -= dt;
	if (timeToNextPacket < 0) {
		if (thisServer) {
			UpdateAsServer(dt);
		}
		else if (thisClient) {
			UpdateAsClient(dt);
		}
		timeToNextPacket += 1.0f / frame; //20hz server/client update

		GameManager::GetInstance().PostCleanUp();
	}

	TutorialGame::UpdateGame(dt);
}

void NetworkedGame::UpdateAsServer(float dt) {
	//packetsToSnapshot--;
	//if (packetsToSnapshot < 0) {
	//	BroadcastSnapshot(false);
	//	packetsToSnapshot = 5;
	//}
	//else {
	//	BroadcastSnapshot(true);
	//}

	BroadcastSnapshot(false);

	this->thisServer->UpdateServer();
}

void NetworkedGame::UpdateAsClient(float dt) {
	ClientPacket newPacket;
	newPacket.type = Received_State;

	newPacket.buttonstates[0] = Window::GetKeyboard()->KeyDown(KeyCodes::W) ? 1 : 0;
	newPacket.buttonstates[1] = Window::GetKeyboard()->KeyDown(KeyCodes::A) ? 1 : 0;
	newPacket.buttonstates[2] = Window::GetKeyboard()->KeyDown(KeyCodes::S) ? 1 : 0;
	newPacket.buttonstates[3] = Window::GetKeyboard()->KeyDown(KeyCodes::D) ? 1 : 0;
	newPacket.buttonstates[4] = Window::GetKeyboard()->KeyDown(KeyCodes::SPACE) ? 1 : 0;

	newPacket.camFront= GameManager::GetInstance().GetCameraFront();

	thisClient->SendPacket(newPacket);

	this->thisClient->UpdateClient();
}

void NetworkedGame::BroadcastSnapshot(bool deltaFrame) {
	//std::cout << "BroadcastSnapshot" << std::endl;

	std::vector<PaintballGameObject*>::const_iterator first;
	std::vector<PaintballGameObject*>::const_iterator last;

	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		//TODO - you'll need some way of determining
		//when a player has sent the server an acknowledgement
		//and store the lastID somewhere. A map between player
		//and an int could work, or it could be part of a 
		//NetworkPlayer struct. 
		GamePacket* newPacket = nullptr;
		if (o->WritePacket(&newPacket, deltaFrame, playerState)) {
			thisServer->SendGlobalPacket(*newPacket);
			delete newPacket;
		}
	}
	if (!deltaFrame) playerState++;
}

void NetworkedGame::UpdateMinimumState() {
	//Periodically remove old data from the server
	int minID = INT_MAX;
	int maxID = 0; //we could use this to see if a player is lagging behind?

	for (auto i : stateIDs) {
		minID = std::min(minID, i.second);
		maxID = std::max(maxID, i.second);
	}
	//every client has acknowledged reaching at least state minID
	//so we can get rid of any old states!
	std::vector<PaintballGameObject*>::const_iterator first;
	std::vector<PaintballGameObject*>::const_iterator last;
	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		NetworkObject* o = (*i)->GetNetworkObject();
		if (!o) {
			continue;
		}
		o->UpdateStateHistory(minID); //clear out old states so they arent taking up memory...
	}
}

void NetworkedGame::SpawnPlayer() {

}

void NetworkedGame::StartLevel() {
	GameManager::GetInstance().InitWorld(1);

	/*
	std::vector<PaintballGameObject*>::const_iterator first;
	std::vector<PaintballGameObject*>::const_iterator last;
	world->GetObjectIterators(first, last);

	for (auto i = first; i != last; ++i) {
		if ((*i)->GetType() != GameObjectType::bullet)
		{
			NetworkObject* obj = new NetworkObject(**i, G1.GetNetworkObjects().size());
			(*i)->SetNetworkObject(obj);
			G1.GetNetworkObjects().find(G1.GetNetworkObjects().size()] = obj;
			//std::cout<< G1.GetNetworkObjects().size() <<std::endl;
		}
	}
	*/
}

void NetworkedGame::ReceivePacket(int type, GamePacket* payload, int source) {
	//std::cout << "Received type[" << type << "] packet from source: " << source << std::endl;
	//return;

	switch (type) {
	case Delta_State: {
		if (isDebug) std::cout << "Received delta packet from source: " << source << std::endl;
		DeltaPacket* deltaPacket = (DeltaPacket*)payload;

		int objectID = deltaPacket->objectID;

		auto it = G1.GetNetworkObjects().find(objectID);
		if (it != G1.GetNetworkObjects().end()) {
			it->second->ReadPacket(*deltaPacket);
		}

		break;
	}
	case Full_State: {
		if (isDebug) std::cout << "Received full packet from source: " << source << std::endl;
		FullPacket* fullPacket = (FullPacket*)payload;

		int objectID = fullPacket->objectID;
		/*
		rp3d::Vector3 position = fullPacket->fullState.position;
		rp3d::Quaternion orientation = fullPacket->fullState.orientation;
		int stateID = fullPacket->fullState.stateID;
		*/

		auto it = G1.GetNetworkObjects().find(objectID);
		if (it != G1.GetNetworkObjects().end()) {
			it->second->ReadPacket(*fullPacket);
		}
		else {
			NetworkState state = fullPacket->fullState;
			std::cout << "add " << objectID << std::endl;
			GameManager::GetInstance().AddObject(static_cast<GameObjectType>(state.type),
				state.position, state.scale, state.orientation,
				 state.color, nullptr, state.mass, state.isEnemy, state.oriV3, objectID);
		}
		//networkObjects[

		break;
	}
	case String_Message: {
		StringPacket* realPacket = (StringPacket*)payload;
		std::string msg = realPacket->GetStringFromData();
		std::cout << "Received message: \"" << msg << "\" from " << source << std::endl;
		break;
	}
	case Received_State: {
		if (isDebug) std::cout << "Received Received packet from source: " << source << std::endl;
		ClientPacket* clientPacket = (ClientPacket*)payload;

		/*
		if (clientPacket->buttonstates[0] == 1)std::cout << "W" << std::endl;;
		if (clientPacket->buttonstates[1] == 1)std::cout << "A" << std::endl;
		if (clientPacket->buttonstates[2] == 1)std::cout << "S" << std::endl;
		if (clientPacket->buttonstates[3] == 1)std::cout << "D" << std::endl;
		if (clientPacket->buttonstates[4] == 1)std::cout << "SPACE" << std::endl;

		std::cout << "camFront = " << clientPacket->camFront[0] << ", " << clientPacket->camFront[1] << ", " << clientPacket->camFront[2] << std::endl;
		*/


		break;
	}
	case None: {
		if (isDebug) std::cout << "Received None packet from source: " << source << std::endl;
		break;
	}
	default: {
		if (isDebug) std::cout << "Received unknown packet type: " << type << " from source: " << source << std::endl;
		break;
	}
	}
}

void NetworkedGame::OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b) {
	if (thisServer) { //detected a collision between players!
		MessagePacket newPacket;
		newPacket.messageID = COLLISION_MSG;
		newPacket.playerID  = a->GetPlayerNum();

		thisClient->SendPacket(newPacket);

		newPacket.playerID = b->GetPlayerNum();
		thisClient->SendPacket(newPacket);
	}
}

void NetworkedGame::SendPacketTest() {
	if (thisClient) {
		std::cout << "Client : SendPacketTest()  # successful" << std::endl;
		ClientPacket newPacket;
		newPacket.type = Received_State;

		newPacket.buttonstates[0] = 1;
		newPacket.lastID = 0; //You'll need to work this out somehow...

		thisClient->SendPacket(newPacket);
		this->thisClient->UpdateClient();
	}
	else
	{
		std::cout << "Client : SendPacketTest()  # thisClient = nullptr" << std::endl;
	}

	if (thisServer) {
		std::cout << "Server : SendPacketTest()  # successful" << std::endl;
		StringPacket newPacket("A StringPacket");
		thisServer->SendGlobalPacket(newPacket);
		this->thisServer->UpdateServer();
	}
	else
	{
		std::cout << "Server : SendPacketTest()  # thisServer = nullptr" << std::endl;
	}
}

void NetworkedGame::SendPacketTest(std::string s) {
	if (thisClient) {
		std::cout << "Client : SendPacketTest() - " << s << std::endl;
		StringPacket newPacket(s);
		thisClient->SendPacket(newPacket);
		this->thisClient->UpdateClient();
	}
	else
	{
		std::cout << "Client : SendPacketTest() - " << s << "  # thisClient = nullptr" << std::endl;
	}

	if (thisServer) {
		std::cout << "Server : SendPacketTest() - " << s << std::endl;
		StringPacket newPacket(s);
		thisServer->SendGlobalPacket(newPacket);
		this->thisServer->UpdateServer();
	}
	else
	{
		std::cout << "Server : SendPacketTest() - " << s << "  # thisServer = nullptr" << std::endl;
	}
}