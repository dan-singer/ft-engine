#pragma once
#include "Component.h"
#include <DirectXMath.h>

// --------------------------------------------------------
// General light component that can be attached to an entity
// Position and direction are auto-acquired from the Transform
// --------------------------------------------------------
class LightComponent : public Component
{

public:
	enum Type
	{
		Directional = 0,
		Point,
		Spot
	};

	// --------------------------------------------------------
	// Light Structure that gets passed to the GPU
	// --------------------------------------------------------
	struct Light {
		Type type;
		DirectX::XMFLOAT3 direction = DirectX::XMFLOAT3(0,0,1); // 16 Bytes
		float range = 5.0f;
		DirectX::XMFLOAT3 position = DirectX::XMFLOAT3(0,0,0); // 32 Bytes
		float intensity = 1.0f;
		DirectX::XMFLOAT3 color = DirectX::XMFLOAT3(1,1,1); // 48 Bytes
		float spotFalloff = 1.0f;
		DirectX::XMFLOAT3 padding; //64 bytes
	};

	Light m_data;

	LightComponent(Entity* entity) : Component(entity) { }

	virtual void Start() override;


	virtual void Tick(float deltaTime) override;

};

