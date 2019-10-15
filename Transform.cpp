#include "Transform.h"
#include "Entity.h"

using namespace DirectX;

Transform::Transform(Entity* entity) : Component(entity)
{
	m_position = XMFLOAT3(0, 0, 0);
	m_scale = XMFLOAT3(1, 1, 1);
	XMStoreFloat4(&m_rotation, XMQuaternionIdentity());
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());
}

void Transform::Start()
{
}


DirectX::XMFLOAT3 Transform::GetForward()
{
	XMVECTOR forwardVec = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&m_rotation));
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, forwardVec);
	return forward;
}

DirectX::XMFLOAT3 Transform::GetRight()
{
	XMVECTOR rightVec = XMVector3Rotate(XMVectorSet(-1, 0, 0, 0), XMLoadFloat4(&m_rotation));
	XMFLOAT3 right;
	XMStoreFloat3(&right, rightVec);
	return right;
}

void Transform::SetPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
	m_worldDirty = true;
}

void Transform::SetScale(DirectX::XMFLOAT3 scale)
{
	m_scale = scale;
	m_worldDirty = true;
}

void Transform::SetRotation(XMFLOAT4 rotation)
{
	m_rotation = rotation;
	m_worldDirty = true;
}

void Transform::Translate(DirectX::XMFLOAT3 translation)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position += XMLoadFloat3(&translation);
	XMStoreFloat3(&m_position, position);
	m_worldDirty = true;
}

void Transform::Scale(DirectX::XMFLOAT3 scaleAmt)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale += XMLoadFloat3(&scaleAmt);
	XMStoreFloat3(&m_scale, scale);
	m_worldDirty = true;
}

void Transform::Rotate(DirectX::XMFLOAT4 rotationAmt)
{
	XMVECTOR rotation = XMLoadFloat4(&m_rotation);
	XMVECTOR newRotation = XMQuaternionMultiply(rotation, XMLoadFloat4(&rotationAmt));
	XMStoreFloat4(&m_rotation, newRotation);
	m_worldDirty = true;
}

void Transform::RecalculateWorldMatrix(bool force)
{
	if (force || m_worldDirty) {
		XMMATRIX translation = XMMatrixTranslationFromVector(XMLoadFloat3(&m_position));
		XMMATRIX rotation = XMMatrixRotationQuaternion(XMLoadFloat4(&m_rotation));
		XMMATRIX scale = XMMatrixScalingFromVector(XMLoadFloat3(&m_scale));

		XMMATRIX newWorld = scale * rotation * translation;

		XMStoreFloat4x4(&m_world, XMMatrixTranspose(newWorld));

		m_worldDirty = false;
	}
}

void Transform::Tick(float deltaTime)
{
}
