#pragma once
#include "Component.h"
#include <DirectXMath.h>
class Rotator : public Component
{
public:
	DirectX::XMFLOAT3 eulerDelta;

	Rotator(Entity* entity) : Component(entity) { }

	virtual void Start() override;


	virtual void Tick(float deltaTime) override;

};

