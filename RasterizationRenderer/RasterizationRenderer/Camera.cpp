#include "Camera.h"

Camera::Camera(Vector3 position1, Vector3 up, Vector3 front1, double yaw1, double pitch1)
	: movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = position1;
	worldUp = up;
	front = front1;
	yaw = yaw1;
	pitch = pitch1;
	UpdateCameraVectors(); // 更新摄像机的front, right, up向量	
}

Camera::Camera(double posX, double posY, double posZ, double upX, double upY, double upZ,
	double frontX, double frontY, double frontZ, double yaw1, double pitch1) 
	: movementSpeed(SPEED), mouseSensitivity(SENSITIVITY), zoom(ZOOM)
{
	position = { posX, posY, posZ };
	worldUp = { upX, upY, upZ };
	front = { frontX, frontY, frontZ };
	yaw = yaw1;
	pitch = pitch1;
	UpdateCameraVectors();
}

Matrix4 Camera::GetViewMatrix()
{
	Vector3 target = position + front;
	return GraphicsLibrary::LookAt(position, up, target);
}

void Camera::ProcessKeyboard(CameraMovement direction, double deltaTime)
{
	double velocity = movementSpeed * deltaTime;
	if (direction == CameraMovement::FORWARD)
		position += front * velocity;
	if (direction == CameraMovement::BACKWARD)
		position -= front * velocity;
	if (direction == CameraMovement::LEFT)
		position -= right * velocity;
	if (direction == CameraMovement::RIGHT)
		position += right * velocity;
	if (direction == CameraMovement::UP)
		position += up * velocity;
	if (direction == CameraMovement::DOWN)
		position -= up * velocity;
}

void Camera::ProcessMouseMovement(double xoffset, double yoffset, bool constrainPitch)
{
	xoffset *= mouseSensitivity;
	yoffset *= mouseSensitivity;

	yaw += xoffset;
	pitch += yoffset;
	// 限制Pitch角度，防止镜头翻转
	if (constrainPitch)
	{
		if (pitch > 89.0f)
			pitch = 89.0f;
		if (pitch < -89.0f)
			pitch = -89.0f;
	}
	UpdateCameraVectors();
}

void Camera::ProcessMouseScroll(double yoffset)
{
	if (zoom >= 1.0f && zoom <= 45.0f)
		zoom -= yoffset;
	if (zoom <= 1.0f)
		zoom = 1.0f;
	if (zoom >= 45.0f)
		zoom = 45.0f;
}

void Camera::UpdateCameraVectors()
{
	// 计算摄像机的方向
	front.x = cos(yaw * PI / 180) * cos(pitch * PI / 180);
	front.y = sin(pitch * PI / 180);
	front.z = sin(yaw * PI / 180) * cos(pitch * PI / 180);
	front.Normalize();
	// 计算摄像机的右向和上向
	right = Vector3::Cross(front, worldUp);
	right.Normalize();
	up = Vector3::Cross(right, front);
	up.Normalize();
}