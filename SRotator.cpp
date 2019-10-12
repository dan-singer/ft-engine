#include "SRotator.h"
#include "CTransform.h"
#include "ComponentManager.h"
#include <DirectXMath.h>

using namespace DirectX;

void SRotator::Update(std::vector<Entity*>& entities, float dt)
{
	for (Entity* entity : entities) {
		CTransform* transform = ComponentManager<CTransform>::Get()->GetComponent(entity);
		if (transform) {
			XMVECTOR vec = XMQuaternionRotationRollPitchYaw(dt, dt, 0);
			XMFLOAT4 dest;
			XMStoreFloat4(&dest, vec);
			transform->Rotate(dest);
		}
	}
}
