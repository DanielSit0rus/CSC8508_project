#pragma once
#include "NavigationMap.h"
#include "Plane.h"
#include <string>
#include <vector>
#include "Debug.h" 
#include <queue>
#include <unordered_map>

namespace NCL {
	namespace CSC8503 {
		class NavigationMesh : public NavigationMap	{
		public:
			NavigationMesh(const std::string& filename1, const std::string& filename2);
			~NavigationMesh();

			bool FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) override;

			
			void DrawNavMesh() const;

			void LoadNavMesh(const std::string& filename, Vector3 offset, float scaleFactor);

			void MoveNavMesh(int meshIndex, Vector3 newOffset);
		
		protected:
			struct NavTri {
				Plane   triPlane;
				Vector3 centroid;
				float	area;
				NavTri* neighbours[3];

				int indices[3];

				NavTri() {
					area = 0.0f;
					neighbours[0] = nullptr;
					neighbours[1] = nullptr;
					neighbours[2] = nullptr;

					indices[0] = -1;
					indices[1] = -1;
					indices[2] = -1;
				}
			};

			const NavTri* GetTriForPosition(const Vector3& pos) const;

			std::vector<NavTri>		allTris;
			std::vector<Vector3>	allVerts;

			struct NavMeshTransform {
				Vector3 offset;
				float scaleFactor;
				NavMeshTransform(Vector3 off, float scale) : offset(off), scaleFactor(scale) {}
			};

			std::vector<NavMeshTransform> navMeshTransforms;

		};
	}
}