#include "DebugMovement.h"
#include "Transform.h"
#include "Entity.h"

using namespace DirectX;

void DebugMovement::Start()
{
}

void DebugMovement::Tick(float deltaTime)
{
	Transform* transform = GetOwner()->GetTransform();

	XMFLOAT3 posData = transform->GetPosition();
	XMVECTOR position = XMLoadFloat3(&posData);

	XMFLOAT3 fwdData = transform->GetForward();
	XMVECTOR globalForward = XMLoadFloat3(&fwdData);

	XMFLOAT3 rightData = transform->GetRight();
	XMVECTOR globalRight = XMLoadFloat3(&rightData);

	XMVECTOR localUp = XMVectorSet(0, 1, 0, 0);

	XMVECTOR translation = XMVectorSet(0, 0, 0, 0);
	if (GetAsyncKeyState('W') & 0x8000) {
		translation += globalForward;
	}
	if (GetAsyncKeyState('S') & 0x8000) {
		translation += -globalForward;
	}
	if (GetAsyncKeyState('A') & 0x8000) {
		translation += globalRight;
	}
	if (GetAsyncKeyState('D') & 0x8000) {
		translation += -globalRight;
	}
	if (GetAsyncKeyState(' ') & 0x8000) {
		translation += localUp;
	}
	if (GetAsyncKeyState('X') & 0x8000) {
		translation += -localUp;
	}
	translation = XMVector3Normalize(translation);
	translation *= deltaTime;
	XMVECTOR newPos = XMVectorAdd(position, translation);

	XMFLOAT3 newPosData;
	XMStoreFloat3(&newPosData, newPos);
	transform->SetPosition(newPosData);
}

void DebugMovement::OnMouseMove(WPARAM buttonState, int x, int y)
{
	// Add any custom code here...
	int dx = x - prevMousePos.x;
	int dy = y - prevMousePos.y;

	XMFLOAT4 rotDeltaData;
	XMVECTOR rotDelta = XMQuaternionRotationRollPitchYaw(dy * .001f, dx * 0.001f, 0.0f);
	XMStoreFloat4(&rotDeltaData, rotDelta);
	GetOwner()->GetTransform()->Rotate(rotDeltaData);

	prevMousePos.x = x; 
	prevMousePos.y = y;
}
