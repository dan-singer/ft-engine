#include "CollisionTester.h"
#include "Entity.h"

void CollisionTester::Start()
{
}

void CollisionTester::Tick(float deltaTime)
{
}

void CollisionTester::OnMouseDown(WPARAM buttonState, int x, int y)
{
	GetOwner()->GetRigidBody()->ApplyImpulse(btVector3(0, 10, 0));
}

void CollisionTester::OnCollisionBegin(Entity* other)
{
	printf("Began colliding with %s\n", other->GetName().c_str());
}

void CollisionTester::OnCollisionStay(Entity* other)
{
	printf("Colliding with %s\n", other->GetName().c_str());
}

void CollisionTester::OnCollisionEnd(Entity* other)
{
	printf("Stopped colliding with %s\n", other->GetName().c_str());
}
