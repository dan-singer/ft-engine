#pragma once
#include <DirectXMath.h>
#include "Component.h"

// --------------------------------------------------------
// Camera class which maintains its own view and projection
// matrices.
// --------------------------------------------------------
class CameraComponent : public Component
{
private:
	DirectX::XMFLOAT4X4 m_view;
	DirectX::XMFLOAT4X4 m_projection;
public:
	CameraComponent(Entity* entity);

	// Getters
	DirectX::XMFLOAT4X4 GetViewMatrix() { return m_view; }
	DirectX::XMFLOAT4X4 GetProjectionMatrix() { return m_projection; }


	// --------------------------------------------------------
	// Updates the camera's view matrix and handles input.
	// Call this once per frame.
	// @param float deltaTime Time between frames
	// --------------------------------------------------------
	virtual void Tick(float deltaTime) override;

	// --------------------------------------------------------
	// Updates the projection matrix. Call this at the start 
	// of the game and whenever the screen resizes
	// @param float aspect width/height of screen
	// --------------------------------------------------------
	void UpdateProjectionMatrix(float aspect);

	virtual void Start() override;


	virtual void OnResize(int width, int height) override;

};

