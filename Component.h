#pragma once
#include "Entity.h"


// --------------------------------------------------------
// Abstract Component class which encapsulates state and 
// Behavior.
// --------------------------------------------------------
class Component
{
protected:
	Entity* m_owner;
public:
	// --------------------------------------------------------
	// Component Constructor. Typically this will not need 
	// to be overridden. Instead, see the Start method.
	// @param Entity * owner Entity who owns this component
	// --------------------------------------------------------
	Component(Entity* owner);

	// --------------------------------------------------------
	// Called at startup after all entities have been created.
	// --------------------------------------------------------
	virtual void Start() = 0;

	// --------------------------------------------------------
	// Called each frame.
	// @param float deltaTime time since last update
	// --------------------------------------------------------
	virtual void Tick(float deltaTime) = 0;

	virtual ~Component();
};

