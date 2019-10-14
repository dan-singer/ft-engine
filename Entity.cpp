#include "Entity.h"

Entity::Entity(const std::string& name) : m_name(name)
{
	m_transform = AddComponent<Transform>();
}

void Entity::PrepareMaterial(DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection, DirectX::XMFLOAT3 cameraPos)
{
	Material* material = GetMaterial();
	SimpleVertexShader* vs = material->GetVertexShader();
	SimplePixelShader* ps = material->GetPixelShader();

	vs->SetMatrix4x4("world", GetTransform()->GetWorldMatrix());
	vs->SetMatrix4x4("view", view);
	vs->SetMatrix4x4("projection", projection);
	vs->SetShader();
	vs->CopyAllBufferData();

	ps->SetFloat3("cameraPos", cameraPos);
	ps->SetSamplerState("samplerState", material->GetSamplerState());
	ps->SetShaderResourceView("diffuseTexture", material->GetShaderResourceView());
	ps->SetShader();
	ps->CopyAllBufferData();
}

Entity::~Entity()
{
	for (Component* component : m_components) {
		delete component;
	}
}

