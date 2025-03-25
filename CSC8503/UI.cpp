#define STB_IMAGE_IMPLEMENTATION
#include "TutorialGame.h"
#include "NetworkedGame.h"
#include "Win32Window.h"
#include "UI.h"
#include "OGLRenderer.h"
#include "OGLTexture.h"
#include "GameTechRenderer.h"
#include "stb_image_aug.h"
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_opengl3.h"
#include <Windows.h>

#include "EventManager.h"
#include "SLSystem.h"

using namespace NCL;
using namespace CSC8503;
using namespace Win32Code;
using namespace std;

// Forward declare message handler from imgui_impl_win32.cpp
extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

void ImguiProcessInput(auto* data) {
	ImGuiIO& io = ImGui::GetIO();
	RAWINPUT* rawInput = (RAWINPUT*)data;

	static bool debugmode = false;

	if (rawInput->header.dwType == RIM_TYPEMOUSE)
	{
		static int btDowns[5] = { RI_MOUSE_BUTTON_1_DOWN,
								  RI_MOUSE_BUTTON_2_DOWN,
								  RI_MOUSE_BUTTON_3_DOWN,
								  RI_MOUSE_BUTTON_4_DOWN,
								  RI_MOUSE_BUTTON_5_DOWN };

		static int btUps[5] = { RI_MOUSE_BUTTON_1_UP,
								RI_MOUSE_BUTTON_2_UP,
								RI_MOUSE_BUTTON_3_UP,
								RI_MOUSE_BUTTON_4_UP,
								RI_MOUSE_BUTTON_5_UP };

		for (int i = 0; i < 5; ++i) {
			if (rawInput->data.mouse.usButtonFlags & btDowns[i]) {
				io.MouseDown[i] = true;
			}
			else if (rawInput->data.mouse.usButtonFlags & btUps[i]) {
				io.MouseDown[i] = false;
			}
		}
	}
	else if (rawInput->header.dwType == RIM_TYPEKEYBOARD)
	{
		USHORT key = rawInput->data.keyboard.VKey;
		bool down = !(rawInput->data.keyboard.Flags & RI_KEY_BREAK);

		if (key < 256) {
			switch (key) {
			case VK_TAB: io.AddKeyEvent(ImGuiKey_Tab, down); break;
			case VK_LEFT: io.AddKeyEvent(ImGuiKey_LeftArrow, down); break;
			case VK_RIGHT: io.AddKeyEvent(ImGuiKey_RightArrow, down); break;
			case VK_UP: io.AddKeyEvent(ImGuiKey_UpArrow, down); break;
			case VK_DOWN: io.AddKeyEvent(ImGuiKey_DownArrow, down); break;
			case VK_PRIOR: io.AddKeyEvent(ImGuiKey_PageUp, down); break;
			case VK_NEXT: io.AddKeyEvent(ImGuiKey_PageDown, down); break;
			case VK_HOME: io.AddKeyEvent(ImGuiKey_Home, down); break;
			case VK_END: io.AddKeyEvent(ImGuiKey_End, down); break;
			case VK_INSERT: io.AddKeyEvent(ImGuiKey_Insert, down); break;
			case VK_DELETE: io.AddKeyEvent(ImGuiKey_Delete, down); break;
			case VK_BACK: io.AddKeyEvent(ImGuiKey_Backspace, down); break;
			case VK_SPACE: io.AddKeyEvent(ImGuiKey_Space, down); break;
			case VK_RETURN: io.AddKeyEvent(ImGuiKey_Enter, down); break;
			case VK_ESCAPE: io.AddKeyEvent(ImGuiKey_Escape, down); break;
			case VK_OEM_7: io.AddKeyEvent(ImGuiKey_Apostrophe, down); break;
			case VK_OEM_COMMA: io.AddKeyEvent(ImGuiKey_Comma, down); break;
			case VK_OEM_MINUS: io.AddKeyEvent(ImGuiKey_Minus, down); break;
			case VK_OEM_PERIOD: io.AddKeyEvent(ImGuiKey_Period, down); break;
			case VK_OEM_2: io.AddKeyEvent(ImGuiKey_Slash, down); break;
			case VK_OEM_1: io.AddKeyEvent(ImGuiKey_Semicolon, down); break;
			case VK_OEM_PLUS: io.AddKeyEvent(ImGuiKey_Equal, down); break;
			case VK_OEM_4: io.AddKeyEvent(ImGuiKey_LeftBracket, down); break;
			case VK_OEM_5: io.AddKeyEvent(ImGuiKey_Backslash, down); break;
			case VK_OEM_6: io.AddKeyEvent(ImGuiKey_RightBracket, down); break;
			case VK_OEM_3: io.AddKeyEvent(ImGuiKey_GraveAccent, down); break;
			case VK_CAPITAL: io.AddKeyEvent(ImGuiKey_CapsLock, down); break;
			case VK_SCROLL: io.AddKeyEvent(ImGuiKey_ScrollLock, down); break;
			case VK_NUMLOCK: io.AddKeyEvent(ImGuiKey_NumLock, down); break;
			case VK_SNAPSHOT: io.AddKeyEvent(ImGuiKey_PrintScreen, down); break;
			case VK_PAUSE: io.AddKeyEvent(ImGuiKey_Pause, down); break;
			case VK_NUMPAD0: io.AddKeyEvent(ImGuiKey_Keypad0, down); break;
			case VK_NUMPAD1: io.AddKeyEvent(ImGuiKey_Keypad1, down); break;
			case VK_NUMPAD2: io.AddKeyEvent(ImGuiKey_Keypad2, down); break;
			case VK_NUMPAD3: io.AddKeyEvent(ImGuiKey_Keypad3, down); break;
			case VK_NUMPAD4: io.AddKeyEvent(ImGuiKey_Keypad4, down); break;
			case VK_NUMPAD5: io.AddKeyEvent(ImGuiKey_Keypad5, down); break;
			case VK_NUMPAD6: io.AddKeyEvent(ImGuiKey_Keypad6, down); break;
			case VK_NUMPAD7: io.AddKeyEvent(ImGuiKey_Keypad7, down); break;
			case VK_NUMPAD8: io.AddKeyEvent(ImGuiKey_Keypad8, down); break;
			case VK_NUMPAD9: io.AddKeyEvent(ImGuiKey_Keypad9, down); break;
			case VK_DECIMAL: io.AddKeyEvent(ImGuiKey_KeypadDecimal, down); break;
			case VK_DIVIDE: io.AddKeyEvent(ImGuiKey_KeypadDivide, down); break;
			case VK_MULTIPLY: io.AddKeyEvent(ImGuiKey_KeypadMultiply, down); break;
			case VK_SUBTRACT: io.AddKeyEvent(ImGuiKey_KeypadSubtract, down); break;
			case VK_ADD: io.AddKeyEvent(ImGuiKey_KeypadAdd, down); break;
			default:
				if (key >= '0' && key <= '9') io.AddKeyEvent((ImGuiKey)(ImGuiKey_0 + (key - '0')), down);
				else if (key >= 'A' && key <= 'Z') io.AddKeyEvent((ImGuiKey)(ImGuiKey_A + (key - 'A')), down);
				break;
			}
		}
		if (key == VK_CONTROL) io.AddKeyEvent(ImGuiKey_ModCtrl, down);
		if (key == VK_SHIFT) io.AddKeyEvent(ImGuiKey_ModShift, down);
		if (key == VK_MENU) io.AddKeyEvent(ImGuiKey_ModAlt, down);
	}
}

bool LoadTextureFromFile(const char* filename, GLuint* out_texture, int* out_width, int* out_height)
{
	int image_width = 0;
	int image_height = 0;
	unsigned char* image_data = stbi_load(filename, &image_width, &image_height, NULL, 4);
	if (image_data == NULL) return false;

	GLuint image_texture;
	glGenTextures(1, &image_texture);
	glBindTexture(GL_TEXTURE_2D, image_texture);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image_width, image_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image_data);
	stbi_image_free(image_data);

	*out_texture = image_texture;
	*out_width = image_width;
	*out_height = image_height;

	return true;
}

UI::UI(PaintballGameWorld* world)
{
	this->world = world;


	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	Win32Code::Win32Window* win32 = (Win32Code::Win32Window*)Window::GetWindow();
	win32->SetExtraMsgFunc(ImguiProcessInput);

	// Initialize Platform/Renderer backends
	ImGui_ImplWin32_Init(win32->GetHandle());
	ImGui_ImplOpenGL3_Init("#version 130");

	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // 启用键盘导航
	io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;             // 确保鼠标输入被启用
	io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange; // 允许ImGui修改鼠标光标

	titlefont = io.Fonts->AddFontFromFileTTF((Assets::FONTSSDIR + "ProggyTiny.ttf").c_str(), 35);
	menufont = io.Fonts->AddFontFromFileTTF((Assets::FONTSSDIR + "ProggyTiny.ttf").c_str(), 30);
	infofont = io.Fonts->AddFontFromFileTTF((Assets::FONTSSDIR + "TreeBold.ttf").c_str(), 30);
	normalfont = io.Fonts->AddFontFromFileTTF((Assets::FONTSSDIR + "KarlaRegular.ttf").c_str(), 30);
	bigfont = io.Fonts->AddFontFromFileTTF((Assets::FONTSSDIR + "ProggyTiny.ttf").c_str(), 45);

	IM_ASSERT(LoadTextureFromFile((Assets::UIDIR + "landscape.png").c_str(), &loading.img_texture, &loading.img_width, &loading.img_height));
	IM_ASSERT(LoadTextureFromFile((Assets::UIDIR + "menu.png").c_str(), &menu.img_texture, &menu.img_width, &menu.img_height));

	ImGui::StyleColorsDark();

	loadingstep = 0;

	// Load saved server list
	LoadServerList();
	InitializeMainMenuButtons();
	InitializeSettingMenuButtons();
	InitializePauseMenuButtons();
	InitializeChooseServerMenuButtons();
	InitializeFinishButtons();
    InitializeFailureButtons();

	AudioSystem::GetInstance().GetBusVolume("BGM", bgmVolume);
	AudioSystem::GetInstance().GetBusVolume("Effect", effectVolume);
	AudioSystem::GetInstance().GetBusVolume("Voice", voiceVolume);

	const json& j = SLSystem::GetInstance().GetCurSave();
	totalStep = j["resources"]["meshes"].size()
		+ j["resources"]["textures"].size()
		+ j["resources"]["shaders"].size();
	totalStep = totalStep == 0 ? 1 : totalStep;

	
	EventManager::Subscribe(EventType::MouseLeftClick, [this](float a, float b) {PrintMousePos(a, b); });
	EventManager::Subscribe(EventType::MouseLeftClick, [this](float a, float b) {ProcessClickEvent(a, b); });

}

UI::~UI()
{
	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplWin32_Shutdown();
	ImGui::DestroyContext();
}


void UI::Update(float dt)
{
	const ImGuiViewport* vp = ImGui::GetMainViewport();
	// 如果检测到尺寸有效，则初始化按钮（仅在第一次更新时）
	static bool buttonsInitialized = false;
	if (!buttonsInitialized && vp->Size.x > 0) {
		InitializeMainMenuButtons();
		InitializeSettingMenuButtons();
		InitializePauseMenuButtons();
		InitializeChooseServerMenuButtons();
		InitializeFinishButtons();
		InitializeFailureButtons();
		buttonsInitialized = true;
	}

	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();

	ImGuiIO& io = ImGui::GetIO();

	// 处理服务器IP输入
	if (GameManager::GetInstance().GetGameState() == CHOOSESERVER) {
		// HandleServerIPInput();
	}

	// Enable mouse in MENU and SETTING states
	if (GameManager::GetInstance().GetGameState() == MENU ||
		GameManager::GetInstance().GetGameState() == PAUSED ||
		GameManager::GetInstance().GetGameState() == LOADING ||
		GameManager::GetInstance().GetGameState() == SETTING ||
		GameManager::GetInstance().GetGameState() == CHOOSESERVER) {
		//Window::GetWindow()->ShowOSPointer(true);
		//Window::GetWindow()->LockMouseToWindow(false);

		// Enable mouse input
		io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
	}
	else {
		//Window::GetWindow()->ShowOSPointer(false);
		//Window::GetWindow()->LockMouseToWindow(true);
	}
	if (GameManager::GetInstance().GetGameState() == PLAYING) {
		// 在PLAYING状态下，让游戏引擎处理鼠标输入
		io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
		//Window::GetWindow()->LockMouseToWindow(true);  
	}
	else {
		Window::GetWindow()->LockMouseToWindow(false); // 在 UI 界面解锁鼠标
	}
	if (GameManager::GetInstance().GetGameState() == SETTING) {
		ImGui::GetIO().ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
		Window::GetWindow()->LockMouseToWindow(false);  // 释放鼠标
	}


	//if (GameManager::GetInstance().GetGameState() == LOADING) {
	//	loadingstep += dt * 2;
	//	if (loadingstep >= 5) {
	//		GameManager::GetInstance().SetGameState(MENU);
	//	}
	//}

	switch (GameManager::GetInstance().GetGameState())
	{
	case LOADING:
		//std::cout << "[UI::Update] Drawing Loading screen" << std::endl;
		DrawLoading(dt);
		break;
	case MENU:
		//std::cout << "[UI::Update] Drawing Menu screen" << std::endl;
		DrawMenu(dt);
		break;
	case CHOOSESERVER:
		DrawChooseServer(dt);
		break;
	case PLAYING:
		DrawPlayingUI(dt);
		break;
	case SERVERPLAYING:
		DrawServerPlayingUI(dt);
		break;
	case CLIENTPLAYING:
		DrawClientPlayingUI(dt);
		break;
	case SETTING:
		DrawSettingMenu(dt);
		break;
	case PAUSED:
		DrawPausedMenu(dt);
		break;
	case FAILURE:
		DrawFailureMenu(dt);
		break;
	case FINISH:
		DrawFinishMenu(dt);
		break;
	default:
		break;
	}

	//std::cout << "[UI::Update] Current state after processing: " << (int)world->GetGameState() << std::endl;

	ImGui::EndFrame();
	ImGui::Render();
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}


void UI::DrawUI()

{

	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void UI::SetLoadingStep(int step) {

	//std::cout << "[SetLoadingStep] Updating loading step to: " << step << std::endl;
	loadingstep += step;
	AudioSystem::GetInstance().Update();
}

void UI::DrawLoading(float dt) {

	//std::cout << "[DrawLoading] loadingstep = " << loadingstep << ", : " << ((loadingstep + 1) * 20) << "%" << std::endl;
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();


	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y), ImGuiCond_Always);
	ImGui::SetNextWindowBgAlpha(0);

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0);

	if (ImGui::Begin("Background", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground)) {
		ImGui::Image((ImTextureID)(uintptr_t)loading.img_texture, main_viewport->Size, ImVec2(0, 0), ImVec2(1, 1));
		ImGui::End();
	}
	ImGui::PopStyleVar(2);  // 


	ImGui::SetNextWindowPos(ImVec2(main_viewport->GetCenter().x - 175, main_viewport->GetCenter().y + 50));
	ImGui::SetNextWindowSize(ImVec2(350, 40));

	if (ImGui::Begin("LoadingBar", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings)) {
		float progress = (float)loadingstep / totalStep;

		ImGui::ProgressBar(progress, ImVec2(350, 40));
		ImGui::End();
	}


	ImGui::SetNextWindowPos(ImVec2(main_viewport->GetCenter().x - 175, main_viewport->GetCenter().y - 100));
	ImGui::SetNextWindowSize(ImVec2(350, 100));

	if (ImGui::Begin("LoadingText", NULL, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoSavedSettings)) {
		ImGui::PushFont(titlefont);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 0, 255));
		ImGui::SetCursorPos(ImVec2(100, 10));
		ImGui::Text("Game Loading...");
		ImGui::PopFont();
		ImGui::PopStyleColor();

		ImGui::PushFont(normalfont);
		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 0, 0, 255));
		ImGui::SetCursorPos(ImVec2(100, 50));
		ImGui::Text("Team 7");
		ImGui::PopFont();
		ImGui::PopStyleColor();

		ImGui::End();
	}
}



void UI::DrawMenu(float dt) {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y), ImGuiCond_Always);

	// Debug output for viewport and mouse state
	ImGuiIO& io = ImGui::GetIO();


	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Background", NULL, window_flags)) {
		ImVec2 windowSize = main_viewport->Size;
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image((ImTextureID)(uintptr_t)menu.img_texture, main_viewport->Size, ImVec2(0, 0), ImVec2(1, 1));

	}

	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::PushFont(bigfont);

	// Center the title
	float titleWidth = ImGui::CalcTextSize("Main Menu").x;
	ImGui::SetCursorPos(ImVec2(
		(main_viewport->Size.x - titleWidth) * 0.5f,
		main_viewport->Size.y * 0.2f
	));
	ImGui::Text("Main Menu");
	ImGui::PopFont();
	ImGui::PopStyleColor();

	ImGui::PushFont(menufont);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.8f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.9f, 0.9f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	float buttonWidth = 220.0f;
	float buttonHeight = 50.0f;
	float buttonSpacing = 20.0f;
	float startY = main_viewport->Size.y * 0.4f;

	// Center all buttons horizontally
	float centerX = (main_viewport->Size.x - buttonWidth) * 0.5f;

	// Singleplayer button
	ImGui::SetCursorPos(ImVec2(centerX, startY));
	if (ImGui::Button("Singleplayer", ImVec2(buttonWidth, buttonHeight))) {

		GameManager::GetInstance().SetGameState(PLAYING);
	}

	// Multiplayer button
	ImGui::SetCursorPos(ImVec2(centerX, startY + buttonHeight + buttonSpacing));
	if (ImGui::Button("Multiplayer", ImVec2(buttonWidth, buttonHeight))) {

		GameManager::GetInstance().SetGameState(CHOOSESERVER);
	}

	// Settings button
	ImGui::SetCursorPos(ImVec2(centerX, startY + (buttonHeight + buttonSpacing) * 2));
	if (ImGui::Button("Settings", ImVec2(buttonWidth, buttonHeight))) {


		if (world) {
			GameManager::GetInstance().SetGameState(SETTING);
			//std::cout << "[DrawMenu] State changed to: " << (int)GameManager::GetInstance().GetGameState() << std::endl;
		}
	}



	// Exit button
	ImGui::SetCursorPos(ImVec2(centerX, startY + (buttonHeight + buttonSpacing) * 3));
	if (ImGui::Button("Exit", ImVec2(buttonWidth, buttonHeight))) {
		GameManager::GetInstance().SetGameState(EXIT);
		//exit(0);  
	}

	ImGui::PopStyleColor(4);
	ImGui::PopFont();

	ImGui::End();
}

void UI::DrawPlayingUI(float dt) {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

	ImGui::SetMouseCursor(ImGuiMouseCursor_None);

	// Game info window (top-left corner)
	ImGui::SetNextWindowPos(ImVec2(10, 10));
	ImGui::SetNextWindowSize(ImVec2(200, 120));
	ImGui::SetNextWindowBgAlpha(0.7f);

	if (ImGui::Begin("Game Info", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar)) {

		ImGui::PushFont(infofont);
		ImGui::Text("Score: %d", 0); // Replace 0 with actual score
		ImGui::Text("Time: %.1f", GameManager::GetInstance().GetLeftTime()); // Replace with actual time
		ImGui::Text("Health: %d / %d", GameManager::GetInstance().GetPlayer()->GetHealth(), GameManager::GetInstance().GetPlayer()->GetMaxHealth());// Replace with actual health
		ImGui::PopFont();

		ImGui::End();
	}

	// Ammo and weapon info (bottom-right corner)
	ImGui::SetNextWindowPos(ImVec2(main_viewport->Size.x - 210, main_viewport->Size.y - 130));
	ImGui::SetNextWindowSize(ImVec2(200, 120));
	ImGui::SetNextWindowBgAlpha(0.7f);

	if (ImGui::Begin("Weapon Info", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar)) {

		ImGui::PushFont(infofont);
		ImGui::Text("Weapon: Paintball Gun");
		ImGui::Text("Ammo: %d / %d", 30, 90); // Replace with actual ammo counts
		ImGui::Text("Power: 100%%");
		ImGui::PopFont();

		ImGui::End();
	}

	// Crosshair (center of screen)
	ImGui::SetNextWindowPos(ImVec2(main_viewport->GetCenter().x - 20, main_viewport->GetCenter().y - 20));
	ImGui::SetNextWindowBgAlpha(0.0f);
	if (ImGui::Begin("Crosshair", nullptr,
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoCollapse |
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoBackground)) {

		ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255, 255, 255, 255));
		ImGui::Text("+");
		ImGui::PopStyleColor();

		ImGui::End();
	}


	if (Window::GetKeyboard()->KeyDown(KeyCodes::ESCAPE)) {
		GameManager::GetInstance().SetGameState(PAUSED);
	}
}

void UI::DrawSettingMenu(float dt) {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

	// mainwindow
	ImGui::SetNextWindowPos(ImVec2(0, 0), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y), ImGuiCond_Always);

	ImGuiWindowFlags window_flags = ImGuiWindowFlags_NoDecoration |
		ImGuiWindowFlags_NoBackground |
		ImGuiWindowFlags_NoBringToFrontOnFocus |
		ImGuiWindowFlags_NoMove;

	if (ImGui::Begin("Settings", NULL, window_flags)) {

		ImVec2 windowSize = main_viewport->Size;
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image((ImTextureID)(uintptr_t)menu.img_texture, main_viewport->Size, ImVec2(0, 0), ImVec2(1, 1));
	}


	ImGui::PushFont(bigfont);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	float titleWidth = ImGui::CalcTextSize("Settings").x;
	ImGui::SetCursorPos(ImVec2(
		(main_viewport->Size.x - titleWidth) * 0.5f,
		main_viewport->Size.y * 0.15f
	));
	ImGui::Text("Settings");
	ImGui::PopStyleColor();
	ImGui::PopFont();

	ImGui::PushFont(menufont);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.8f, 0.8f, 0.8f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrab, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_SliderGrabActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.8f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.9f, 0.9f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	// layout
	float labelWidth = 200.0f;
	float sliderWidth = 600.0f;
	float extraSpacing = 80.0f;
	float totalWidth = labelWidth + sliderWidth + 50.0f;
	float startX = (main_viewport->Size.x - totalWidth) * 0.1f;
	float startY = main_viewport->Size.y * 0.3f;
	float itemSpacing = 80.0f;

	/*static bool debugMode = false;*/


	ImGui::SetCursorPos(ImVec2(startX, startY));
	ImGui::Text("BGM Volume");
	ImGui::SameLine(startX + labelWidth + extraSpacing);
	ImGui::PushItemWidth(sliderWidth);
	ImGui::SliderFloat("##BGM", &bgmVolume, 0.0f, 1.0f, "%.2f");


	ImGui::SetCursorPos(ImVec2(startX, startY + itemSpacing));
	ImGui::Text("Effect Volume");
	ImGui::SameLine(startX + labelWidth + extraSpacing);
	ImGui::SliderFloat("##Effect", &effectVolume, 0.0f, 1.0f, "%.2f");

	ImGui::SetCursorPos(ImVec2(startX, startY + itemSpacing * 2));
	ImGui::Text("Voice Volume");
	ImGui::SameLine(startX + labelWidth + extraSpacing);
	ImGui::SliderFloat("##Voice", &voiceVolume, 0.0f, 1.0f, "%.2f");

	ImGui::PopItemWidth();

	AudioSystem::GetInstance().SetBusVolume("BGM", bgmVolume);
	AudioSystem::GetInstance().SetBusVolume("Effect", effectVolume);
	AudioSystem::GetInstance().SetBusVolume("Voice", voiceVolume);


	// Debug Mode 
	ImGui::SetCursorPos(ImVec2(startX, startY + itemSpacing * 3));
	ImGui::Text("Debug Mode");
	ImGui::SameLine(startX + labelWidth + extraSpacing);
	ImGui::Checkbox("##Debug", &debugMode);
	// back
	float buttonWidth = 300.0f;
	float buttonHeight = 50.0f;
	ImGui::SetCursorPos(ImVec2(
		(main_viewport->Size.x - buttonWidth) * 0.5f,
		startY + itemSpacing * 4
	));

	if (ImGui::Button("Back to Menu", ImVec2(buttonWidth, buttonHeight))) {
		GameManager::GetInstance().SetGameState(MENU);
	}

	ImGui::PopStyleColor(7);
	ImGui::PopFont();
	ImGui::End();
}

void UI::DrawServerPlayingUI(float dt) {
	ImGui::Begin("Server Game UI");
	ImGui::Text("Server is running...");
	ImGui::End();
}

void UI::DrawClientPlayingUI(float dt) {
	ImGui::Begin("Client Game UI");
	ImGui::Text("Client is connected...");
	ImGui::End();
}

void UI::DrawChooseServer(float dt) {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();
	ImGui::SetNextWindowPos(ImVec2(main_viewport->WorkPos.x, main_viewport->WorkPos.y), ImGuiCond_Always);
	ImGui::SetNextWindowSize(ImVec2(main_viewport->Size.x, main_viewport->Size.y), ImGuiCond_Always);

	// 绘制背景图
	if (ImGui::Begin("Choose Server", NULL, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoBringToFrontOnFocus)) {
		ImGui::SetCursorPos(ImVec2(0, 0));
		ImGui::Image((ImTextureID)(uintptr_t)menu.img_texture, main_viewport->Size, ImVec2(0, 0), ImVec2(1, 1));
	}

	// 绘制标题
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.1f, 0.1f, 0.1f, 1.0f));
	ImGui::PushFont(bigfont);
	ImGui::SetCursorPos(ImVec2(
		main_viewport->GetCenter().x - ImGui::CalcTextSize("Enter Server IP").x * 0.5f,
		main_viewport->GetCenter().y - 150
	));
	ImGui::Text("Enter Server IP");
	ImGui::PopFont();
	ImGui::PopStyleColor();

	// 设置按钮和输入框的样式
	ImGui::PushFont(menufont);
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.8f, 0.8f, 0.8f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.9f, 0.9f, 0.9f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.8f, 0.8f, 0.8f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgHovered, ImVec4(0.9f, 0.9f, 0.9f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_FrameBgActive, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));

	// 定义输入框和按钮的尺寸及位置
	float inputWidth = 350.0f;
	float buttonWidth = 180.0f;
	float buttonHeight = 40.0f;
	float buttonSpacing = 20.0f;
	float centerX = (main_viewport->Size.x - inputWidth) * 0.5f;

	// 创建一个新的窗口用于输入框
	ImGui::SetNextWindowPos(ImVec2(centerX, main_viewport->GetCenter().y - 50));
	ImGui::SetNextWindowSize(ImVec2(inputWidth, 40));
	ImGui::SetNextWindowBgAlpha(0.0f);

	static char serverIP[128] = "127.0.0.1";

	if (ImGui::Begin("##ServerIPInput", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar)) {

		ImGui::PushItemWidth(inputWidth);

		// 如果需要获得焦点，设置焦点
		if (serverIPFocused) {
			ImGui::SetKeyboardFocusHere();
			serverIPFocused = false;  // 重置标志，避免重复设置焦点
		}

		// 使用InputText来处理输入
		if (ImGui::InputText("##IP", serverIP, IM_ARRAYSIZE(serverIP),
			ImGuiInputTextFlags_CharsNoBlank | ImGuiInputTextFlags_EnterReturnsTrue)) {
			// 当按下回车时连接服务器
			GameManager::GetInstance().SetGameState(PLAYING);
		}
		ProcessKeyboard(serverIP);


		ImGui::PopItemWidth();
	}
	ImGui::End();

	// Connect 按钮
	ImGui::SetCursorPos(ImVec2(centerX, main_viewport->GetCenter().y + 30));
	if (ImGui::Button("Connect", ImVec2(buttonWidth, buttonHeight))) {
		GameManager::GetInstance().SetGameState(PLAYING);
	}

	// Back 按钮
	ImGui::SameLine(0, buttonSpacing);
	if (ImGui::Button("Back", ImVec2(buttonWidth, buttonHeight))) {
		GameManager::GetInstance().SetGameState(MENU);
	}

	ImGui::PopStyleColor(7);
	ImGui::PopFont();
	ImGui::End();
}

void UI::DrawPausedMenu(float dt) {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

	// 设置窗口位置和大小
	float windowWidth = 400.0f;
	float windowHeight = 300.0f;
	ImGui::SetNextWindowPos(ImVec2(
		main_viewport->WorkPos.x + (main_viewport->Size.x - windowWidth) * 0.5f,
		main_viewport->WorkPos.y + (main_viewport->Size.y - windowHeight) * 0.5f
	));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

	// 设置窗口样式
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

	// 创建窗口
	if (ImGui::Begin("##PauseMenu", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse)) {

		// 标题
		ImGui::PushFont(bigfont);
		float titleWidth = ImGui::CalcTextSize("PAUSED").x;
		ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
		ImGui::SetCursorPosY(40.0f);
		ImGui::Text("PAUSED");
		ImGui::PopFont();

		ImGui::PushFont(menufont);

		// 按钮
		float buttonWidth = 200.0f;
		float buttonHeight = 50.0f;
		float buttonSpacing = 20.0f;

		// Resume 按钮
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
		ImGui::SetCursorPosY(120.0f);
		if (ImGui::Button("Resume", ImVec2(buttonWidth, buttonHeight))) {
			GameManager::GetInstance().SetGameState(PLAYING);
		}

		// Main Menu 按钮
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
		ImGui::SetCursorPosY(120.0f + buttonHeight + buttonSpacing);
		if (ImGui::Button("Main Menu", ImVec2(buttonWidth, buttonHeight))) {
			GameManager::GetInstance().SetGameState(MENU);
		}

		ImGui::PopFont();
	}
	ImGui::End();

	ImGui::PopStyleColor(5);
	ImGui::PopStyleVar(2);
}

void UI::DrawFailureMenu(float dt) {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

	// 设置窗口位置和大小
	float windowWidth = 400.0f;
	float windowHeight = 300.0f;
	ImGui::SetNextWindowPos(ImVec2(
		main_viewport->WorkPos.x + (main_viewport->Size.x - windowWidth) * 0.5f,
		main_viewport->WorkPos.y + (main_viewport->Size.y - windowHeight) * 0.5f
	));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

	// 设置窗口样式
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

	// 创建窗口
	if (ImGui::Begin("Failure", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse)) {

		// 标题
		ImGui::PushFont(bigfont);
		float titleWidth = ImGui::CalcTextSize("you lost").x;
		ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
		ImGui::SetCursorPosY(40.0f);
		ImGui::Text("you lost");
		ImGui::PopFont();

		ImGui::PushFont(menufont);

		// 按钮
		float buttonWidth = 200.0f;
		float buttonHeight = 50.0f;
		float buttonSpacing = 20.0f;

		// Resume 按钮
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
		ImGui::SetCursorPosY(120.0f);
		if (ImGui::Button("Retry", ImVec2(buttonWidth, buttonHeight))) {
			GameManager::GetInstance().SetGameState(PLAYING);
		}

		// Main Menu 按钮
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
		ImGui::SetCursorPosY(120.0f + buttonHeight + buttonSpacing);
		if (ImGui::Button("Main Menu", ImVec2(buttonWidth, buttonHeight))) {
			GameManager::GetInstance().SetGameState(MENU);
		}

		ImGui::PopFont();
	}
	ImGui::End();

	ImGui::PopStyleColor(5);
	ImGui::PopStyleVar(2);
}

void UI::DrawFinishMenu(float dt) {
	const ImGuiViewport* main_viewport = ImGui::GetMainViewport();

	// 设置窗口位置和大小
	float windowWidth = 400.0f;
	float windowHeight = 300.0f;
	ImGui::SetNextWindowPos(ImVec2(
		main_viewport->WorkPos.x + (main_viewport->Size.x - windowWidth) * 0.5f,
		main_viewport->WorkPos.y + (main_viewport->Size.y - windowHeight) * 0.5f
	));
	ImGui::SetNextWindowSize(ImVec2(windowWidth, windowHeight));

	// 设置窗口样式
	ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 12.0f);
	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(20.0f, 20.0f));
	ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0.0f, 0.0f, 0.0f, 0.9f));
	ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.2f, 0.2f, 0.2f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f, 0.3f, 0.3f, 1.0f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.4f, 0.4f, 0.4f, 1.0f));

	// 创建窗口
	if (ImGui::Begin("Win!!", nullptr,
		ImGuiWindowFlags_NoTitleBar |
		ImGuiWindowFlags_NoResize |
		ImGuiWindowFlags_NoMove |
		ImGuiWindowFlags_NoScrollbar |
		ImGuiWindowFlags_NoScrollWithMouse |
		ImGuiWindowFlags_NoCollapse)) {

		// 标题
		ImGui::PushFont(bigfont);
		float titleWidth = ImGui::CalcTextSize("you win!").x;
		ImGui::SetCursorPosX((windowWidth - titleWidth) * 0.5f);
		ImGui::SetCursorPosY(40.0f);
		ImGui::Text("you win");
		ImGui::PopFont();

		ImGui::PushFont(menufont);

		// 按钮
		float buttonWidth = 200.0f;
		float buttonHeight = 50.0f;
		float buttonSpacing = 20.0f;

		// Resume 按钮
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
		ImGui::SetCursorPosY(120.0f);
		if (ImGui::Button("Retry", ImVec2(buttonWidth, buttonHeight))) {
			GameManager::GetInstance().SetGameState(PLAYING);
		}

		// Main Menu 按钮
		ImGui::SetCursorPosX((windowWidth - buttonWidth) * 0.5f);
		ImGui::SetCursorPosY(120.0f + buttonHeight + buttonSpacing);
		if (ImGui::Button("Main Menu", ImVec2(buttonWidth, buttonHeight))) {
			GameManager::GetInstance().SetGameState(MENU);
		}

		ImGui::PopFont();
	}
	ImGui::End();

	ImGui::PopStyleColor(5);
	ImGui::PopStyleVar(2);
}

void UI::InitializeMainMenuButtons() {

	mainMenuButtons.clear();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	float buttonWidth = 220.0f;
	float buttonHeight = 50.0f;
	float buttonSpacing = 20.0f;
	float startY = viewport->Size.y * 0.4f;
	float centerX = (viewport->Size.x - buttonWidth) * 0.5f;

	UIButton btn;
	// Singleplayer 按钮
	btn.relativePos = ImVec2(centerX, startY);
	btn.size = ImVec2(buttonWidth, buttonHeight);
	btn.onClick = []() {
		GameManager::GetInstance().SetGameState(PLAYING);
		};
	mainMenuButtons.push_back(btn);

	// Multiplayer 按钮
	btn.relativePos = ImVec2(centerX, startY + buttonHeight + buttonSpacing);
	btn.size = ImVec2(buttonWidth, buttonHeight);
	btn.onClick = []() {
		GameManager::GetInstance().SetGameState(CHOOSESERVER);
		};
	mainMenuButtons.push_back(btn);

	// Settings 按钮
	btn.relativePos = ImVec2(centerX, startY + 2 * (buttonHeight + buttonSpacing));
	btn.size = ImVec2(buttonWidth, buttonHeight);
	btn.onClick = []() {
		GameManager::GetInstance().SetGameState(SETTING);
		};
	mainMenuButtons.push_back(btn);

	// Exit 按钮
	btn.relativePos = ImVec2(centerX, startY + 3 * (buttonHeight + buttonSpacing));
	btn.size = ImVec2(buttonWidth, buttonHeight);
	btn.onClick = []() {
		GameManager::GetInstance().SetGameState(EXIT);
		};
	mainMenuButtons.push_back(btn);
}

void UI::InitializeSettingMenuButtons() {
	settingMenuButtons.clear();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	float buttonWidth = 300.0f;
	float buttonHeight = 50.0f;
	float itemSpacing = 80.0f;
	float startY = viewport->Size.y * 0.3f;
	float posX = (viewport->Size.x - buttonWidth) * 0.5f;
	float posY = startY + itemSpacing * 4;

	UIButton btn;
	// "Back to Menu" 按钮
	btn.relativePos = ImVec2(posX, posY);
	btn.size = ImVec2(buttonWidth, buttonHeight);
	btn.onClick = []() {
		GameManager::GetInstance().SetGameState(MENU);
		};
	settingMenuButtons.push_back(btn);
}


void UI::InitializeChooseServerMenuButtons() {
	chooseServerMenuButtons.clear();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// 定义 DrawChooseServer() 中使用的尺寸参数
	float inputWidth = 350.0f;   // 输入框宽度（用于水平居中计算）
	float buttonWidth = 180.0f;
	float buttonHeight = 40.0f;
	float buttonSpacing = 20.0f;

	// 根据 DrawChooseServer() 中的代码，输入框水平居中使用：
	float centerX = (viewport->Size.x - inputWidth) * 0.5f;
	// Connect 按钮放在屏幕垂直中心偏下 30 个像素处
	float posY = (viewport->Size.y * 0.5f) + 30.0f;

	UIButton btn;
	// Connect 按钮
	btn.relativePos = ImVec2(centerX, posY);
	btn.size = ImVec2(buttonWidth, buttonHeight);
	btn.onClick = []() {
		// TODO：在这里可以增加使用输入框中 serverIP 的逻辑，目前直接切换状态为 CLIENTPLAYING
		std::cout << "ChooseServer: Connect button clicked" << std::endl;
		GameManager::GetInstance().SetGameState(PLAYING);
		};
	chooseServerMenuButtons.push_back(btn);

	// Back 按钮，位于 Connect 按钮右侧，间距 20 像素
	btn.relativePos = ImVec2(centerX + buttonWidth + buttonSpacing, posY);
	btn.size = ImVec2(buttonWidth, buttonHeight);
	btn.onClick = []() {
		std::cout << "ChooseServer: Back button clicked" << std::endl;
		GameManager::GetInstance().SetGameState(MENU);
		};
	chooseServerMenuButtons.push_back(btn);
}

void UI::ProcessKeyboard(char s[128])
{

}

void UI::InitializeFinishButtons() {
	FinishButtons.clear();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// 使用与DrawPausedMenu完全相同的窗口参数
	float windowWidth = 400.0f;
	float windowHeight = 300.0f;
	float buttonWidth = 200.0f;
	float buttonHeight = 50.0f;
	float buttonSpacing = 20.0f;

	// 计算窗口的绝对位置
	float windowX = viewport->WorkPos.x + (viewport->Size.x - windowWidth) * 0.5f;
	float windowY = viewport->WorkPos.y + (viewport->Size.y - windowHeight) * 0.5f;

	// Retry 按钮位置 (与DrawPausedMenu中的120.0f对应)
	UIButton retryBtn;
	retryBtn.relativePos = ImVec2(
		windowX - viewport->WorkPos.x + (windowWidth - buttonWidth) * 0.5f,
		windowY - viewport->WorkPos.y + 120.0f
	);
	retryBtn.size = ImVec2(buttonWidth, buttonHeight);
	retryBtn.onClick = []() {
		GameManager::GetInstance().SetGameState(PLAYING);
		};
	FinishButtons.push_back(retryBtn);

	// Main Menu 按钮位置 (与DrawPausedMenu中的120.0f + buttonHeight + buttonSpacing对应)
	UIButton menuBtn;
	menuBtn.relativePos = ImVec2(
		windowX - viewport->WorkPos.x + (windowWidth - buttonWidth) * 0.5f,
		windowY - viewport->WorkPos.y + 120.0f + buttonHeight + buttonSpacing
	);
	menuBtn.size = ImVec2(buttonWidth, buttonHeight);
	menuBtn.onClick = []() {
		GameManager::GetInstance().SetGameState(MENU);
		};
	FinishButtons.push_back(menuBtn);

}

void UI::InitializeFailureButtons() {
	FailureButtons.clear();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// 使用与DrawPausedMenu完全相同的窗口参数
	float windowWidth = 400.0f;
	float windowHeight = 300.0f;
	float buttonWidth = 200.0f;
	float buttonHeight = 50.0f;
	float buttonSpacing = 20.0f;

	// 计算窗口的绝对位置
	float windowX = viewport->WorkPos.x + (viewport->Size.x - windowWidth) * 0.5f;
	float windowY = viewport->WorkPos.y + (viewport->Size.y - windowHeight) * 0.5f;

	// Retry 按钮位置 (与DrawPausedMenu中的120.0f对应)
	UIButton retryBtn;
	retryBtn.relativePos = ImVec2(
		windowX - viewport->WorkPos.x + (windowWidth - buttonWidth) * 0.5f,
		windowY - viewport->WorkPos.y + 120.0f
	);
	retryBtn.size = ImVec2(buttonWidth, buttonHeight);
	retryBtn.onClick = []() {
		GameManager::GetInstance().SetGameState(PLAYING);
		};
	FailureButtons.push_back(retryBtn);

	// Main Menu 按钮位置 (与DrawPausedMenu中的120.0f + buttonHeight + buttonSpacing对应)
	UIButton menuBtn;
	menuBtn.relativePos = ImVec2(
		windowX - viewport->WorkPos.x + (windowWidth - buttonWidth) * 0.5f,
		windowY - viewport->WorkPos.y + 120.0f + buttonHeight + buttonSpacing
	);
	menuBtn.size = ImVec2(buttonWidth, buttonHeight);
	menuBtn.onClick = []() {
		GameManager::GetInstance().SetGameState(MENU);
		};
	FailureButtons.push_back(menuBtn);

}

void UI::InitializePauseMenuButtons() {
	pauseMenuButtons.clear();
	const ImGuiViewport* viewport = ImGui::GetMainViewport();

	// 使用与DrawPausedMenu完全相同的窗口参数
	float windowWidth = 400.0f;
	float windowHeight = 300.0f;
	float buttonWidth = 200.0f;
	float buttonHeight = 50.0f;
	float buttonSpacing = 20.0f;

	// 计算窗口的绝对位置
	float windowX = viewport->WorkPos.x + (viewport->Size.x - windowWidth) * 0.5f;
	float windowY = viewport->WorkPos.y + (viewport->Size.y - windowHeight) * 0.5f;

	// Resume 按钮位置 (与DrawPausedMenu中的120.0f对应)
	UIButton resumeBtn;
	resumeBtn.relativePos = ImVec2(
		windowX - viewport->WorkPos.x + (windowWidth - buttonWidth) * 0.5f,
		windowY - viewport->WorkPos.y + 120.0f
	);
	resumeBtn.size = ImVec2(buttonWidth, buttonHeight);
	resumeBtn.onClick = []() {
		GameManager::GetInstance().SetGameState(PLAYING);
		};
	pauseMenuButtons.push_back(resumeBtn);

	// Main Menu 按钮位置 (与DrawPausedMenu中的120.0f + buttonHeight + buttonSpacing对应)
	UIButton menuBtn;
	menuBtn.relativePos = ImVec2(
		windowX - viewport->WorkPos.x + (windowWidth - buttonWidth) * 0.5f,
		windowY - viewport->WorkPos.y + 120.0f + buttonHeight + buttonSpacing
	);
	menuBtn.size = ImVec2(buttonWidth, buttonHeight);
	menuBtn.onClick = []() {
		GameManager::GetInstance().SetGameState(MENU);
		};
	pauseMenuButtons.push_back(menuBtn);
}

void UI::ClickTest(float x, float y) {
	std::cout << "click :" << x << " " << y << std::endl;
}

void UI::ProcessClickEvent(float x, float y) {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	float winX = viewport->WorkPos.x;
	float winY = viewport->WorkPos.y;

	// 根据当前游戏状态选择对应的按钮列表
	std::vector<UIButton>* activeButtons = nullptr;
	auto currentState = GameManager::GetInstance().GetGameState();

	// 处理不同状态的UI元素
	switch (currentState) {
	case MENU:
		activeButtons = &mainMenuButtons;
		break;
	case SETTING:
		activeButtons = &settingMenuButtons;
		// 处理设置菜单的滑块和复选框
		HandleSettingMenuClick(x, y);
		break;
	case PAUSED:
		activeButtons = &pauseMenuButtons;
		break;
	case FAILURE:
		activeButtons = &FailureButtons;
		break;
	case FINISH:
		activeButtons = &FinishButtons;
		break;
	case CHOOSESERVER:
		activeButtons = &chooseServerMenuButtons;
		// 处理服务器选择菜单的输入框
		HandleChooseServerClick(x, y);
		break;
	}

	if (!activeButtons) return;

	// 遍历当前菜单所有按钮
	for (size_t i = 0; i < activeButtons->size(); ++i) {
		float btnAbsX = winX + (*activeButtons)[i].relativePos.x;
		float btnAbsY = winY + (*activeButtons)[i].relativePos.y;
		float w = (*activeButtons)[i].size.x;
		float h = (*activeButtons)[i].size.y;

		if (x >= btnAbsX && x <= btnAbsX + w &&
			y >= btnAbsY && y <= btnAbsY + h) {
			if ((*activeButtons)[i].onClick)
				(*activeButtons)[i].onClick();
			return;
		}
	}
}

// 处理设置菜单的点击事件
void UI::HandleSettingMenuClick(float x, float y) {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	float winX = viewport->WorkPos.x;
	float winY = viewport->WorkPos.y;

	// 定义滑块和复选框的位置参数
	float labelWidth = 200.0f;
	float sliderWidth = 600.0f;
	float extraSpacing = 80.0f;
	float totalWidth = labelWidth + sliderWidth + 50.0f;
	float startX = (viewport->Size.x - totalWidth) * 0.1f;
	float startY = viewport->Size.y * 0.3f;
	float itemSpacing = 80.0f;

	// 处理音量滑块
	float sliderY = startY;
	if (y >= sliderY && y <= sliderY + 40) {
		if (x >= startX + labelWidth + extraSpacing &&
			x <= startX + labelWidth + extraSpacing + sliderWidth) {
			// 计算滑块值
			float normalizedX = (x - (startX + labelWidth + extraSpacing)) / sliderWidth;
			bgmVolume = std::max(0.0f, std::min(1.0f, normalizedX));
			AudioSystem::GetInstance().SetBusVolume("BGM", bgmVolume);
		}
	}

	sliderY = startY + itemSpacing;
	if (y >= sliderY && y <= sliderY + 40) {
		if (x >= startX + labelWidth + extraSpacing &&
			x <= startX + labelWidth + extraSpacing + sliderWidth) {
			float normalizedX = (x - (startX + labelWidth + extraSpacing)) / sliderWidth;
			effectVolume = std::max(0.0f, std::min(1.0f, normalizedX));
			AudioSystem::GetInstance().SetBusVolume("Effect", effectVolume);
		}
	}

	sliderY = startY + itemSpacing * 2;
	if (y >= sliderY && y <= sliderY + 40) {
		if (x >= startX + labelWidth + extraSpacing &&
			x <= startX + labelWidth + extraSpacing + sliderWidth) {
			float normalizedX = (x - (startX + labelWidth + extraSpacing)) / sliderWidth;
			voiceVolume = std::max(0.0f, std::min(1.0f, normalizedX));
			AudioSystem::GetInstance().SetBusVolume("Voice", voiceVolume);
		}
	}

	// 处理Debug Mode复选框
	float checkboxY = startY + itemSpacing * 3;
	if (y >= checkboxY && y <= checkboxY + 40) {
		if (x >= startX + labelWidth + extraSpacing &&
			x <= startX + labelWidth + extraSpacing + 40) {
			debugMode = !debugMode;
		}
	}
}

// 处理服务器选择菜单的点击事件
void UI::HandleChooseServerClick(float x, float y) {
	const ImGuiViewport* viewport = ImGui::GetMainViewport();
	float winX = viewport->WorkPos.x;
	float winY = viewport->WorkPos.y;

	// 定义输入框的位置参数
	float inputWidth = 350.0f;
	float centerX = (viewport->Size.x - inputWidth) * 0.5f;
	float inputY = viewport->Size.y * 0.5f - 50;

	// 检查是否点击了输入框区域
	if (y >= inputY && y <= inputY + 40) {
		if (x >= centerX && x <= centerX + inputWidth) {
			serverIPFocused = true;  // 设置焦点状态
			isServerIPInputActive = true;
		}
	}
	else {
		// 如果点击在输入框外部，取消焦点
		serverIPFocused = false;
		isServerIPInputActive = false;
	}
}