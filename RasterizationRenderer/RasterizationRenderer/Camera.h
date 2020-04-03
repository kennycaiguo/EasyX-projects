#pragma once

#include "GraphicsLibrary.h"

// 摄像机移动的方向枚举
enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// 摄像机的默认值
static const double YAW = -90.0;
static const double PITCH = 0.0;
static const double SPEED = 0.0025;
static const double SENSITIVITY = 0.1;
static const double ZOOM = 45.0;

class Camera
{
public:
	Camera(Vector3 position = { 0.0, 0.0, 0.0 }, Vector3 up = { 0.0, 1.0, 0.0 },
		Vector3 front = { 0.0, 0.0, -1.0 }, double yaw1 = YAW, double pitch1 = PITCH);
	Camera(double posX, double posY, double posZ, double upX, double upY, double upZ,
		double frontX, double frontY, double frontZ, double yaw1, double pitch1);
	
	Matrix4 GetViewMatrix();											// 计算返回观察矩阵
	void ProcessKeyboard(CameraMovement direction, double deltaTime);	// 处理键盘输入
	void ProcessMouseMovement(double xoffset, double yoffset,				// 处理鼠标输入
		bool constrainPitch = true);
	void ProcessMouseScroll(double yoffset);								// 处理鼠标滚轮事件

	// 摄像机属性
	Vector3 position;
	Vector3 front;
	Vector3 up;
	Vector3 right;
	Vector3 worldUp;
	// 欧拉角
	double yaw;
	double pitch;
	// 摄像机选项
	double movementSpeed;
	double mouseSensitivity;
	double zoom;

private:
	void UpdateCameraVectors();											// 通过欧拉角计算摄像机的方向
};