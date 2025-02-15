#include "Debug.h"
using namespace NCL;

std::vector<Debug::DebugStringEntry>	Debug::stringEntries;
std::vector<Debug::DebugLineEntry>		Debug::lineEntries;
std::vector<Debug::DebugTexEntry>		Debug::texEntries;

SimpleFont* Debug::debugFont = nullptr;

const Vector4 Debug::RED		= Vector4(1, 0, 0, 1);
const Vector4 Debug::GREEN		= Vector4(0, 1, 0, 1);
const Vector4 Debug::BLUE		= Vector4(0, 0, 1, 1);

const Vector4 Debug::BLACK		= Vector4(0, 0, 0, 1);
const Vector4 Debug::WHITE		= Vector4(1, 1, 1, 1);

const Vector4 Debug::YELLOW		= Vector4(1, 1, 0, 1);
const Vector4 Debug::MAGENTA	= Vector4(1, 0, 1, 1);
const Vector4 Debug::CYAN		= Vector4(0, 1, 1, 1);

void Debug::Print(const std::string& text, const Vector2& pos, const Vector4& colour) {
	DebugStringEntry newEntry;

	newEntry.data = text;
	newEntry.position = pos;
	newEntry.colour = colour;

	stringEntries.emplace_back(newEntry);
}

void Debug::DrawLine(const Vector3& startpoint, const Vector3& endpoint, const Vector4& colour, float time) {
	DebugLineEntry newEntry;

	newEntry.start = startpoint;
	newEntry.end = endpoint;
	newEntry.colourA = colour;
	newEntry.colourB = colour;
	newEntry.time = time;

	lineEntries.emplace_back(newEntry);
}

void Debug::DrawTex(const Texture& t, const Vector2& pos, const Vector2& scale, const Vector4& colour) {
	DebugTexEntry newEntry;

	newEntry.t			= &t;
	newEntry.position	= pos;
	newEntry.scale		= scale;
	newEntry.colour		= colour;

	texEntries.push_back(newEntry);
}

void Debug::DrawAxisLines(const Matrix4& modelMatrix, float scaleBoost, float time) {
	Matrix4 local = modelMatrix;
	local.SetColumn(3, Vector4(0,0,0,1));

	Vector3 fwd = local * Vector4(0, 0, -1, 1.0f);
	Vector3 up = local * Vector4(0, 1, 0, 1.0f);
	Vector3 right = local * Vector4(1, 0, 0, 1.0f);

	Vector3 worldPos = modelMatrix.GetColumn(3);

	DrawLine(worldPos, worldPos + (right * scaleBoost), Debug::RED, time);
	DrawLine(worldPos, worldPos + (up * scaleBoost), Debug::GREEN, time);
	DrawLine(worldPos, worldPos + (fwd * scaleBoost), Debug::BLUE, time);
}

void Debug::DrawBox(const Vector3& boxCenter, const Vector3& boxSize, const Vector4& colour, float time)
{
	Vector3 v3FrontTopLeft = Vector3(boxCenter.x - boxSize.x, boxCenter.y + boxSize.y, boxCenter.z - boxSize.z);
	Vector3 v3FrontTopRight = Vector3(boxCenter.x + boxSize.x, boxCenter.y + boxSize.y, boxCenter.z - boxSize.z);
	Vector3 v3FrontBottomLeft = Vector3(boxCenter.x - boxSize.x, boxCenter.y - boxSize.y, boxCenter.z - boxSize.z);
	Vector3 v3FrontBottomRight = Vector3(boxCenter.x + boxSize.x, boxCenter.y - boxSize.y, boxCenter.z - boxSize.z);

	Vector3 v3BackTopLeft = Vector3(boxCenter.x - boxSize.x, boxCenter.y + boxSize.y, boxCenter.z + boxSize.z);
	Vector3 v3BackTopRight = Vector3(boxCenter.x + boxSize.x, boxCenter.y + boxSize.y, boxCenter.z + boxSize.z);
	Vector3 v3BackBottomLeft = Vector3(boxCenter.x - boxSize.x, boxCenter.y - boxSize.y, boxCenter.z + boxSize.z);
	Vector3 v3BackBottomRight = Vector3(boxCenter.x + boxSize.x, boxCenter.y - boxSize.y, boxCenter.z + boxSize.z);

	DrawLine(v3FrontTopLeft, v3FrontTopRight, colour, time);
	DrawLine(v3FrontTopRight, v3FrontBottomRight, colour, time);
	DrawLine(v3FrontBottomRight, v3FrontBottomLeft, colour, time);
	DrawLine(v3FrontBottomLeft, v3FrontTopLeft, colour, time);

	DrawLine(v3BackTopLeft, v3BackTopRight, colour, time);
	DrawLine(v3BackTopRight, v3BackBottomRight, colour, time);
	DrawLine(v3BackBottomRight, v3BackBottomLeft, colour, time);
	DrawLine(v3BackBottomLeft, v3BackTopLeft, colour, time);

	DrawLine(v3FrontTopLeft, v3BackTopLeft, colour, time);
	DrawLine(v3FrontTopRight, v3BackTopRight, colour, time);
	DrawLine(v3FrontBottomRight, v3BackBottomRight, colour, time);
	DrawLine(v3FrontBottomLeft, v3BackBottomLeft, colour, time);
}


void Debug::UpdateRenderables(float dt) {
	int trim = 0;
	for (int i = 0; i < lineEntries.size(); ) {
		DebugLineEntry* e = &lineEntries[i];
		e->time -= dt;
		if (e->time < 0) {
			trim++;
			lineEntries[i] = lineEntries[lineEntries.size() - trim];
		}
		else {
			++i;
		}
		if (i + trim >= lineEntries.size()) {
			break;
		}
	}
	lineEntries.resize(lineEntries.size() - trim);
	stringEntries.clear();
	texEntries.clear();
}

SimpleFont* Debug::GetDebugFont() {
	return debugFont;
}

void Debug::CreateDebugFont(const std::string& dataFile, Texture& tex) {
	debugFont = new SimpleFont(dataFile, tex);
}

const std::vector<Debug::DebugStringEntry>& Debug::GetDebugStrings() {
	return stringEntries;
}

const std::vector<Debug::DebugLineEntry>& Debug::GetDebugLines() {
	return lineEntries;
}

const std::vector<Debug::DebugTexEntry>& Debug::GetDebugTex() {
	return texEntries;
}