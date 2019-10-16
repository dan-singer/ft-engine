#include "Rotator.h"
#include "Component.h"
#include "Entity.h"

using namespace DirectX;

void Rotator::Start()
{
}

void Rotator::Tick(float deltaTime)
{
	Transform* transform = GetOwner()->GetTransform();

	XMFLOAT4 rotationData;
	XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(eulerDelta.x * deltaTime, eulerDelta.y * deltaTime, eulerDelta.z * deltaTime);
	XMStoreFloat4(&rotationData, rotation);
	transform->Rotate(rotationData);
}
