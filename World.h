#pragma once
#define MAX_LIGHTS 128
#include <vector>
#include <string>
#include <Windows.h>
#include <d3d11.h>
#include <map>
#include <bullet/btBulletDynamicsCommon.h>
#include "LightComponent.h"
#include "Mesh.h"
#include "SimpleShader.h"
#include "Material.h"
#include <set>
#include <queue>
#include <SpriteBatch.h>
#include <SpriteFont.h>
class CameraComponent;
class Entity;

// --------------------------------------------------------
// The World class is in charge of managing Entities and resources.
// Access it with the GetInstance method. 
// If you create anything with the world, you are not required to delete it.
// The World will keep track of everything and delete it for you.
// --------------------------------------------------------
class World
{
private:
	std::vector<Entity*> m_entities;
	std::map<std::string, Mesh*> m_meshes;
	std::map<std::string, SimpleVertexShader*> m_vertexShaders;
	std::map<std::string, SimplePixelShader*> m_pixelShaders;
	std::map<std::string, Material*> m_materials;
	std::map<std::string, ID3D11ShaderResourceView*> m_SRVs;
	std::map<std::string, ID3D11SamplerState*> m_samplerStates;
	std::map<std::string, DirectX::SpriteBatch*> m_spriteBatches;
	std::map<std::string, DirectX::SpriteFont*> m_fonts;
	std::queue<Entity*> m_spawnQueue;
	std::queue<Entity*> m_destroyQueue;
	LightComponent::Light m_lights[MAX_LIGHTS];
	int m_activeLightCount = 0;

	// Bullet
	btDefaultCollisionConfiguration* m_collisionConfiguration;
	btCollisionDispatcher* m_dispatcher;
	btBroadphaseInterface* m_overlappingPairCache;
	btSequentialImpulseConstraintSolver* m_solver;
	btDiscreteDynamicsWorld* m_dynamicsWorld;
	btVector3 m_gravity = btVector3(0, -9.81f, 0);
	std::map<const btCollisionObject*, std::set<const btCollisionObject*>> m_collisionMap;

	World();
	// --------------------------------------------------------
	// Rebuilds the array of light structs that will be sent to the GPU.
	// --------------------------------------------------------
	void RebuildLights();

	// --------------------------------------------------------
	// Spawns entities in the spawn queue and destroys those 
	// in the destroy queue
	// --------------------------------------------------------
	void Flush();
public:
	CameraComponent* m_mainCamera = nullptr;

	// --------------------------------------------------------
	// Get the singleton instance of the world
	// --------------------------------------------------------
	static World* GetInstance();


	// --------------------------------------------------------
	// Returns a pointer to the Bullet library's world object
	// --------------------------------------------------------
	btDiscreteDynamicsWorld* GetPhysicsWorld() { return m_dynamicsWorld; }

	void SetGravity(btVector3 gravity);

	// --------------------------------------------------------
	// Create an Entity in the world. 
	// Note: you'll have to manually call Start on all of the components
	// After Instantiating an Entity.
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
	// Creates a pixel shader and adds it to the internal PS map
	// --------------------------------------------------------
	SimplePixelShader* CreatePixelShader(const std::string& name, ID3D11Device* device, ID3D11DeviceContext* context, LPCWSTR shaderFile);
	SimplePixelShader* GetPixelShader(const std::string& name);

	// --------------------------------------------------------
	// Creates a Material and adds it to the internal Materials map
	// --------------------------------------------------------
	Material* CreateMaterial(const std::string& name, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader,
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11SamplerState* samplerState);
	Material* GetMaterial(const std::string& name);

	// --------------------------------------------------------
	// Creates a shader resource view and returns it
	// --------------------------------------------------------
	ID3D11ShaderResourceView* CreateTexture(const std::string& name, ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* fileName);
	ID3D11ShaderResourceView* GetTexture(const std::string& name);

	// --------------------------------------------------------
	// Create a sampler state and store it in the internal map
	// --------------------------------------------------------
	ID3D11SamplerState* CreateSamplerState(const std::string& name, D3D11_SAMPLER_DESC* description, ID3D11Device* device);
	ID3D11SamplerState* GetSamplerState(const std::string& name);

	// --------------------------------------------------------
	// Create a SpriteBatch and store it in the internal map
	// --------------------------------------------------------
	DirectX::SpriteBatch* CreateSpriteBatch(const std::string& name, ID3D11DeviceContext* context);
	DirectX::SpriteBatch* GetSpriteBatch(const std::string& name);


	// --------------------------------------------------------
	// Create a SpriteFont and store it in the internal map
	// --------------------------------------------------------
	DirectX::SpriteFont* CreateFont(const std::string& name, ID3D11Device* device, const wchar_t* path);
	DirectX::SpriteFont* GetFont(const std::string& name);


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
	void DrawEntities(ID3D11DeviceContext* context, DirectX::SpriteBatch* spriteBatch, int screenWidth, int screenHeight);

	~World();
};

