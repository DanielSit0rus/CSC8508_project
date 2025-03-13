#pragma once

#include <unordered_map>
#include <stack>

#include "PaintballGameObject.h"
#include "PaintballBullet.h"

namespace NCL::CSC8503 {
	class GameObjectFreeList
	{
	public:
		static GameObjectFreeList& GetInstance() {
			static GameObjectFreeList instance;
			return instance;
		}

		PaintballBullet* GetBullet(rp3d::Vector3 ori3, bool isenemy,
			const rp3d::Vector3& position, rp3d::Vector3 dimensions, rp3d::Quaternion orientation,
			Vector4 color, float mass = 1.0f);

		void PushObject(PaintballGameObject* obj);

		void Clear();


	private:
		GameObjectFreeList() = default;
		~GameObjectFreeList() = default;

		std::unordered_map<GameObjectType, std::stack<PaintballGameObject*>> objLists;
	};
}