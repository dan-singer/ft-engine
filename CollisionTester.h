#pragma once
#include "Component.h"

// This is a test component to make sure collisions are working
class CollisionTester : public Component
{
public:

	CollisionTester(Entity* entity) : Component(entity) { }

	virtual void Start() override;


	virtual void Tick(float deltaTime) override;


	virtual void OnMouseDown(WPARAM buttonState, int x, int y) override;


	virtual void OnCollisionBegin(Entity* other) override;


	virtual void OnCollisionStay(Entity* other) override;


	virtual void OnCollisionEnd(Entity* other) override;

};

