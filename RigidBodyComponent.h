#pragma once
#include "Component.h"
#include <bullet/btBulletDynamicsCommon.h>
#include <DirectXMath.h>

// --------------------------------------------------------
// Abstraction of Bullet physics to allow for easy
// Attaching of physics behaviors with a Component.
// --------------------------------------------------------
class RigidBodyComponent : public Component
{
private:
	btCollisionShape* m_shape = nullptr;
	btDefaultMotionState* m_motionState = nullptr;
	btRigidBody* m_body = nullptr;
public:

	float m_mass = 0.0f; // 0 indicates this is a static object

	btRigidBody* GetBody() { return m_body; }

	RigidBodyComponent(Entity* entity) : Component(entity) { }


	// --------------------------------------------------------
	// Sets the collider to a box shape
	// @param float halfX Half of the box's length in the x-direction
	// @param float halfY Half of the box's length in the y-direction
	// @param float halfZ Half of the box's length in the z-direction
	// --------------------------------------------------------
	void SetBoxCollider(float halfX, float halfY, float halfZ);

	// --------------------------------------------------------
	// Sets to the collider to a sphere shape
	// @param float radius radius of the sphere shape
	// --------------------------------------------------------
	void SetSphereCollider(float radius);


	// --------------------------------------------------------
	// Applies an impulse to this Entity. 
	// @param XMFLOAT3 impulse
	// --------------------------------------------------------
	void ApplyImpulse(DirectX::XMFLOAT3 impulse);


	virtual void Start() override;

	virtual void Tick(float deltaTime) override;

	~RigidBodyComponent();

};

