#pragma once
#include "Component.h"
#include <DirectXMath.h>

enum class Anchor {
	TOP_LEFT,
	TOP_CENTER,
	TOP_RIGHT,
	CENTER_LEFT,
	CENTER_CENTER,
	CENTER_RIGHT,
	BOTTOM_LEFT,
	BOTTOM_CENTER,
	BOTTOM_RIGHT
};

// --------------------------------------------------------
// UITransform contains transformation data for rendering 
// 2D UI elements on the screen using SpriteBatch
// Use the regular Transform component to set position and scale
// --------------------------------------------------------
class UITransform : public Component
{
private:
	RECT m_bounds;
public:
	Anchor m_anchor = Anchor::TOP_LEFT;
	float m_rotation = 0.0f;
	DirectX::XMFLOAT2 m_normalizedOrigin = DirectX::XMFLOAT2(0, 0);

	UITransform(Entity* entity) : Component(entity) { }

	// --------------------------------------------------------
	// Helper method to set common UITransform fields in one method call
	// @param DirectX::XMFLOAT2 normalizedOrigin Origin with coordinates relative to width and height of texture in [0, 1] range
	// --------------------------------------------------------
	void Init(Anchor anchor, float rotation, DirectX::XMFLOAT2 normalizedOrigin, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 position);


	// --------------------------------------------------------
	// Caches the bounds. This has no effect on rendering.
	// --------------------------------------------------------
	void StoreBounds(RECT newBounds);

	RECT GetBounds();

	DirectX::XMFLOAT2 GetAnchorOrigin(int screenWidth, int screenHeight);

	virtual void Start() override;
	virtual void Tick(float deltaTime) override;

};

