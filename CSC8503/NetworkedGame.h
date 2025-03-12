#pragma once
#include "TutorialGame.h"
#include "NetworkBase.h"

namespace NCL {
	namespace CSC8503 {
		class GameServer;
		class GameClient;
		class NetworkPlayer;

		class NetworkedGame : public TutorialGame, public PacketReceiver {
		public:
			NetworkedGame();
			~NetworkedGame();

			void StartAsServer();
			void StartAsClient(char a, char b, char c, char d);
			void RequestStart(int arg) { toStart = arg; }

			void UpdateGame(float dt) override;

			void SpawnPlayer();

			void StartLevel();

			void ReceivePacket(int type, GamePacket* payload, int source) override;

			void OnPlayerCollision(NetworkPlayer* a, NetworkPlayer* b);

			void SetFrame(unsigned int f) { frame = f; }

		protected:
			void UpdateAsServer(float dt);
			void UpdateAsClient(float dt);
			int ip1, ip2, ip3, ip4;

			void BroadcastSnapshot(bool deltaFrame);
			void UpdateMinimumState();
			std::map<int, int> stateIDs;

			unsigned int frame = 40;

			GameServer* thisServer;
			GameClient* thisClient;
			float timeToNextPacket;
			int packetsToSnapshot;

			std::map<int, PaintballGameObject*> serverPlayers;
			PaintballGameObject* localPlayer;
			int playerState = 0;

			bool isDebug = false;
			void SendPacketTest();
			void SendPacketTest(std::string s);

			int toStart = -1;	//0 for server, 1 for client
		};
	}
}

