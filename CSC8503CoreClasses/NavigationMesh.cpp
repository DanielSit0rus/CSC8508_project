#include "NavigationMesh.h"
#include "Assets.h"
#include "Maths.h"
#include <fstream>
using namespace NCL;
using namespace CSC8503;
using namespace std;

NavigationMesh::NavigationMesh()
{
}

NavigationMesh::NavigationMesh(const std::string&filename)
{
	ifstream file(Assets::DATADIR + filename);

	int numVertices = 0;
	int numIndices	= 0;

	file >> numVertices;
	file >> numIndices;

	for (int i = 0; i < numVertices; ++i) {
		Vector3 vert;
		file >> vert.x;
		file >> vert.y;
		file >> vert.z;

		allVerts.emplace_back(vert);
	}

	allTris.resize(numIndices / 3);

	for (int i = 0; i < allTris.size(); ++i) {
		NavTri* tri = &allTris[i];
		file >> tri->indices[0];
		file >> tri->indices[1];
		file >> tri->indices[2];

		tri->centroid = allVerts[tri->indices[0]] +
			allVerts[tri->indices[1]] +
			allVerts[tri->indices[2]];

		tri->centroid = allTris[i].centroid / 3.0f;

		tri->triPlane = Plane::PlaneFromTri(allVerts[tri->indices[0]],
			allVerts[tri->indices[1]],
			allVerts[tri->indices[2]]);

		tri->area = Maths::AreaofTri3D(allVerts[tri->indices[0]], allVerts[tri->indices[1]], allVerts[tri->indices[2]]);
	}
	for (int i = 0; i < allTris.size(); ++i) {
		NavTri* tri = &allTris[i];
		for (int j = 0; j < 3; ++j) {
			int index = 0;
			file >> index;
			if (index != -1) {
				tri->neighbours[j] = &allTris[index];
			}
		}
	}
}

NavigationMesh::~NavigationMesh()
{
}

bool NavigationMesh::FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) {
	const NavTri* start	= GetTriForPosition(from);
	const NavTri* end	= GetTriForPosition(to);

	return false;
}

/*
If you have triangles on top of triangles in a full 3D environment, you'll need to change this slightly,
as it is currently ignoring height. You might find tri/plane raycasting is handy.
*/

const NavigationMesh::NavTri* NavigationMesh::GetTriForPosition(const Vector3& pos) const {
	for (const NavTri& t : allTris) {
		Vector3 planePoint = t.triPlane.ProjectPointOntoPlane(pos);

		float ta = Maths::AreaofTri3D(allVerts[t.indices[0]], allVerts[t.indices[1]], planePoint);
		float tb = Maths::AreaofTri3D(allVerts[t.indices[1]], allVerts[t.indices[2]], planePoint);
		float tc = Maths::AreaofTri3D(allVerts[t.indices[2]], allVerts[t.indices[0]], planePoint);

		float areaSum = ta + tb + tc;

		if (abs(areaSum - t.area)  > 0.001f) { //floating points are annoying! Are we more or less inside the triangle?
			continue;
		}
		return &t;
	}
	return nullptr;
}



void NavigationMesh::DrawNavMesh() const {
	// Define colors for clarity
	Vector4 vertexColor = Vector4(1.0f, 0.0f, 0.0f, 1.0f); // Red for vertices
	Vector4 edgeColor = Vector4(0.0f, 1.0f, 0.0f, 1.0f);   // Green for edges
	Vector4 centroidColor = Vector4(0.0f, 0.0f, 1.0f, 1.0f); // Blue for centroids

	// Draw vertices
	for (const Vector3& vertex : allVerts) {
		//Debug::DrawAxisLines(vertex, vertexColor);
	}

	// Draw triangles and edges
	for (const NavTri& tri : allTris) {
		const Vector3& v0 = allVerts[tri.indices[0]];
		const Vector3& v1 = allVerts[tri.indices[1]];
		const Vector3& v2 = allVerts[tri.indices[2]];

		// Draw edges of the triangle
		Debug::DrawLine(v0, v1, edgeColor);
		Debug::DrawLine(v1, v2, edgeColor);
		Debug::DrawLine(v2, v0, edgeColor);

		// Draw centroid
		//Debug::DrawAxisLines(tri.centroid, centroidColor);
	}
}