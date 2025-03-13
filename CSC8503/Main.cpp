#include "Main.h"

using namespace NCL;
using namespace CSC8503;

void TestBehaviourTree() {
	float behaviourTimer;
	float distanceToTarget;
	BehaviourAction* findKey = new BehaviourAction("Find Key", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			std::cout << "\n Looking for a key !\n ";
			behaviourTimer = rand() % 100;
			state = Ongoing;

		}
		else if (state == Ongoing) {
			behaviourTimer -= dt;
			if (behaviourTimer <= 0.0f) {
				std::cout << "\n Found a key !\n ";
				return Success;

			}

		}
		return state; // will be ?ongoing ?until success
		}
	);
	BehaviourAction* goToRoom = new BehaviourAction("Go To Room", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			std::cout << "\n Going to the loot room ! \n";
			state = Ongoing;

		}
		else if (state == Ongoing) {
			distanceToTarget -= dt;
			if (distanceToTarget <= 0.0f) {
				std::cout << "\n Reached room !\n ";
				return Success;
			}

		}
		return state; // will be ?ongoing ?until success
		}
	);

	BehaviourAction* openDoor = new BehaviourAction("Open Door", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			std::cout << "\n Opening Door !\n ";
			return Success;

		}
		return state;
		}
	);

	BehaviourAction* lookForTreasure = new BehaviourAction("Look For Treasure", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			std::cout << "\n Looking for treasure ! \n ";
			return Ongoing;

		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << "\n I found some treasure ! \n ";
				return Success;

			}
			std::cout << "\n No treasure in here ...\n ";
			return Failure;

		}
		return state;
		}
	);

	BehaviourAction* lookForItems = new BehaviourAction("Look For Items", [&](float dt, BehaviourState state) -> BehaviourState {
		if (state == Initialise) {
			std::cout << " Looking for items !\n ";
			return Ongoing;

		}
		else if (state == Ongoing) {
			bool found = rand() % 2;
			if (found) {
				std::cout << " I found some items !\n ";
				return Success;

			}
			std::cout << " No items in here ...\n ";
			return Failure;

		}
		return state;
		}
	);

	BehaviourSequence* sequence = new BehaviourSequence("Room Sequence");
	sequence->AddChild(findKey);
	sequence->AddChild(goToRoom);
	sequence->AddChild(openDoor);
	BehaviourSelector* selection = new BehaviourSelector("Loot Selection");
	selection->AddChild(lookForTreasure);
	selection->AddChild(lookForItems);
	BehaviourSequence* rootSequence = new BehaviourSequence("Root Sequence");
	rootSequence->AddChild(sequence);
	rootSequence->AddChild(selection);

	for (int i = 0; i < 5; ++i) {
		rootSequence->Reset();
		behaviourTimer = 0.0f;
		distanceToTarget = rand() % 250;
		BehaviourState state = Ongoing;
		std::cout << " We are going on an adventure ! \n ";
		while (state == Ongoing) {
			state = rootSequence->Execute(1.0f); // fake dt

		}
		if (state == Success) {
			std::cout << " What a successful adventure ! \n ";

		}
		else if (state == Failure) {
			std::cout << " What a waste of time ! \n ";

		}

	}
	std::cout << " All done ! \n ";
}

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

	w->SetWindowPosition(5, 30);

	NetworkedGame* g = new NetworkedGame();
	//PushdownMachine machine(new GameScreen(g, w));
	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	//TestPathfinding();
	//TestNetworking();
	
	Console::GetInstance().Init(w);
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