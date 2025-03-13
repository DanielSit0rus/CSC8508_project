#pragma once
#include <random>
#include "Camera.h"
#include "Debug.h"
#include "../CSC8503/PaintballGameObject.h"

namespace NCL {
	class Camera;
	namespace CSC8503 {
		class PaintballGameObject;

		typedef std::function<void(PaintballGameObject*)> PaintballGameObjectFunc;
		typedef std::vector<PaintballGameObject*>::const_iterator PaintballGameObjectIterator;

		enum PaintballGameState {
			LOADING,
			PLAYING,
			SERVERPLAYING,
			CLIENTPLAYING,
			PAUSED,
			FAILURE,
			FINISH,
			MENU,
			SETTING,
			CHOOSESERVER,
			EXIT
		};//imgui gamestate

		

		class PaintballGameWorld {
		public:
			PaintballGameWorld();
			~PaintballGameWorld();

			PaintballGameState GetGameState() const {

				return currentstate;
			}

			void Clear();
			void ClearAndErase();

			void AddGameObject(PaintballGameObject* o);
			void RemoveGameObject(PaintballGameObject* o, bool andDelete = false);

			PerspectiveCamera& GetMainCamera() {
				return mainCamera;
			}

			void ShuffleObjects(bool state) {
				shuffleObjects = state;
			}

			virtual void UpdateWorld(float dt);

			void OperateOnContents(PaintballGameObjectFunc f);

			void GetObjectIterators(
				PaintballGameObjectIterator& first,
				PaintballGameObjectIterator& last) const;

			int GetWorldStateID() const {
				return worldStateCounter;
			}

			void SetGameState(PaintballGameState state);
			PaintballGameState GetGameState() { return currentstate; };


		protected:
			std::vector<PaintballGameObject*> gameObjects;

			PerspectiveCamera mainCamera;

			bool shuffleObjects;
			int		worldIDCounter;
			int		worldStateCounter;

			PaintballGameState currentstate;
		};
	}
}