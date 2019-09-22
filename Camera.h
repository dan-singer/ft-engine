#pragma once
#include <DirectXMath.h>
class Camera
{
private:
	DirectX::XMFLOAT4X4 m_view;
	DirectX::XMFLOAT4X4 m_projection;
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_direction;
	float m_pitch = 0.0f;
	float m_yaw = 0.0f;
public:
	Camera();

	// Getters
	DirectX::XMFLOAT4X4 GetViewMatrix() { return m_view; }
	DirectX::XMFLOAT4X4 GetProjectionMatrix() { return m_projection; }
	DirectX::XMFLOAT3 GetPosition() { return m_position; }

	// Setters
	void SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }

	void Rotate(float pitch, float yaw);

	void Update(float deltaTime);
	void UpdateProjectionMatrix(float aspect);
};

