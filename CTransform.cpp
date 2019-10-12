#include "CTransform.h"

using namespace DirectX;

CTransform::CTransform() : m_position(), m_scale(1, 1, 1) {
	XMStoreFloat4(&m_rotation, XMQuaternionIdentity());
	XMStoreFloat4x4(&m_world, XMMatrixIdentity());
}

DirectX::XMFLOAT3 CTransform::GetForward()
{
	XMVECTOR forwardVec = XMVector3Rotate(XMVectorSet(0, 0, 1, 0), XMLoadFloat4(&m_rotation));
	XMFLOAT3 forward;
	XMStoreFloat3(&forward, forwardVec);
	return forward;
}

void CTransform::SetPosition(DirectX::XMFLOAT3 position)
{
	m_position = position;
	m_worldDirty = true;
}

void CTransform::SetScale(DirectX::XMFLOAT3 scale)
{
	m_scale = scale;
	m_worldDirty = true;
}

void CTransform::SetRotation(XMFLOAT4 rotation)
{
	m_rotation = rotation;
	m_worldDirty = true;
}

void CTransform::Translate(DirectX::XMFLOAT3 translation)
{
	XMVECTOR position = XMLoadFloat3(&m_position);
	position += XMLoadFloat3(&translation);
	XMStoreFloat3(&m_position, position);
	m_worldDirty = true;
}

void CTransform::Scale(DirectX::XMFLOAT3 scaleAmt)
{
	XMVECTOR scale = XMLoadFloat3(&m_scale);
	scale += XMLoadFloat3(&scaleAmt);
	XMStoreFloat3(&m_scale, scale);
	m_worldDirty = true;
}

void CTransform::Rotate(DirectX::XMFLOAT4 rotationAmt)
{
	XMVECTOR rotation = XMLoadFloat4(&m_rotation);
	XMVECTOR newRotation = XMQuaternionMultiply(rotation, XMLoadFloat4(&rotationAmt));
	XMStoreFloat4(&m_rotation, newRotation);
	m_worldDirty = true;
}

void CTransform::RecalculateWorldMatrix(bool force)
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

void CTransform::PrepareMaterial(XMFLOAT4X4 view, XMFLOAT4X4 projection, DirectX::XMFLOAT3 cameraPos)
{
	SimpleVertexShader* vs = m_material->GetVertexShader();
	SimplePixelShader* ps = m_material->GetPixelShader();

	vs->SetMatrix4x4("world", GetWorldMatrix());
	vs->SetMatrix4x4("view", view);
	vs->SetMatrix4x4("projection", projection);
	vs->SetShader();
	vs->CopyAllBufferData();

	ps->SetFloat3("cameraPos", cameraPos);
	ps->SetSamplerState("samplerState", m_material->GetSamplerState());
	ps->SetShaderResourceView("diffuseTexture", m_material->GetShaderResourceView());
	ps->SetShader();
	ps->CopyAllBufferData();
}

