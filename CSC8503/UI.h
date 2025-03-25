#pragma once
#include "Win32Window.h"
#include"window.h"
#include"imgui.h"
#include"imgui_impl_win32.h"
#include"imgui_impl_opengl3.h"
#include"Assets.h"
#include"GameWorld.h"
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

			void SetShowGameOverMenu(bool show) { showGameOver = show; }

			void SetLoadingStep(int step = 1);
			void SetSuccess(bool s) { success = s; }
			bool IsDebugMode() const { return debugMode; }

			float GetPlayTime() { return playtime; };
		protected:
			PaintballGameWorld* world;

			int loadingstep = 0;
			int totalStep = 1;

			float bgmVolume;
			float effectVolume;
			float voiceVolume;

			ImFont* titlefont;
			ImFont* menufont;
			ImFont* infofont;
			ImFont* normalfont;
			ImFont* bigfont;

			UI_Image loading_bg;
			UI_Image bucket_empty;
			UI_Image bucket_fill;
			UI_Image loading;
			UI_Image team;
			UI_Image menu;
			UI_Image play;
			UI_Image assassin;

			bool success = false;
			bool debugMode = true;
			bool showGameOver = false;

			float playtime;
			int fflag = 1;
			/*float firealpha = 0.0f;*/
			float counter = 3.0f;

			std::string dist;
			std::string time;

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
		};
	}
}