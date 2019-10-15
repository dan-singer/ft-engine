#pragma once
#include <vector>
#include <string>
#include <Windows.h>
#include <d3d11.h>
class CameraComponent;

class Entity;

class World
{
private:
	std::vector<Entity*> m_entities;
	static World* m_instance;
	World();
public:
	CameraComponent* m_mainCamera = nullptr;

	// --------------------------------------------------------
	// Get the singleton instance of the world
	// --------------------------------------------------------
	static World* GetInstance();

	// --------------------------------------------------------
	// Create an Entity in the world.
	// @param const std::string& name name of the entity
 	// @returns Entity* the created Entity pointer
	// --------------------------------------------------------
	Entity* Instantiate(const std::string& name);

	void Destroy(Entity* entity);

	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);

	void Start();
	void Tick(float deltaTime);
	void DrawEntities(ID3D11DeviceContext* content);

	~World();
};

