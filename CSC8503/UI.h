#pragma once
#include "Win32Window.h"
#include"window.h"
#include"imgui.h"
#include"imgui_impl_win32.h"
#include"imgui_impl_opengl3.h"
#include"Assets.h"

#include<string>
#include "PaintballGameWorld.h"
#include"stb_image_aug.h"
#include"GameTechRenderer.h"
#include<string>
#include<vector>
#include<fstream>

namespace NCL {
	namespace CSC8503 {

		enum UItype
		{

		};

		struct UI_Image
		{
			int img_width;
			int img_height;
			unsigned int img_texture;
		};


		struct UIButton {
			ImVec2 relativePos;                // The relative coordinates of the button within the menu window (relative to the main viewport WorkPos)
			ImVec2 size;
			std::function<void()> onClick;
		};
		class UI
		{
		public:
			UI(PaintballGameWorld* world);
			~UI();
			void Update(float dt);
			void DrawUI();
			void DrawLoading(float dt);
			void DrawMenu(float dt);
			void DrawPlayingUI(float dt);
			void DrawServerPlayingUI(float dt);
			void DrawClientPlayingUI(float dt);
			void DrawChooseServer(float dt);
			void DrawPausedMenu(float dt);
			void DrawFailureMenu(float dt);
			void DrawFinishMenu(float dt);
			void DrawSettingMenu(float dt);
			void ClickTest(float x, float y);


			void SetLoadingStep(int step = 1);
			void SetSuccess(bool s) { success = s; }
			bool IsDebugMode() const { return debugMode; }

			float GetPlayTime() { return playtime; };

			void ProcessClickEvent(float x, float y);

		protected:


			PaintballGameWorld* world;

			int loadingstep = 0;
			int totalStep;

			float bgmVolume;
			float effectVolume;
			float voiceVolume;

			ImFont* titlefont;
			ImFont* menufont;
			ImFont* infofont;
			ImFont* normalfont;
			ImFont* bigfont;
			ImFont* loadfont;

			UI_Image loading_bg;
			UI_Image bucket_empty;
			UI_Image bucket_fill;
			UI_Image loading;
			UI_Image team;
			UI_Image menu;
			UI_Image play;
			UI_Image assassin;
			std::vector<UIButton> mainMenuButtons;
			std::vector<UIButton> settingMenuButtons;
			std::vector<UIButton> pauseMenuButtons;
			std::vector<UIButton> chooseServerMenuButtons;
			std::vector<UIButton> FinishButtons;
			std::vector<UIButton> FailureButtons;


			void InitializeMainMenuButtons();
			void InitializeSettingMenuButtons();
			void InitializePauseMenuButtons();
			void InitializeChooseServerMenuButtons();
			void InitializeFinishButtons();
			void InitializeFailureButtons();

			void ProcessKeyboard(char s[128]);

			bool success = false;
			bool debugMode = true;

			float playtime;
			int fflag = 1;
			/*float firealpha = 0.0f;*/
			float counter = 3.0f;

			std::string dist;
			std::string time;
			std::string serverIP = "127 0 0 1";
			float ipPopupTimer = 0.0f;

			// Server management
			struct ServerInfo {
				std::string name;
				std::string ip;
				bool favorite;
			};
			std::vector<ServerInfo> availableServers;

			void LoadServerList() {
				std::ifstream file("servers.txt");
				if (file.is_open()) {
					ServerInfo server;
					while (std::getline(file, server.name)) {
						std::getline(file, server.ip);
						file >> server.favorite;
						file.ignore(); // Skip newline
						availableServers.push_back(server);
					}
					file.close();
				}
			}

			void SaveServerList() {
				std::ofstream file("servers.txt");
				if (file.is_open()) {
					for (const auto& server : availableServers) {
						file << server.name << std::endl;
						file << server.ip << std::endl;
						file << server.favorite << std::endl;
					}
					file.close();
				}
			}

			void PrintMousePos(float a, float b) {
				std::cout << "Mouse Pos: " << a << ", " << b << std::endl;
			}

			void ProcessKeyboardInput(std::string& s) {
				//s.c_str()

				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM1)) s += "1";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM2)) s += "2";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM3)) s += "3";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM4)) s += "4";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM5)) s += "5";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM6)) s += "6";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM7)) s += "7";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM8)) s += "8";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM9)) s += "9";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::NUM0)) s += "0";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::SPACE)) s += " ";
				//if (Window::GetKeyboard()->KeyPressed(KeyCodes::PERIOD)) s += ".";
				if (Window::GetKeyboard()->KeyPressed(KeyCodes::BACK) && s != "")s.pop_back();
			}

			bool isValidIPAddress(const std::string& ip);

			// 新增函数声明
			void HandleSettingMenuClick(float x, float y);

			void AddCharToServerIP(char c);

			// 新增变量
			bool showInvalidIPPopup = false;
			bool isServerIPInputActive = false;
			bool serverIPFocused = false;  // 新增：记录IP输入框是否需要获得焦点

		};
	}
}