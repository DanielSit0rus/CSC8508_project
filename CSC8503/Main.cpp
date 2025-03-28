#include "Main.h"

using namespace NCL;
using namespace CSC8503;



int main() {
	SLSystem::GetInstance().Init();
	AudioSystem::GetInstance().Init();

	WindowInitialisation initInfo;
	initInfo.width		= 1280;
	initInfo.height		= 720;
	initInfo.windowTitle = "CSC8508!";

	Window*w = Window::CreateGameWindow(initInfo);

	if (!w->HasInitialised()) {
		return -1;
	}	

	w->SetWindowPosition(20, 40);
	Console::GetInstance().Init(w);

	NetworkedGame* g = new NetworkedGame();
	//PushdownMachine machine(new GameScreen(g, w));
	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	//TestPathfinding();
	//TestNetworking();
	
	std::thread console([] {Console::GetInstance().ProcessInput(); });

	while (w->UpdateWindow() && GameManager::GetInstance().GetGameState() != EXIT) {
		//std::cout << "[DrawMenu] State = " << (int)GameManager::GetInstance().GetGameState() << std::endl;

		float dt = w->GetTimer().GetTimeDeltaSeconds();
		//TestStateMachine();
		//DisplayPathfinding();
		if (dt > 0.1f) {
			//std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::F3)) {
			Console::GetInstance().ShowConsole();
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		g->UpdateGame(dt);

		AudioSystem::GetInstance().Update();
	}
	EventManager::Trigger(EventType::MainMenu_Quit);
	
	Console::GetInstance().Release();
	console.detach();

	Window::DestroyGameWindow();

	return 0;
}