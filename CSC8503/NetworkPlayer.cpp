#include "NetworkPlayer.h"
#include "NetworkedGame.h"

using namespace NCL;
using namespace CSC8503;

NetworkPlayer::NetworkPlayer(NetworkedGame* game, int num)
	:PaintballGameObject(type = GameObjectType::test_networkPlayer, name)
{
	this->game = game;
	playerNum  = num;
}

NetworkPlayer::~NetworkPlayer() {

}