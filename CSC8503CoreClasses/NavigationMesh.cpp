#include "NavigationMesh.h"
#include "NavigationPath.h"
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
	if (!file.is_open()) {
		Debug::Print("Failed to open navigation mesh file: " + filename, Vector2(10, 10), Debug::RED);
		return;
	}

	int numVertices = 0;
	int numIndices	= 0;

	file >> numVertices;
	file >> numIndices;
	Debug::Print("Loaded " + std::to_string(numVertices) + " vertices and " +
		std::to_string(numIndices) + " indices.", Vector2(10, 20), Debug::GREEN);

	// Transform reference point (-13.06667, 1.016667, 4.8) to (0, 1, 0)
    Vector3 referencePoint(-13.06667f, 1.016667f, 4.8f);
    Vector3 targetPoint(0.0f, 1.0f, 0.0f);
    Vector3 translation = targetPoint - referencePoint;

	allVerts.reserve(numVertices); // avoid reallocation
	for (int i = 0; i < numVertices; ++i) {
		Vector3 vert;
		file >> vert.x;
		file >> vert.y;
		file >> vert.z;
		vert += translation;
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

	Debug::Print("Navigation mesh loaded successfully.", Vector2(10, 30), Debug::GREEN);
}

NavigationMesh::~NavigationMesh()
{
}

bool NavigationMesh::FindPath(const Vector3& from, const Vector3& to, NavigationPath& outPath) {
	const NavTri* startTri = GetTriForPosition(from);
	const NavTri* goalTri = GetTriForPosition(to);

	if (!startTri || !goalTri) {
		return false; // Start or goal is not on the navigation mesh
	}

	// Lambda to calculate the heuristic (Euclidean distance)
	auto heuristic = [](const Vector3& a, const Vector3& b) {
		return Vector::Length((a - b));
		};

	// Open list to hold the frontier for exploration, using a min-heap based on f cost
	auto Compare = [&](const pair<float, const NavTri*>& a, const pair<float, const NavTri*>& b) {
		return a.first > b.first;
		};
	std::priority_queue<pair<float, const NavTri*>, vector<pair<float, const NavTri*>>, decltype(Compare)> openList(Compare);

	// Maps to keep track of costs and path
	std::unordered_map<const NavTri*, float> costSoFar;
	std::unordered_map<const NavTri*, const NavTri*> cameFrom;

	// Start by adding the starting triangle
	costSoFar[startTri] = 0.0f;
	openList.push(make_pair(heuristic(startTri->centroid, goalTri->centroid), startTri));
	cameFrom[startTri] = nullptr;

	while (!openList.empty()) {
		const NavTri* current = openList.top().second;
		openList.pop();

		// Check if we have reached the goal
		if (current == goalTri) {
			// Reconstruct path
			const NavTri* step = goalTri;
			while (step != startTri) {
				outPath.PushWaypoint(step->centroid);
				step = cameFrom[step];
			}
			outPath.PushWaypoint(startTri->centroid);
			std::reverse(outPath.waypoints.begin(), outPath.waypoints.end());
			return true;
		}

		// Explore each neighbor
		for (int i = 0; i < 3; ++i) {
			const NavTri* neighbor = current->neighbours[i];
			if (!neighbor) continue; // Skip if no neighbor

			float newCost = costSoFar[current] + heuristic(current->centroid, neighbor->centroid);
			if (costSoFar.find(neighbor) == costSoFar.end() || newCost < costSoFar[neighbor]) {
				costSoFar[neighbor] = newCost;
				float priority = newCost + heuristic(neighbor->centroid, goalTri->centroid);
				openList.push(make_pair(priority, neighbor));
				cameFrom[neighbor] = current;
			}
		}
	}

	return false; // No path found
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

	Vector3 boxSize = Vector3(0.1f, 0.1f, 0.1f);


	// Draw vertices
	for (const Vector3& vertex : allVerts) {
		//Debug::DrawAxisLines(vertex, vertexColor);
		Debug::DrawBox(vertex, boxSize, vertexColor, 0.05f);
		
	}

	// Draw triangles and edges
	for (const NavTri& tri : allTris) {
		const Vector3& v0 = allVerts[tri.indices[0]];
		const Vector3& v1 = allVerts[tri.indices[1]];
		const Vector3& v2 = allVerts[tri.indices[2]];

		// Draw edges of the triangle
		/*Debug::DrawLine(v0, v1, edgeColor);
		Debug::DrawLine(v1, v2, edgeColor);
		Debug::DrawLine(v2, v0, edgeColor);*/

		// Draw centroid
		Vector3 centroid = (v0 + v1 + v2) / 3.0f;
		NCL::Debug::DrawBox(centroid, boxSize, centroidColor, 0.15f);
	}
}