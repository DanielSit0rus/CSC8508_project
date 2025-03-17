#pragma once
#include <reactphysics3d/reactphysics3d.h>

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		//class PaintballGameObject;
		class NetworkState {
		public:
			NetworkState();
			virtual ~NetworkState();

			int					type;		//static_cast<GameObjectType>(state.type)
			rp3d::Vector3		position;
			rp3d::Vector3		scale;
			rp3d::Quaternion	orientation;

			int					meshID		= -1;
			int					textureID_D = -1;
			int					textureID_S = -1;
			int					textureID_N = -1;
			int					textureID_H = -1;
			int					textureID_M = -1;
			int					textureID_O = -1;
			int					textureID_MM = -1;
			int					textureID_R= -1;
			int					shaderID	= -1;

			Vector4				color;
			int					mass;

			bool				isEnemy;
			rp3d::Vector3		oriV3;

			int					stateID;

			bool				toDelete = false;
			bool				isActive = true;

			int					playerID;
		};
	}
}

