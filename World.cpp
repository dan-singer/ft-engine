#include "World.h"
#include "Entity.h"
#include "CameraComponent.h"
#include "Entity.h"
#include <algorithm>
World* World::m_instance = nullptr;

World::World()
{

}

World* World::GetInstance()
{
	if (!m_instance) {
		m_instance = new World();
	}
	return m_instance;
}

Entity* World::Instantiate(const std::string& name)
{
	Entity* entity = new Entity(name);
	m_entities.push_back(entity);
	return entity;
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

void World::Start()
{
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
	// Set buffers in the input assembler
	//  - Do this ONCE PER OBJECT you're drawing, since each object might
	//    have different geometry.
	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	for (Entity* entity : m_entities) {
		entity->GetTransform()->RecalculateWorldMatrix();
		if (entity->GetMesh() && entity->GetMaterial()) {
			entity->PrepareMaterial(m_mainCamera->GetViewMatrix(), m_mainCamera->GetProjectionMatrix(), m_mainCamera->GetOwner()->GetTransform()->GetPosition());
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
