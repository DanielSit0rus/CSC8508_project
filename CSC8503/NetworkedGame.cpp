#include "NetworkedGame.h"
#include "NetworkPlayer.h"
#include "NetworkObject.h"
#include "GameServer.h"
#include "GameClient.h"
#include "SLSystem.h"

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

	EventManager::Subscribe(EventType::Network_StartAsServer, [this]() {RequestStart(0); });
	EventManager::Subscribe(EventType::Network_StartAsClient, [this](std::string& s) {
		std::istringstream stream(s);
		stream >> ip1; stream >> ip2; stream >> ip3; stream >> ip4;
		RequestStart(1);
		});
	EventManager::Subscribe(EventType::Network_Test, [this]() {SendPacketTest(); });
	EventManager::Subscribe(EventType::Network_Test, [this](std::string& s) {SendPacketTest(s); });

	EventManager::Subscribe(EventType::Game_End, [this]()
		{
			if (thisServer) {
				delete thisServer;
				thisServer = nullptr;
			}
			if (thisClient) {
				delete thisClient;
				thisClient = nullptr;
			}
			G1.SetNet(false);
		});

	EventManager::Subscribe(EventType::Network_Connected, [&](int arg) {
		IntPacket newPacket(arg);
		thisServer->SendGlobalPacket(newPacket);
		this->thisServer->UpdateServer();
		});
}

NetworkedGame::~NetworkedGame()	{
	delete thisServer;
	delete thisClient;
}

void NetworkedGame::StartAsServer() {
	if (thisServer || thisClient) return;

	thisServer = new GameServer(NetworkBase::GetDefaultPort(), 8);

	thisServer->RegisterPacketHandler(Received_State, this);
	thisServer->RegisterPacketHandler(String_Message, this);

	std::cout << "[NetworkedGame.cpp]Start As Server" << std::endl;

	G1.SetNet(true);

	StartLevel();
}

void NetworkedGame::StartAsClient(char a, char b, char c, char d) {
	if (thisServer || thisClient) return;

	thisClient = new GameClient();

	bool isConnected = thisClient->Connect(a, b, c, d, NetworkBase::GetDefaultPort());
	if (isConnected) std::cout << "[NetworkedGame.cpp]Connecting" << std::endl;
	else {
		std::cout << "[NetworkedGame.cpp]Error in connection" << std::endl;
		return;
	}

	thisClient->RegisterPacketHandler(Delta_State, this);
	thisClient->RegisterPacketHandler(Full_State, this);
	thisClient->RegisterPacketHandler(Player_Connected, this);
	thisClient->RegisterPacketHandler(Player_Disconnected, this);
	thisClient->RegisterPacketHandler(String_Message, this);

	GameManager::GetInstance().EnablePhys(false);
	G1.SetNet(true);

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
	if (toStart == 0) {
		StartAsServer();
		toStart = -1;
	}
	else if (toStart == 1) {
		StartAsClient(ip1, ip2, ip3, ip4);
		toStart = -1;
	}

	TutorialGame::UpdateGame(dt);
	for (const auto& pair : canfronts) {
		if(G1.GetNetworkPlayers()[pair.first]) G1.GetNetworkPlayers()[pair.first]->UpdatePlayerRotation(pair.second);
	}
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

	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM1)) {
		G1.GetNetworkPlayers()[-1]->SwitchWeapon(WeaponType::RedGun);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM2)) {
		G1.GetNetworkPlayers()[-1]->SwitchWeapon(WeaponType::GreenGun);
	}
	if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM3)) {
		G1.GetNetworkPlayers()[-1]->SwitchWeapon(WeaponType::BlueGun);
	}

	for (auto it = timers.begin(); it != timers.end(); ++it) {
		it->second -= dt;
	}
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
	newPacket.buttonstates[5] = Window::GetKeyboard()->KeyDown(KeyCodes::Q) ? 1 : 0;
	newPacket.buttonstates[6] = Window::GetKeyboard()->KeyDown(KeyCodes::NUM1) ? 1 : 0;
	newPacket.buttonstates[7] = Window::GetKeyboard()->KeyDown(KeyCodes::NUM2) ? 1 : 0;
	newPacket.buttonstates[8] = Window::GetKeyboard()->KeyDown(KeyCodes::NUM3) ? 1 : 0;


	newPacket.camFront= GameManager::GetInstance().GetCameraFront();
	newPacket.camPos = GameManager::GetInstance().GetMainCamera().GetPosition();

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

void NetworkedGame::StartLevel() {
	if (thisServer) G1.RequestRebuildWorld(192);
	else if (thisClient) G1.CleanWorld();

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
			//std::cout << "add " << objectID << std::endl;
			GameObjectType type = static_cast<GameObjectType>(state.type);
			if (type == GameObjectType::player) {
				if (state.playerID == -2) break;	//error data

				PaintballPlayer* obj = (PaintballPlayer*)G1.AddPlayerClass(state.position);

				NetworkObject* netObj = new NetworkObject(*obj, G1.GetNetworkObjects().size());
				G1.GetNetworkObjects()[G1.GetNetworkObjects().size()] = netObj;
				obj->SetNetworkObject(netObj);

				G1.GetNetworkPlayers()[state.playerID] = obj;

				//std::cout << "addPlayer : thisPeer = " << thisPeer << std::endl;
				//G1.GetNetworkPlayers()[thisPeer] = obj;
			}
			else
			{
				if (state.meshID == -1)break;
				G1.AddObject(type,
					state.position, state.scale, state.orientation,
					state.color,
					ResourceManager::GetInstance().GetResourceByID("mesh",state.meshID),
					ResourceManager::GetInstance().GetResourceByID("texture", state.textureID_D),
					ResourceManager::GetInstance().GetResourceByID("texture", state.textureID_S),
					ResourceManager::GetInstance().GetResourceByID("texture", state.textureID_N),
					ResourceManager::GetInstance().GetResourceByID("texture", state.textureID_H),
					ResourceManager::GetInstance().GetResourceByID("texture", state.textureID_M),
					ResourceManager::GetInstance().GetResourceByID("texture", state.textureID_O),
					ResourceManager::GetInstance().GetResourceByID("texture", state.textureID_MM),
					ResourceManager::GetInstance().GetResourceByID("texture", state.textureID_R),
					ResourceManager::GetInstance().GetResourceByID("shader", state.shaderID),
					state.mass, state.isEnemy, state.oriV3, objectID);
			}
		}
		G1.SetLeftTime(fullPacket->fullState.leftTime);
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

		rp3d::Vector3 dir(0, 0, 0);

		if (clientPacket->buttonstates[0] == 1) dir[0] += 1;	//W
		if (clientPacket->buttonstates[1] == 1) dir[1] -= 1;	//A
		if (clientPacket->buttonstates[2] == 1) dir[0] -= 1;	//S	
		if (clientPacket->buttonstates[3] == 1) dir[1] += 1;	//D
		if (clientPacket->buttonstates[4] == 1)	dir[2] += 1;	//SPACE

		if (clientPacket->buttonstates[6] == 1) G1.GetNetworkPlayers()[source]->SwitchWeapon(WeaponType::RedGun);
		if (clientPacket->buttonstates[7] == 1) G1.GetNetworkPlayers()[source]->SwitchWeapon(WeaponType::GreenGun);
		if (clientPacket->buttonstates[8] == 1) G1.GetNetworkPlayers()[source]->SwitchWeapon(WeaponType::BlueGun);

		inputManager.HandleGameInput(dir, source, clientPacket->camFront);
		//std::cout << "camFront = " << clientPacket->camFront[0] << ", " << clientPacket->camFront[1] << ", " << clientPacket->camFront[2] << std::endl;

		if (clientPacket->buttonstates[5] == 1 && timers[source] < 0)
		{
			G1.GetNetworkPlayers()[source]->Attack(clientPacket->camFront,
				G1.GetNetworkPlayers()[source]->GetCurrentWeaponColor());
			timers[source] = attackCD;
		}
		canfronts[source] = clientPacket->camFront;
		//G1.GetNetworkPlayers()[source]->UpdatePlayerRotation(clientPacket->camFront);

		break;
	}
	case Player_Connected: {
		IntPacket* realPacket = (IntPacket*)payload;
		if (G1.GetThisPeer() == -1) G1.GetThisPeer() = realPacket->num;
		std::cout << "Received thisPeer: \"" << G1.GetThisPeer() << "\" from " << source << std::endl;
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