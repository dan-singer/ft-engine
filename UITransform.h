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
// Make sure to set m_screenDimensions before using this
// --------------------------------------------------------
class UITransform : public Component
{
public:
	Anchor m_anchor = Anchor::TOP_LEFT;
	float m_rotation = 0.0f;
	DirectX::XMFLOAT2 m_normalizedOrigin = DirectX::XMFLOAT2(0, 0);
	DirectX::XMFLOAT2 m_dimensions = DirectX::XMFLOAT2(0, 0);

	UITransform(Entity* entity) : Component(entity) { }

	void Init(Anchor anchor, float rotation, DirectX::XMFLOAT2 normalizedOrigin, DirectX::XMFLOAT2 dimensions);

	virtual void Start() override;
	virtual void Tick(float deltaTime) override;

};

