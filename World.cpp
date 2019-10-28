#include "World.h"
#include "Entity.h"
#include "CameraComponent.h"
#include "Entity.h"
#include <algorithm>
#include <iostream>
#include <WICTextureLoader.h>
#include "RigidBodyComponent.h"

World::World()
{
	// Bullet Physics Setup. See https://github.com/bulletphysics/bullet3/blob/master/examples/HelloWorld/HelloWorld.cpp
	///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
	m_collisionConfiguration = new btDefaultCollisionConfiguration();

	///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
	m_dispatcher = new btCollisionDispatcher(m_collisionConfiguration);

	///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
	m_overlappingPairCache = new btDbvtBroadphase();

	///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
	m_solver = new btSequentialImpulseConstraintSolver();

	m_dynamicsWorld = new btDiscreteDynamicsWorld(m_dispatcher, m_overlappingPairCache, m_solver, m_collisionConfiguration);

	m_dynamicsWorld->setGravity(m_gravity);
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

void World::Flush()
{
	while (!m_spawnQueue.empty()) {
		Entity* toAdd = m_spawnQueue.front();
		m_entities.push_back(toAdd);
		m_spawnQueue.pop();
	}

	while (!m_destroyQueue.empty()) {
		Entity* toDestroy = m_destroyQueue.front();

		// Remove destroyed entities from the collision map
		for (auto it = m_collisionMap.cbegin(); it != m_collisionMap.cend(); ) {
			auto& pair = *it;
			Entity* e0 = static_cast<Entity*>(pair.first->getUserPointer());
			if (toDestroy == e0) {
				m_collisionMap.erase(it++);
				continue;
			}
			else {
				for (const btCollisionObject* coll : m_collisionMap[pair.first]) {
					Entity* e1 = static_cast<Entity*>(coll->getUserPointer());
					if (toDestroy == e1) {
						m_collisionMap[pair.first].erase(coll);
						break;
					}
				}
			}
			++it;
		}


		// Rigidbodies need to be deleted separately
		RigidBodyComponent* rb = toDestroy->GetComponent<RigidBodyComponent>();
		if (rb) {
			btRigidBody* body = rb->GetBody();
			if (body && body->getMotionState()) {
				delete body->getMotionState();
			}
			m_dynamicsWorld->removeCollisionObject(body);
			delete body;
		}
		m_entities.erase(std::find(m_entities.begin(), m_entities.end(), toDestroy));
		m_destroyQueue.pop();



		delete toDestroy;
	}
}

void World::SetGravity(btVector3 gravity)
{
	m_gravity = gravity;
	m_dynamicsWorld->setGravity(gravity);
}

Entity* World::Instantiate(const std::string& name)
{
	Entity* entity = new Entity(name);
	m_spawnQueue.push(entity); // Hold off on adding to the internal vector until all iterations over it are finished
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
	m_destroyQueue.push(entity);
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

Material* World::CreateMaterial(const std::string& name, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, ID3D11ShaderResourceView* shaderResourceView, ID3D11SamplerState* samplerState)
{
	Material* material = new Material(vertexShader, pixelShader, shaderResourceView, samplerState);
	m_materials[name] = material;
	return material;
}

Material* World::GetMaterial(const std::string& name)
{
	return m_materials[name];
}

ID3D11ShaderResourceView* World::CreateTexture(const std::string& name, ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* fileName)
{
	m_SRVs[name] = nullptr;
	DirectX::CreateWICTextureFromFile(device, context, fileName, 0, &m_SRVs[name]);
	return m_SRVs[name];
}

ID3D11ShaderResourceView* World::GetTexture(const std::string& name)
{
	return m_SRVs[name];
}

ID3D11SamplerState* World::CreateSamplerState(const std::string& name, D3D11_SAMPLER_DESC* description, ID3D11Device* device)
{
	m_samplerStates[name] = nullptr;
	device->CreateSamplerState(description, &m_samplerStates[name]);
	return m_samplerStates[name];
}

ID3D11SamplerState* World::GetSamplerState(const std::string& name)
{
	return m_samplerStates[name];
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
	Flush();
	RebuildLights();
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			component->Start();
		}
	}
}

void World::Tick(float deltaTime)
{
	// Spawn and destroy entities **before** iterating through them
	Flush();

	// Simulate physics
	m_dynamicsWorld->stepSimulation(deltaTime, 10);

	// This represents collisions that occurred only during this frame
	// Used to determine when collisions ended
	std::map<const btCollisionObject*, std::set<const btCollisionObject*>> collisionSnapshot;

	// Dispatch collision events
	int numManifolds = m_dynamicsWorld->getDispatcher()->getNumManifolds();
	for (int i = 0; i < numManifolds; ++i) {
		btPersistentManifold* contactManifold = m_dynamicsWorld->getDispatcher()->getManifoldByIndexInternal(i);
		const btCollisionObject* body0 = contactManifold->getBody0();
		const btCollisionObject* body1 = contactManifold->getBody1();
		Entity* e0 = static_cast<Entity*>(body0->getUserPointer());
		Entity* e1 = static_cast<Entity*>(body1->getUserPointer());

		// Check if this is a new collision
		if (m_collisionMap.count(body0) == 0) {
			m_collisionMap[body0] = std::set<const btCollisionObject*>();
			for (Component* component : e0->GetAllComponents()) {
				component->OnCollisionBegin(e1);
			}
			for (Component* component : e1->GetAllComponents()) {
				component->OnCollisionBegin(e0);
			}
		}
		else if (m_collisionMap[body0].count(body1) == 0) {
			m_collisionMap[body0].insert(body1);
			for (Component* component : e0->GetAllComponents()) {
				component->OnCollisionBegin(e1);
			}
			for (Component* component : e1->GetAllComponents()) {
				component->OnCollisionBegin(e0);
			}
		}
		// Recurring collision callback
		else {
			// Collision callback triggered each frame of the collision
			for (Component* component : e0->GetAllComponents()) {
				component->OnCollisionStay(e1);
			}
			for (Component* component : e1->GetAllComponents()) {
				component->OnCollisionStay(e0);
			}
		}
		// Update the snapshot
		if (collisionSnapshot.count(body0) == 0) {
			collisionSnapshot[body0] = std::set<const btCollisionObject*>();
		}
		collisionSnapshot[body0].insert(body1);
	}
	// Take the difference collisionMap - snapshot
	// That value represents what's not being collided with anymore 
	for (const auto& pair : m_collisionMap) {
		Entity* e0 = static_cast<Entity*>(pair.first->getUserPointer());
		// If the snapshot doesn't contain the key, all of the things we've 
		// reported are colliding with it must not be colliding anymore
		if (collisionSnapshot.count(pair.first) == 0) {
			for (const btCollisionObject* coll : m_collisionMap[pair.first]) {
				Entity* e1 = static_cast<Entity*>(coll->getUserPointer());
				for (Component* component : e0->GetAllComponents()) {
					component->OnCollisionEnd(e1);
				}
				for (Component* component : e1->GetAllComponents()) {
					component->OnCollisionEnd(e0);
				}
			}
		}
		// Otherwise, check in the collision set for this key to see if there are any collisions
		// that the snapshot doesn't have. Again, these are collisions that just ended.
		else {
			for (const btCollisionObject* coll : m_collisionMap[pair.first]) {
				if (collisionSnapshot[pair.first].count(coll) == 0) {
					Entity* e1 = static_cast<Entity*>(coll->getUserPointer());
					for (Component* component : e0->GetAllComponents()) {
						component->OnCollisionEnd(e1);
					}
					for (Component* component : e1->GetAllComponents()) {
						component->OnCollisionEnd(e0);
					}
				}
			}
		}
	}
	// We need the map to look exactly like the snapshot now
	m_collisionMap = collisionSnapshot;

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
	// Delete Bullet resources
	for (int i = m_dynamicsWorld->getNumCollisionObjects() - 1; i >= 0; --i) {
		btCollisionObject* obj = m_dynamicsWorld->getCollisionObjectArray()[i];
		btRigidBody* body = btRigidBody::upcast(obj);
		if (body && body->getMotionState()) {
			delete body->getMotionState();
		}
		m_dynamicsWorld->removeCollisionObject(obj);
		delete obj;
	}
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_overlappingPairCache;
	delete m_dispatcher;
	delete m_collisionConfiguration;


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
	for (const auto& pair : m_materials) {
		delete pair.second;
	}
	for (const auto& pair : m_SRVs) {
		pair.second->Release();
	}
	for (const auto& pair : m_samplerStates) {
		pair.second->Release();
	}


}


