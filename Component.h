#pragma once
class Entity;


// --------------------------------------------------------
// Abstract Component class which encapsulates state and 
// Behavior.
// --------------------------------------------------------
class Component
{
private:
	Entity* m_owner = nullptr;
public:
	// --------------------------------------------------------
	// Component Constructor. Do not change the parameters that 
	// this constructor accepts in subclasses!
	// @param Entity * owner Entity who owns this component
	// --------------------------------------------------------
	Component(Entity* owner);

	// --------------------------------------------------------
	// Called at startup **after** all entities have been created.
	// --------------------------------------------------------
	virtual void Start() = 0;

	// --------------------------------------------------------
	// Called each frame.
	// @param float deltaTime time since last update
	// --------------------------------------------------------
	virtual void Tick(float deltaTime) = 0;

	// --------------------------------------------------------
	// Returns the owner of this Component
	// @returns Entity*
	// --------------------------------------------------------
	Entity* GetOwner() { return m_owner; }

	virtual ~Component();
};

