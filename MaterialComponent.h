#pragma once
#include "Component.h"
#include "Material.h"

// --------------------------------------------------------
// Wrapper for a Material object
// --------------------------------------------------------
class MaterialComponent : public Component
{
public:
	Material* m_material;

	MaterialComponent(Entity* entity) : Component(entity) { }

	virtual void Start() override;

	virtual void Tick(float deltaTime) override;

};

