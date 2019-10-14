#include "CameraComponent.h"
#include "Entity.h"
#include <Windows.h>
using namespace DirectX;

CameraComponent::CameraComponent(Entity* entity) : Component(entity)
{
	XMStoreFloat4x4(&m_view, XMMatrixIdentity());
	XMStoreFloat4x4(&m_projection, XMMatrixIdentity());
}


void CameraComponent::Tick(float deltaTime)
{
	Transform* transform = GetOwner()->GetTransform();
	XMVECTOR upAxis = XMVectorSet(0, 1.0f, 0.0f, 1.0f);

	XMFLOAT3 pos = transform->GetPosition();
	XMFLOAT3 fwd = transform->GetForward();

	XMVECTOR globalForward = XMLoadFloat3(&fwd);
	XMMATRIX view = XMMatrixLookToLH(XMLoadFloat3(&pos), globalForward, upAxis);
	XMStoreFloat4x4(&m_view, XMMatrixTranspose(view));
}

void CameraComponent::UpdateProjectionMatrix(float aspect)
{
	// Create the Projection matrix
	// - This should match the window's aspect ratio, and also update anytime
	//    the window resizes (which is already happening in OnResize() below)
	XMMATRIX P = XMMatrixPerspectiveFovLH(
		0.25f * 3.1415926535f,		// Field of View Angle
		aspect,						// Aspect ratio
		0.1f,						// Near clip plane distance
		100.0f);					// Far clip plane distance
	XMStoreFloat4x4(&m_projection, XMMatrixTranspose(P)); // Transpose for HLSL!
}

void CameraComponent::Start()
{
}
