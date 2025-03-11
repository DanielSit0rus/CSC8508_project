#pragma once
#include <reactphysics3d/reactphysics3d.h>

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		//class PaintballGameObject;
		class NetworkState	{
		public:
			NetworkState();
			virtual ~NetworkState();

			int					type;		//static_cast<GameObjectType>(state.type)
			rp3d::Vector3		position;
			rp3d::Vector3		scale;
			rp3d::Quaternion	orientation;
			//Mesh*
			Vector4				color;
			int					mass;

			bool				isEnemy;
			rp3d::Vector3		oriV3;

			int					stateID;

			bool				toDelete = false;
			bool				isActive = true;
		};
	}
}

