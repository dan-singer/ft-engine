#include "RigidBodyComponent.h"
#include "Transform.h"
#include "Entity.h"

using namespace DirectX;

void RigidBodyComponent::SetBoxCollider(float halfX, float halfY, float halfZ)
{
	m_shape = new btBoxShape(btVector3((btScalar)halfX, (btScalar)halfY, (btScalar)halfZ));
}

void RigidBodyComponent::SetSphereCollider(float radius)
{
	m_shape = new btSphereShape((btScalar)radius);
}

void RigidBodyComponent::ApplyImpulse(DirectX::XMFLOAT3 impulse)
{
	// Convert to a btVector3
	btVector3 btForce(impulse.x, impulse.y, impulse.z);
	m_body->applyCentralImpulse(btForce);
}

void RigidBodyComponent::Start()
{
	// Shape should have been set by this point
	if (!m_shape) {
		throw "Use SetBoxCollider or SetSphereCollider to set a shape collider";
	}

	Transform* transform = GetOwner()->GetTransform();
	XMFLOAT3 pos = transform->GetPosition();
	XMFLOAT4 rot = transform->GetRotation();

	btTransform btTransform;
	// Copy the DirectX world matrix into the bullet transform, excluding scale, as bt does not support it
	btTransform.setOrigin(btVector3(pos.x, pos.y, pos.z));
	btTransform.setRotation(btQuaternion(rot.x, rot.y, rot.z, rot.w));

	bool isDynamic = m_mass != 0.0f;
	btVector3 localInertia(0, 0, 0);
	if (isDynamic) {
		m_shape->calculateLocalInertia(m_mass, localInertia);
	}

	// Setup the motion state
	m_motionState = new btDefaultMotionState(btTransform);
	btRigidBody::btRigidBodyConstructionInfo rbInfo(m_mass, m_motionState, m_shape, localInertia);
	m_body = new btRigidBody(rbInfo);

	// Embed a link back to this component
	m_body->setUserPointer((void*)GetOwner());

	World::GetInstance()->GetPhysicsWorld()->addRigidBody(m_body);
}

void RigidBodyComponent::Tick(float deltaTime)
{
	if (m_mass > 0) {
		// Retrieve the motion state's transform
		btTransform btTransform;
		m_motionState->getWorldTransform(btTransform);
		btVector3 origin = btTransform.getOrigin();
		btQuaternion rotation = btTransform.getRotation();

		// Copy over the btTransform to the Transform component
		Transform* transform = GetOwner()->GetTransform();
		transform->SetPosition(XMFLOAT3(origin.x(), origin.y(), origin.z()));
		transform->SetRotation(XMFLOAT4(rotation.x(), rotation.y(), rotation.z(), rotation.w()));
	}
	else {
		// If this is a static body, copy over the position from Transform into the bullet body
		Transform* transform = GetOwner()->GetTransform();
		XMFLOAT3 pos = transform->GetPosition();
		XMFLOAT4 rot = transform->GetRotation();

		btVector3 origin = btVector3(pos.x, pos.y, pos.z);
		btQuaternion orientation = btQuaternion(rot.x, rot.y, rot.z, rot.w);

		btTransform newTransform;
		newTransform.setOrigin(origin);
		newTransform.setRotation(orientation);

		m_body->setWorldTransform(newTransform);

	}


}

RigidBodyComponent::~RigidBodyComponent()
{
	// We can only delete the shape here because the other 
	// pointers to data on the heap must be taken care of in a specific order
	// which is done by the World's destructor
	delete m_shape;
}
