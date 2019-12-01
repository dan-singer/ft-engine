#include "Entity.h"

Entity::Entity(const std::string& name) : m_name(name)
{
	m_transform = AddComponent<Transform>();
}

void Entity::PrepareMaterial(DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection, DirectX::XMFLOAT3 cameraPos, LightComponent::Light lights[], int numLights)
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
	ps->SetShaderResourceView("diffuseTexture", material->GetDiffuse());
	ps->SetShaderResourceView("normalTexture", material->GetNormals());
	ps->SetShaderResourceView("reflectionTexture", material->GetReflectionSRV());
	ps->SetData("lights", lights, sizeof(LightComponent::Light) * MAX_LIGHTS);
	ps->SetInt("lightCount", numLights);
	ps->SetFloat("shininess", material->m_shiniess);
	ps->SetFloat("metalness", material->m_metalness);
	ps->SetFloat("roughness", material->m_roughness);
	ps->SetFloat3("specColor", material->m_specColor);
	ps->SetShader();
	ps->CopyAllBufferData();
}

void Entity::PrepareParticleMaterial(CameraComponent* camera)
{
	Material* material = GetMaterial();
	SimpleVertexShader* vs = material->GetVertexShader();
	SimplePixelShader* ps = material->GetPixelShader();

	vs->SetMatrix4x4("view", camera->GetViewMatrix());
	vs->SetMatrix4x4("projection", camera->GetProjectionMatrix());
	vs->SetShader();
	vs->CopyAllBufferData();

	ps->SetSamplerState("particleSampler", material->GetSamplerState());
	ps->SetShaderResourceView("particle", material->GetDiffuse());
	ps->SetShader();
	ps->CopyAllBufferData();
}

void Entity::StartAllComponents()
{
	if (!m_hasStarted) {
		for (Component* component : m_components) {
			component->Start();
		}
		m_hasStarted = true;
	}
}

Entity::~Entity()
{
	for (Component* component : m_components) {
		delete component;
	}
}

