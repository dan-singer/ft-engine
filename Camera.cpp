#include "Camera.h"
#include <Windows.h>
using namespace DirectX;

Camera::Camera() : m_position(0,0,0), m_direction(0,0,0)
{
	XMStoreFloat4x4(&m_view, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projection, XMMatrixIdentity());
}

void Camera::Rotate(float pitch, float yaw)
{
	m_pitch += pitch;
	m_yaw += yaw;
}

void Camera::Update(float deltaTime)
{
	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(m_pitch, m_yaw, 0);
	XMVECTOR forwardAxis = XMVectorSet(0, 0, 1.0f, 1.0f);
	XMVECTOR upAxis = XMVectorSet(0, 1.0f, 0.0f, 1.0f);

	XMVECTOR globalForward = XMVector3Rotate(forwardAxis, rotation);
	XMVECTOR globalRight = XMVector3Cross(globalForward, upAxis);
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&m_position), globalForward, upAxis);
	XMStoreFloat4x4(&m_view, XMMatrixTranspose(view));

	// Handle input
	// TODO create an InputManager Singleton
	// TODO make camera more general
	XMVECTOR position = XMLoadFloat3(&m_position);
	XMVECTOR translation = XMVectorSet(0, 0, 0, 0);
	if (GetAsyncKeyState('W') & 0x8000) {
		translation += globalForward;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		translation += -globalForward;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		translation += globalRight;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		translation += -globalRight;
	}
	if (GetAsyncKeyState(' ') & 0x8000) {
		translation += upAxis;
	}
	if (GetAsyncKeyState('X') & 0x8000) {
		translation += -upAxis;
	}
	translation = XMVector3Normalize(translation);
	translation *= deltaTime;
	XMVECTOR newPos = position + translation;
	XMStoreFloat3(&m_position, newPos);
}

void Camera::UpdateProjectionMatrix(float aspect)
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		aspect,						// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&m_projection, XMMatrixTranspose(P)); // Transpose for HLSL!
}
