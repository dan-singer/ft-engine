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
}

