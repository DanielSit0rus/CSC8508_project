#pragma once
#include <reactphysics3d/reactphysics3d.h>

namespace NCL {
	using namespace Maths;
	namespace CSC8503 {
		//class GameObject;
		class NetworkState	{
		public:
			NetworkState();
			virtual ~NetworkState();

			rp3d::Vector3		position;
			rp3d::Quaternion	orientation;
			int					stateID;

			bool				toDelete = false;
			bool				isActive = true;
		};
	}
}

