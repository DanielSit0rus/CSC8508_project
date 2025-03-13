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

		class PaintballGameWorld {
		public:
			PaintballGameWorld();
			~PaintballGameWorld();

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

			int GetObjectsNum() { return gameObjects.size(); }

		protected:
			std::vector<PaintballGameObject*> gameObjects;

			PerspectiveCamera mainCamera;

			bool shuffleObjects;
			int		worldIDCounter;
			int		worldStateCounter;
		};
	}
}