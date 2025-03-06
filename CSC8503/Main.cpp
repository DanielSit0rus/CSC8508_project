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


class PauseScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
	PushdownState * *newState) override {
		if (Window::GetKeyboard() -> KeyPressed(KeyCodes::U)) {
			return PushdownResult::Pop;
			
		}
		return PushdownResult::NoChange;
		
	}
	void OnAwake() override {
		std::cout << "Press U to unpause game ! \n";
	}
	
};

class GameScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
	PushdownState * *newState) override {
		pauseReminder -= dt;
		if (pauseReminder < 0) {
			std::cout << " Coins mined : " << coinsMined << " \n ";
			std::cout << " Press P to pause game , or F1 to return to main menu ! \n " ;
			pauseReminder += 1.0f;
		}
		if (Window::GetKeyboard() -> KeyDown(KeyCodes::P)) {
			*newState = new PauseScreen();
			return PushdownResult::Push;
			
		}
		if (Window::GetKeyboard() -> KeyDown(KeyCodes::F1)) {
			std::cout << " Returning to main menu !\n ";
			return PushdownResult::Pop;
			
		}
		if (rand() % 7 == 0) {
			coinsMined++;
			
		}
		return PushdownResult::NoChange;
		
	};
	void OnAwake() override {
		std::cout << "Preparing to mine coins ! \n";
		
	}
    protected:
	int coinsMined = 0;
	float pauseReminder = 1;
};

class IntroScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
	PushdownState** newState) override {
		std::cout << "\n Checking for input...\n";
		if (Window::GetKeyboard() ->KeyDown(KeyCodes::SPACE)) {
			std::cout << "\n space pressed...\n";
			*newState = new GameScreen();
			return PushdownResult::Push;
			
		}
		if (Window::GetKeyboard() ->KeyPressed(KeyCodes::ESCAPE)) {
			return PushdownResult::Pop;
			
		}
		return PushdownResult::NoChange;
	};

	void OnAwake() override {
		std::cout << " Welcome to a really awesome game ! \n ";
		std::cout << " Press Space To Begin or escape to quit ! \n ";
		
	}
	
};

void TestPushdownAutomata(Window* w) {
	PushdownMachine machine(new IntroScreen());
	while (w -> UpdateWindow()) {
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		if (!machine.Update(dt)) {
			return;
			
		}
	}
}

/*

The main function should look pretty familar to you!
We make a window, and then go into a while loop that repeatedly
runs our 'game' until we press escape. Instead of making a 'renderer'
and updating it, we instead make a whole game, and repeatedly update that,
instead. 

This time, we've added some extra functionality to the window class - we can
hide or show the 

*/

bool paused = false;
bool reset = false;
float elapsedTime = 120;
//FMOD
bool canStart_FMOD = true;

class pauseScreen : public PushdownState {
public:
	pauseScreen(Window* window) {
		w = window;
	}
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {

		Debug::Print("PAUSED press U to unpause", Vector2(5, 10));


		if (Window::GetKeyboard()->KeyPressed(KeyCodes::U)) {
			paused = false;
			return PushdownResult::Pop;

		}
		return PushdownResult::NoChange;

	}
	void OnAwake() override {
		paused = true;

		w->ShowOSPointer(true);
		w->LockMouseToWindow(false);
	}
	void OnSleep() override {
		w->ShowOSPointer(false);
		w->LockMouseToWindow(true);
	}
private:
	Window* w;
};

class WinScreen : public PushdownState {
	PushdownResult OnUpdate(float dt,
		PushdownState** newState) override {

		Debug::Print("You Win, Press R to play again or ESC to quit", Vector2(5, 10), Debug::GREEN);


		if (Window::GetKeyboard()->KeyDown(KeyCodes::R)) {
			elapsedTime = 120;
			canStart_FMOD = true;
			//pause = false;
			reset = true;
			return PushdownResult::Pop;
			//ResetGame();
		}

		return PushdownResult::NoChange;

	}
	void OnAwake() override {
		paused = true;

	}

};

class gameOverScreen : public PushdownState {
	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		//std::cout << "Game Over! Press R to Retry or ESC to Exit.\n";
		Debug::Print("Game Over! Press R to Retry or ESC to Quit", Vector2(5, 10), Debug::RED);

		if (Window::GetKeyboard()->KeyDown(KeyCodes::R)) {
			elapsedTime = 120;
			canStart_FMOD = true;
			//pause = false;
			reset = true;
			return PushdownResult::Pop;
			//ResetGame();
		}

		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		paused = true;
		//std::cout << "Game Over! Thank you for playing.\n";
	}
};


class gameScreen : public PushdownState {
public:
	gameScreen(Window* window) {
		w = window;
	}

	PushdownResult OnUpdate(float dt, PushdownState** newState) override {
		//reset = false;
		pauseReminder -= dt;
		elapsedTime -= dt;
		if (pauseReminder < 0) {
			//std::cout << "\n Press P to pause game\n";
			pauseReminder += 1.0f;
		}
		Debug::Print("Starting Game, timer: " + std::to_string(elapsedTime), Vector2(5, 15));

		//FMOD
		if (canStart_FMOD && elapsedTime < 118.5f && elapsedTime > 115) {
			AudioSystem::GetInstance().TriggerEvent("event:/Felicia/Start2");
			canStart_FMOD = false;
		}

		// Pause state transition
		if (Window::GetKeyboard()->KeyDown(KeyCodes::P)) {
			*newState = new pauseScreen(w);
			return PushdownResult::Push;
		}
		return PushdownResult::NoChange;
	}

	void OnAwake() override {
		paused = false;
		//reset = false;
		//std::cout << "Preparing to mine coins!\n";
	}
protected:
	//int coinsMined = 0;
	float pauseReminder = 1.0f;
	Window* w;
};

int main() {
	SLSystem::GetInstance().Init();
	AudioSystem::GetInstance().Init();

	WindowInitialisation initInfo;
	initInfo.width		= 1280;
	initInfo.height		= 720;
	initInfo.windowTitle = "CSC8508!";

	Window*w = Window::CreateGameWindow(initInfo);
	//TestPushdownAutomata(w);
	//TestBehaviourTree();

	if (!w->HasInitialised()) {
		return -1;
	}	

	w->SetWindowPosition(5, 30);

	NetworkedGame* g = new NetworkedGame();
	PushdownMachine* PushMachine = new PushdownMachine(new gameScreen(w));
	w->GetTimer().GetTimeDeltaSeconds(); //Clear the timer so we don't get a larget first dt!
	//TestPathfinding();
	//TestNetworking();
	
	Console::GetInstance().Init(w);
	std::thread console([] {Console::GetInstance().ProcessInput(); });

	EventManager::Trigger(EventType::Game_Start);

	while (w->UpdateWindow() && !Window::GetKeyboard()->KeyDown(KeyCodes::ESCAPE)) {
		float dt = w->GetTimer().GetTimeDeltaSeconds();
		//TestStateMachine();
		//DisplayPathfinding();
		if (dt > 0.1f) {
			//std::cout << "Skipping large time delta" << std::endl;
			continue; //must have hit a breakpoint or something to have a 1 second frame time!
		}

		if (Window::GetKeyboard()->KeyPressed(KeyCodes::TAB)) {
			Console::GetInstance().ShowConsole();
		}

		w->SetTitle("Gametech frame time:" + std::to_string(1000.0f * dt));

		PushMachine->Update(dt);
		if (paused)
		{
			g->pauseGame();
		}
		if (!paused)
		{
			g->UnpauseGame();
		}
		if (reset)
		{
			g = new NetworkedGame();
			reset = false;
		}

		g->UpdateGame(dt);

		AudioSystem::GetInstance().Update();
	} 
	EventManager::Trigger(EventType::Game_End);

	console.detach();

	Window::DestroyGameWindow();
}