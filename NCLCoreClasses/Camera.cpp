#include "Camera.h"
#include "Window.h"

using namespace NCL;

/*
Polls the camera for keyboard / mouse movement.
Should be done once per frame! Pass it the msec since
last frame (default value is for simplicities sake...)
*/
void Camera::UpdateCamera(float dt) {
	if (!activeController) {
		return;
	}

	//Update the mouse by how much
	pitch	-= activeController->GetNamedAxis("YLook");
	yaw		-= activeController->GetNamedAxis("XLook");

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw <0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	float frameSpeed = speed * dt;

	Matrix3 yawRotation = Matrix::RotationMatrix3x3(yaw, Vector3(0, 1, 0));

	position += yawRotation * Vector3(0, 0, -activeController->GetNamedAxis("Forward")) * frameSpeed;
	position += yawRotation * Vector3(activeController->GetNamedAxis("Sidestep"), 0, 0) * frameSpeed;

	position.y += activeController->GetNamedAxis("UpDown") * frameSpeed;
	
}
void Camera::UpdateCamera() {
	if (!activeController) {
		return;
	}

	//Update the mouse by how much
	pitch -= activeController->GetNamedAxis("YLook");
	yaw -= activeController->GetNamedAxis("XLook");

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}
}
void Camera::UpdateCamera(float dt, float sp) {
	if (!activeController) {
		return;
	}

	//Update the mouse by how much
	pitch -= activeController->GetNamedAxis("YLook");
	yaw -= activeController->GetNamedAxis("XLook");

	//Bounds check the pitch, to be between straight up and straight down ;)
	pitch = std::min(pitch, 90.0f);
	pitch = std::max(pitch, -90.0f);

	if (yaw < 0) {
		yaw += 360.0f;
	}
	if (yaw > 360.0f) {
		yaw -= 360.0f;
	}

	float frameSpeed = sp * dt;

	Matrix3 yawRotation = Matrix::RotationMatrix3x3(yaw, Vector3(0, 1, 0));

	position += yawRotation * Vector3(0, 0, -activeController->GetNamedAxis("Forward")) * frameSpeed;
	position += yawRotation * Vector3(activeController->GetNamedAxis("Sidestep"), 0, 0) * frameSpeed;

	position.y += activeController->GetNamedAxis("UpDown") * frameSpeed;

}

/*
Generates a view matrix for the camera's viewpoint. This matrix can be sent
straight to the shader...it's already an 'inverse camera' matrix.
*/
Matrix4 Camera::BuildViewMatrix() const {
	//Why do a complicated matrix inversion, when we can just generate the matrix
	//using the negative values ;). The matrix multiplication order is important!
	return	Matrix::Rotation(-pitch, Vector3(1, 0, 0)) *
		Matrix::Rotation(-yaw, Vector3(0, 1, 0)) *
		Matrix::Translation(-position);
};

Matrix4 PerspectiveCamera::BuildProjectionMatrix(float currentAspect) const {
	return Matrix::Perspective(nearPlane, farPlane, currentAspect, fov);
}

Matrix4 OrhographicCamera::BuildProjectionMatrix(float currentAspect) const {
	return Matrix::Orthographic(left, right, bottom, top, nearPlane, farPlane);
}

#pragma region Raycasst
Vector3 PerspectiveCamera::GetMouseDir() {
	Vector2 screenMouse = Window::GetMouse()->GetAbsolutePosition();
	Vector2i screenSize = Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenMouse.x,
		screenSize.y - screenMouse.y,
		0.99999f
	);

	Vector3 a = Unproject(nearPos);
	Vector3 b = Unproject(farPos);
	Vector3 c = b - a;

	return Vector::Normalise(c);
}
Vector3 PerspectiveCamera::GetScreenDir(float xRatio, float yRatio) {
	Vector2i screenSize = Window::GetWindow()->GetScreenSize();

	//We remove the y axis mouse position from height as OpenGL is 'upside down',
	//and thinks the bottom left is the origin, instead of the top left!
	Vector3 nearPos = Vector3(screenSize.x * xRatio,
		screenSize.y * yRatio,
		-0.99999f
	);

	//We also don't use exactly 1.0 (the normalised 'end' of the far plane) as this
	//causes the unproject function to go a bit weird. 
	Vector3 farPos = Vector3(screenSize.x / 2,
		screenSize.y / 2,
		0.99999f
	);

	Vector3 a = Unproject(nearPos);
	Vector3 b = Unproject(farPos);
	Vector3 c = b - a;

	c = Vector::Normalise(c);

	return Vector::Normalise(c);
}

Vector3 PerspectiveCamera::Unproject(const Vector3& screenPos) {
	Vector2i screenSize = Window::GetWindow()->GetScreenSize();

	float aspect = Window::GetWindow()->GetScreenAspect();
	float fov = GetFieldOfVision();
	float nearPlane = GetNearPlane();
	float farPlane = GetFarPlane();

	//Create our inverted matrix! Note how that to get a correct inverse matrix,
	//the order of matrices used to form it are inverted, too.
	Matrix4 invVP = GenerateInverseView() * GenerateInverseProjection(aspect, fov, nearPlane, farPlane);

	Matrix4 proj = this->BuildProjectionMatrix(aspect);

	//Our mouse position x and y values are in 0 to screen dimensions range,
	//so we need to turn them into the -1 to 1 axis range of clip space.
	//We can do that by dividing the mouse values by the width and height of the
	//screen (giving us a range of 0.0 to 1.0), multiplying by 2 (0.0 to 2.0)
	//and then subtracting 1 (-1.0 to 1.0).
	Vector4 clipSpace = Vector4(
		(screenPos.x / (float)screenSize.x) * 2.0f - 1.0f,
		(screenPos.y / (float)screenSize.y) * 2.0f - 1.0f,
		(screenPos.z),
		1.0f
	);

	//Then, we multiply our clipspace coordinate by our inverted matrix
	Vector4 transformed = invVP * clipSpace;

	//our transformed w coordinate is now the 'inverse' perspective divide, so
	//we can reconstruct the final world space by dividing x,y,and z by w.
	return Vector3(transformed.x / transformed.w, transformed.y / transformed.w, transformed.z / transformed.w);
}

Matrix4 PerspectiveCamera::GenerateInverseView() {
	Matrix4 iview =
		Matrix::Translation(GetPosition()) *
		Matrix::Rotation(GetYaw(), Vector3(0, 1, 0)) *
		Matrix::Rotation(GetPitch(), Vector3(1, 0, 0));
	return iview;
}

Matrix4 PerspectiveCamera::GenerateInverseProjection(float aspect, float fov, float nearPlane, float farPlane) {
	Matrix4 m;

	float t = tan(fov * PI_OVER_360);

	float neg_depth = nearPlane - farPlane;

	const float h = 1.0f / t;

	float c = (farPlane + nearPlane) / neg_depth;
	float e = -1.0f;
	float d = 2.0f * (nearPlane * farPlane) / neg_depth;

	m.array[0][0] = aspect / h;
	m.array[1][1] = tan(fov * PI_OVER_360);
	m.array[2][2] = 0.0f;

	m.array[2][3] = 1.0f / d;

	m.array[3][2] = 1.0f / e;
	m.array[3][3] = -c / (d * e);

	return m;
}
#pragma endregion
