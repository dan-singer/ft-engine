#pragma once
#include "Component.h"
#include "Mesh.h"


// --------------------------------------------------------
// Wrapper for a shared Mesh resource
// --------------------------------------------------------
class MeshComponent : public Component
{
public:

	MeshComponent(Entity* entity) : Component(entity) {} 

	Mesh* m_mesh;

	virtual void Start() override;
	virtual void Tick(float deltaTime) override;

};

