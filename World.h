#pragma once
#define MAX_LIGHTS 128
#include <vector>
#include <string>
#include <Windows.h>
#include <d3d11.h>
#include <map>
#include "LightComponent.h"
#include "Mesh.h"
#include "SimpleShader.h"
class CameraComponent;
class Entity;

// --------------------------------------------------------
// The World class is in charge of managing Entities and resources.
// Access it with the GetInstance method.
// --------------------------------------------------------
class World
{
private:
	std::vector<Entity*> m_entities;
	std::map<std::string, Mesh*> m_meshes;
	std::map<std::string, SimpleVertexShader*> m_vertexShaders;
	std::map<std::string, SimplePixelShader*> m_pixelShaders;
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

	// --------------------------------------------------------
	// Destroys an Entity. It will not be rendered after this.
	// --------------------------------------------------------
	void Destroy(Entity* entity);

	// --------------------------------------------------------
	// Creates a mesh and adds it to the internal Mesh map
	// --------------------------------------------------------
	Mesh* CreateMesh(const std::string& name, Vertex* vertices, int numVertices, unsigned int* indices, int numIndices, ID3D11Device* device);
	Mesh* CreateMesh(const std::string& name, const char* file, ID3D11Device* device);
	Mesh* GetMesh(const std::string& name);

	// --------------------------------------------------------
	// Creates a vertex shader and adds it to the internal VS map
	// --------------------------------------------------------
	SimpleVertexShader* CreateVertexShader(const std::string& name, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile);
	SimpleVertexShader* GetVertexShader(const std::string& name);

	// --------------------------------------------------------
	// Creates a pixel shader and adds it to the internal VS map
	// --------------------------------------------------------
	SimplePixelShader* CreatePixelShader(const std::string& name, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile);
	SimplePixelShader* GetPixelShader(const std::string& name);

	// Lifecycle methods for Entities
	void OnMouseDown(WPARAM buttonState, int x, int y);
	void OnMouseUp(WPARAM buttonState, int x, int y);
	void OnMouseMove(WPARAM buttonState, int x, int y);
	void OnMouseWheel(float wheelDelta, int x, int y);
	void OnResize(int width, int height);
	void Start();
	void Tick(float deltaTime);


	// --------------------------------------------------------
	// Draws all entities using the device context
	// --------------------------------------------------------
	void DrawEntities(ID3D11DeviceContext* context);

	~World();
};

