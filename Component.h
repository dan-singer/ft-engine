#pragma once
class Entity;
#include <Windows.h>
#include <bullet/btBulletDynamicsCommon.h>

// --------------------------------------------------------
// Abstract Component class which encapsulates state and 
// Behavior.
// --------------------------------------------------------
class Component
{
private:
	Entity* m_owner = nullptr;
	bool m_enabled = true;
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


	// Lifecycle Methods ///////////////////////////////////////////
	virtual void OnResize() { }
	virtual void OnMouseDown(WPARAM buttonState, int x, int y) { }
	virtual void OnMouseUp(WPARAM buttonState, int x, int y) { }
	virtual void OnMouseMove(WPARAM buttonState, int x, int y) { }
	virtual void OnMouseWheel(float wheelDelta, int x, int y) { }
	virtual void OnResize(int width, int height) { }
	///////////////////////////////////////////////////////////////
	
	// Collision Methods //////////////////////////////////////////
	virtual void OnCollisionBegin(Entity* other) { }
	virtual void OnCollisionStay(Entity* other) { }
	virtual void OnCollisionEnd(Entity* other) { }
	///////////////////////////////////////////////////////////////

	
	// --------------------------------------------------------
	// Returns the owner of this Component
	// @returns Entity*
	// --------------------------------------------------------
	Entity* GetOwner() { return m_owner; }

	// --------------------------------------------------------
	// Returns whether this component is enabled. 
	// Disabled components will not be updated or started.
	// --------------------------------------------------------
	bool GetEnabled() { return m_enabled; }
	void SetEnabled(bool enabled) { m_enabled = enabled; }

	virtual ~Component();
};

