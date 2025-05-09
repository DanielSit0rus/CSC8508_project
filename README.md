# COLOR THEORY
A **first-person paintball game** developed as part of a Team Project (CSC8508) at **Newcastle University**, built using a **custom C++ Game Engine**. This repository contains the **PC OpenGL build** of the game, with support for physics, AI, UI, and multiplayer.

[Demo Video](https://www.youtube.com/watch?v=XYPnicpUgAI)

[Multiplayer Demo Video](https://www.youtube.com/watch?v=06udzTvDwIg)

## Gameplay Overview:
- Navigate through an arena-style environment.
- Shoot enemies using color-coded paintballs.
- Match the paintball color to the enemy's indicator to deal damage.
- Defeat all enemies in the arena while managing your aim and timing to survive.

## Features
- Navigation mesh based pathfinding for AI (patrol, chase, attack)
- Color-based combat mechanic (Enemy color changes with damage taken)
- Physics integration using [ReactPhysics3D](https://github.com/DanielChappuis/reactphysics3d)
- Dynamic sound integration using FMOD 
- Simple custom UI system using ImgUI
- JSON-based level and object loading
- Network Multiplayer for Windows using enet
- Developer Console for Debugging
  
  ## 📸 Screenshots
![screenshot1](https://media.journoportfolio.com/users/351105/images/88d3a0ea-5306-4925-ac04-fbdd2c935039.png)
![screenshot2](https://media.journoportfolio.com/users/351105/images/7be31e7d-52d9-47ca-b949-ba8df69183dd.png)
![screenshot3](https://media.journoportfolio.com/users/351105/images/33de8158-5da7-4a73-a603-65c3d7507bf2.png)

## Setup (PC):
1. Clone the repository
2. Generate project soultion with CMake
3. Open the generated Visual Studio solution and build the project
4. Press F3 to open the developer console and 'help' command to get the list of commands  


## 👥 Credits
Team members:
- Daniel Sitorus
- George Jose Ambooken
- Kelvin (Hong Zhang)
- Chen Lizhi
- Aria(Jiahui Tang)
- Vaibhav Koorkkanadath
- Terrance(He Hu)

Special thanks to the University staff for guidance and devkit access with sample codebase.
