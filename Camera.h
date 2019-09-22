#pragma once
#include <DirectXMath.h>

// --------------------------------------------------------
// Camera class which maintains its own view and projection
// matrices.
// --------------------------------------------------------
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
	DirectX::XMFLOAT3 GetDirection() { return m_direction; }

	// Setters
	void SetPosition(DirectX::XMFLOAT3 position) { m_position = position; }
	void SetDirection(DirectX::XMFLOAT3 direction) { m_direction = direction; }


	// --------------------------------------------------------
	// Rotate the camera by provided pitch and yaw in radians
	// --------------------------------------------------------
	void Rotate(float pitch, float yaw);

	// --------------------------------------------------------
	// Updates the camera's view matrix and handles input.
	// Call this once per frame.
	// TODO make this behave like Entity in that the view matrix 
	// is only updated if it is dirty
	// @param float deltaTime Time between frames
	// --------------------------------------------------------
	void Update(float deltaTime);

	// --------------------------------------------------------
	// Updates the projection matrix. Call this at the start 
	// of the game and whenever the screen resizes
	// @param float aspect width/height of screen
	// --------------------------------------------------------
	void UpdateProjectionMatrix(float aspect);
};

