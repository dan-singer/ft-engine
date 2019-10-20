#include "World.h"
#include "Entity.h"
#include "CameraComponent.h"
#include "Entity.h"
#include <algorithm>
#include <iostream>

World::World()
{

}

World* World::GetInstance()
{
	static World world;
	return &world;
}

void World::RebuildLights()
{
	m_activeLightCount = 0;
	for (Entity* entity : m_entities) {
		LightComponent* lightComp = entity->GetComponent<LightComponent>();
		if (lightComp) {
			m_lights[m_activeLightCount++] = lightComp->m_data;
		}
	}
}

Entity* World::Instantiate(const std::string& name)
{
	Entity* entity = new Entity(name);
	m_entities.push_back(entity);
	return entity;
}

Entity* World::Find(const std::string& name)
{
	for (Entity* entity : m_entities) {
		if (entity->GetName() == name) {
			return entity;
		}
	}
	return nullptr;
}

Entity* World::FindWithTag(const std::string& tag)
{
	for (Entity* entity : m_entities) {
		if (entity->HasTag(tag)) {
			return entity;
		}
	}
	return nullptr;
}

void World::Destroy(Entity* entity)
{
	delete entity;
	m_entities.erase(std::find(m_entities.begin(), m_entities.end(), entity));
}

Mesh* World::CreateMesh(const std::string& name, Vertex* vertices, int numVertices, unsigned int* indices, int numIndices, ID3D11Device* device)
{
	Mesh* mesh = new Mesh(vertices, numVertices, indices, numIndices, device);
	m_meshes[name] = mesh;
	return mesh;
}

Mesh* World::CreateMesh(const std::string& name, const char* file, ID3D11Device* device)
{
	Mesh* mesh = new Mesh(file, device);
	m_meshes[name] = mesh;
	return mesh;
}

Mesh* World::GetMesh(const std::string& name)
{
	return m_meshes[name];
}

SimpleVertexShader* World::CreateVertexShader(const std::string& name, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile)
{
	SimpleVertexShader* vs = new SimpleVertexShader(device, context);
	vs->LoadShaderFile(shaderFile);
	m_vertexShaders[name] = vs;
	return vs;
}

SimpleVertexShader* World::GetVertexShader(const std::string& name)
{
	return m_vertexShaders[name];
}

SimplePixelShader* World::CreatePixelShader(const std::string& name, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile)
{
	SimplePixelShader* ps = new SimplePixelShader(device, context);
	ps->LoadShaderFile(shaderFile);
	m_pixelShaders[name] = ps;
	return ps;
}

SimplePixelShader* World::GetPixelShader(const std::string& name)
{
	return m_pixelShaders[name];
}

void World::OnMouseDown(WPARAM buttonState, int x, int y)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			component->OnMouseDown(buttonState, x, y);
		}
	}
}

void World::OnMouseUp(WPARAM buttonState, int x, int y)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			component->OnMouseUp(buttonState, x, y);
		}
	}
}

void World::OnMouseMove(WPARAM buttonState, int x, int y)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			component->OnMouseMove(buttonState, x, y);
		}
	}
}

void World::OnMouseWheel(float wheelDelta, int x, int y)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			component->OnMouseWheel(wheelDelta, x, y);
		}
	}
}

void World::OnResize(int width, int height)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			component->OnResize(width, height);
		}
	}
}

void World::Start()
{
	RebuildLights();
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			component->Start();
		}
	}
}

void World::Tick(float deltaTime)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			component->Tick(deltaTime);
		}
	}
}

void World::DrawEntities(ID3D11DeviceContext* context)
{
	if (!m_mainCamera) {
		return;
	}

	RebuildLights();

	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	for (Entity* entity : m_entities) {
		entity->GetTransform()->RecalculateWorldMatrix();
		if (entity->GetMesh() && entity->GetMaterial()) {
			entity->PrepareMaterial(
				m_mainCamera->GetViewMatrix(), m_mainCamera->GetProjectionMatrix(), 
				m_mainCamera->GetOwner()->GetTransform()->GetPosition(), 
				m_lights, m_activeLightCount);
			ID3D11Buffer* entityVB = entity->GetMesh()->GetVertexBuffer();
			context->IASetVertexBuffers(0, 1, &entityVB, &stride, &offset);
			context->IASetIndexBuffer(entity->GetMesh()->GetIndexBuffer(), DXGI_FORMAT_R32_UINT, 0);
			context->DrawIndexed(entity->GetMesh()->GetIndexCount(), 0, 0);
		}
	}
}

World::~World()
{
	// Delete the entities
	for (Entity* entity : m_entities) {
		delete entity;
	}
	// Delete resources
	for (const auto& pair : m_meshes) {
		delete pair.second;
	}
	for (const auto& pair : m_vertexShaders) {
		delete pair.second;
	}
	for (const auto& pair : m_pixelShaders) {
		delete pair.second;
	}
}


