#pragma once
#define MAX_LIGHTS 128
#include <vector>
#include <string>
#include <Windows.h>
#include <d3d11.h>
#include "LightComponent.h"
class CameraComponent;
class Entity;

class World
{
private:
	std::vector<Entity*> m_entities;
	LightComponent::Light m_lights[MAX_LIGHTS];
	int m_activeLightCount = 0;
	World();
	// --------------------------------------------------------
	// Rebuilds the array of light structs that will be sent to the GPU.
	// --------------------------------------------------------
	void RebuildLights();

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

	// --------------------------------------------------------
	// Finds an Entity with the provided name
	// @returns Entity* the found Entity* or nullptr if not found
	// --------------------------------------------------------
	Entity* Find(const std::string& name);

	// --------------------------------------------------------
	// Finds the first entity containing the tag
	// @returns Entity* the found Entity* or nullptr if not found
	// --------------------------------------------------------
	Entity* FindWithTag(const std::string& tag);

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

