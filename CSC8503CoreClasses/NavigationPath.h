#pragma once
#include <vector>

namespace NCL {
	using namespace NCL::Maths;
	namespace CSC8503 {
		class NavigationPath		{
		public:
			std::vector <Vector3> waypoints;
			NavigationPath() {}
			~NavigationPath() {}

			void	Clear() {
				waypoints.clear();
			}
			void	PushWaypoint(const Vector3& wp) {
				waypoints.emplace_back(wp);
			}
			bool	PopWaypoint(Vector3& waypoint) {
				if (waypoints.empty()) {
					return false;
				}
				waypoint = waypoints.back();
				waypoints.pop_back();
				return true;
			}
			void ReversePath() {
				std::reverse(waypoints.begin(), waypoints.end());
			}

		protected:

			
		};
	}
}

