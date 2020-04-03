#pragma once

#include "GraphicsLibrary.h"

// ������ƶ��ķ���ö��
enum class CameraMovement
{
	FORWARD,
	BACKWARD,
	LEFT,
	RIGHT,
	UP,
	DOWN
};

// �������Ĭ��ֵ
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
	
	Matrix4 GetViewMatrix();											// ���㷵�ع۲����
	void ProcessKeyboard(CameraMovement direction, double deltaTime);	// �����������
	void ProcessMouseMovement(double xoffset, double yoffset,				// �����������
		bool constrainPitch = true);
	void ProcessMouseScroll(double yoffset);								// �����������¼�

	// ���������
	Vector3 position;
	Vector3 front;
	Vector3 up;
	Vector3 right;
	Vector3 worldUp;
	// ŷ����
	double yaw;
	double pitch;
	// �����ѡ��
	double movementSpeed;
	double mouseSensitivity;
	double zoom;

private:
	void UpdateCameraVectors();											// ͨ��ŷ���Ǽ���������ķ���
};