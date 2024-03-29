#include "World.h"
#include "Entity.h"
#include "CameraComponent.h"
#include "Entity.h"
#include <algorithm>
#include <iostream>
#include <WICTextureLoader.h>
#include "DDSTextureLoader.h"
#include "RigidBodyComponent.h"
#include "UITextComponent.h"

using namespace DirectX;

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

	// FMOD sound setup
	FMOD::System_Create(&m_soundSystem);
	m_soundSystem->init(36, FMOD_INIT_NORMAL, nullptr);
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
		toAdd->StartAllComponents();
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

		// Somehow, the thing we're trying to destroy may not actually be in the entities list. 
		// Guard against this condition
		auto destroyLoc = std::find(m_entities.begin(), m_entities.end(), toDestroy);
		if (destroyLoc != m_entities.end()) {
			m_entities.erase(destroyLoc);
			delete toDestroy;
		}
		m_destroyQueue.pop();
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

void World::DestroyAllEntities()
{
	for (Entity* entity : m_entities) {
		Destroy(entity);
	}
	m_mainCamera = nullptr;
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

Material* World::CreateMaterial(
	const std::string& name, SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader,
	ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11ShaderResourceView* reflectionSRV,
	ID3D11SamplerState* samplerState, ID3D11BlendState* blendState, ID3D11DepthStencilState* depthStencilState)
{
	Material* material = new Material(vertexShader, pixelShader, diffuseSRV, normalSRV, reflectionSRV, samplerState, blendState, depthStencilState);
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
	CreateWICTextureFromFile(device, context, fileName, 0, &m_SRVs[name]);
	return m_SRVs[name];
}

ID3D11ShaderResourceView* World::GetTexture(const std::string& name)
{
	return m_SRVs[name];
}

ID3D11ShaderResourceView* World::CreateCubeTexture(const std::string& name, ID3D11Device* device, ID3D11DeviceContext* context, const wchar_t* fileName)
{
	m_cubeSRVs[name] = nullptr;
	DirectX::CreateDDSTextureFromFile(device, context, fileName, 0, &m_cubeSRVs[name]);
	return m_cubeSRVs[name];
}
ID3D11ShaderResourceView* World::GetCubeTexture(const std::string& name)
{
	return m_cubeSRVs[name];
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

ID3D11RasterizerState* World::CreateRasterizerState(const std::string& name, D3D11_RASTERIZER_DESC* description, ID3D11Device* device)
{
	m_rastStates[name] = nullptr;
	device->CreateRasterizerState(description, &m_rastStates[name]);
	return m_rastStates[name];
}

ID3D11RasterizerState* World::GetRasterizerState(const std::string& name)
{
	return m_rastStates[name];
}

ID3D11DepthStencilState* World::CreateDepthStencilState(const std::string& name, D3D11_DEPTH_STENCIL_DESC* description, ID3D11Device* device)
{
	m_depthStencilStates[name] = nullptr;
	device->CreateDepthStencilState(description, &m_depthStencilStates[name]);
	return m_depthStencilStates[name];
}

ID3D11DepthStencilState* World::GetDepthStencilState(const std::string& name)
{
	return m_depthStencilStates[name];
}

ID3D11BlendState* World::CreateBlendState(const std::string& name, D3D11_BLEND_DESC* description, ID3D11Device* device)
{
	m_blendStates[name] = nullptr;
	device->CreateBlendState(description, &m_blendStates[name]);
	return m_blendStates[name];
}

ID3D11BlendState* World::GetBlendState(const std::string& name)
{
	return m_blendStates[name];
}

DirectX::SpriteBatch* World::CreateSpriteBatch(const std::string& name, ID3D11DeviceContext* context)
{
	SpriteBatch* spriteBatch = new DirectX::SpriteBatch(context);
	m_spriteBatches[name] = spriteBatch;
	return spriteBatch;
}

DirectX::SpriteBatch* World::GetSpriteBatch(const std::string& name)
{
	return m_spriteBatches[name];
}

DirectX::SpriteFont* World::CreateFont(const std::string& name, ID3D11Device* device, const wchar_t* path)
{
	SpriteFont* font = new SpriteFont(device, path);
	m_fonts[name] = font;
	return font;
}

DirectX::SpriteFont* World::GetFont(const std::string& name)
{
	return m_fonts[name];
}

FMOD::Sound* World::CreateSound(const std::string& name, const char* path)
{
	FMOD::Sound* newSound;
	m_soundSystem->createSound(path, FMOD_DEFAULT, 0, &newSound);
	m_sounds[name] = newSound;
	return newSound;
}

FMOD::Sound* World::GetSound(const std::string& name)
{
	return m_sounds[name];
}

void World::OnMouseDown(WPARAM buttonState, int x, int y)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			if (component->GetEnabled()) {
				component->OnMouseDown(buttonState, x, y);
			}
		}
	}
}

void World::OnMouseUp(WPARAM buttonState, int x, int y)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			if (component->GetEnabled()) {
				component->OnMouseUp(buttonState, x, y);
			}
		}
	}
}

void World::OnMouseMove(WPARAM buttonState, int x, int y)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			if (component->GetEnabled()) {
				component->OnMouseMove(buttonState, x, y);
			}
		}
	}
}

void World::OnMouseWheel(float wheelDelta, int x, int y)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			if (component->GetEnabled()) {
				component->OnMouseWheel(wheelDelta, x, y);
			}
		}
	}
}

void World::OnResize(int width, int height)
{
	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			if (component->GetEnabled()) {
				component->OnResize(width, height);
			}
		}
	}
}


void World::Tick(float deltaTime)
{
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
				if (component->GetEnabled()) {
					component->OnCollisionBegin(e1);
				}
			}
			for (Component* component : e1->GetAllComponents()) {
				if (component->GetEnabled()) {
					component->OnCollisionBegin(e0);
				}
			}
		}
		else if (m_collisionMap[body0].count(body1) == 0) {
			m_collisionMap[body0].insert(body1);
			for (Component* component : e0->GetAllComponents()) {
				if (component->GetEnabled()) {
					component->OnCollisionBegin(e1);
				}
			}
			for (Component* component : e1->GetAllComponents()) {
				if (component->GetEnabled()) {
					component->OnCollisionBegin(e0);
				}
			}
		}
		// Recurring collision callback
		else {
			// Collision callback triggered each frame of the collision
			for (Component* component : e0->GetAllComponents()) {
				if (component->GetEnabled()) {
					component->OnCollisionStay(e1);
				}
			}
			for (Component* component : e1->GetAllComponents()) {
				if (component->GetEnabled()) {
					component->OnCollisionStay(e0);
				}
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
					if (component->GetEnabled()) {
						component->OnCollisionEnd(e1);
					}
				}
				for (Component* component : e1->GetAllComponents()) {
					if (component->GetEnabled()) {
						component->OnCollisionEnd(e0);
					}
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
						if (component->GetEnabled()) {
							component->OnCollisionEnd(e1);
						}
					}
					for (Component* component : e1->GetAllComponents()) {
						if (component->GetEnabled()) {
							component->OnCollisionEnd(e0);
						}
					}
				}
			}
		}
	}
	// We need the map to look exactly like the snapshot now
	m_collisionMap = collisionSnapshot;

	for (Entity* entity : m_entities) {
		for (Component* component : entity->GetAllComponents()) {
			if (component->GetEnabled()) {
				component->Tick(deltaTime);
			}
		}
	}

	// Spawn and destroy entities **after** iterating through them
	Flush();
}

void World::DrawEntities(ID3D11DeviceContext* context, DirectX::SpriteBatch* spriteBatch, int screenWidth, int screenHeight)
{
	if (!m_mainCamera) {
		return;
	}

	RebuildLights();

	std::queue<Entity*> uiEntities;
	std::queue<Entity*> particleEntities;

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	//GetVertexShader("vs")
	for (Entity* entity : m_entities) {
		entity->GetTransform()->RecalculateWorldMatrix();

		// Delay rendering UI elements so they can be batched together
		if (entity->GetUITransform()) {
			uiEntities.push(entity);
		}
		// Delay rendering Particle Emitters 
		else if (entity->GetEmitter()) {
			particleEntities.push(entity);
		}
		// Render traditional 3D entities
		else if (entity->GetMesh() && entity->GetMaterial()) {
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

	//skyStuff

	context->RSSetState(m_rastStates["skyRastState"]);
	context->OMSetDepthStencilState(m_depthStencilStates["skyDepthState"], 0);

	ID3D11Buffer* skyVB = World::GetInstance()->GetMesh("cube")->GetVertexBuffer();
	ID3D11Buffer* skyIB = World::GetInstance()->GetMesh("cube")->GetIndexBuffer();

	context->IASetVertexBuffers(0, 1, &skyVB, &stride, &offset);
	context->IASetIndexBuffer(skyIB, DXGI_FORMAT_R32_UINT, 0);

	SimpleVertexShader* vsSky = GetVertexShader("vsSky");
	vsSky->SetMatrix4x4("view", m_mainCamera->GetViewMatrix());
	vsSky->SetMatrix4x4("projection", m_mainCamera->GetProjectionMatrix());

	vsSky->CopyAllBufferData();
	vsSky->SetShader();

	SimplePixelShader* psSky = GetPixelShader("psSky");
	psSky->SetShader();
	psSky->SetShaderResourceView("skyTexture", GetCubeTexture("sky"));
	psSky->SetSamplerState("samplerOptions", GetSamplerState("main"));

	// Finally do the actual drawing
	context->DrawIndexed(GetMesh("cube")->GetIndexCount(), 0, 0);

	// Reset states for next frame
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
	// Particle Systems
	while (!particleEntities.empty()) {
		Entity* entity = particleEntities.front();
		EmitterComponent* emitter = entity->GetEmitter();
		particleEntities.pop();


		Material* particleMat = entity->GetMaterial();
		// Particle states
		float blend[4] = { 1,1,1,1 };
		context->OMSetBlendState(particleMat->GetBlendState(), blend, 0xffffffff);	// Additive blending
		context->OMSetDepthStencilState(particleMat->GetDepthStencilState(), 0); // No depth WRITING
		entity->PrepareParticleMaterial(m_mainCamera);

		// Draw the emitter
		emitter->CopyParticlesToGPU(context, m_mainCamera);

		// Set up buffers
		UINT stride = sizeof(ParticleVertex);
		UINT offset = 0;
		context->IASetVertexBuffers(0, 1, &emitter->m_vertexBuffer, &stride, &offset);
		context->IASetIndexBuffer(emitter->m_indexBuffer, DXGI_FORMAT_R32_UINT, 0);

		// Draw the correct parts of the buffer
		if (emitter->m_firstAliveIndex < emitter->m_firstDeadIndex)
		{
			context->DrawIndexed(emitter->m_livingParticleCount * 6, emitter->m_firstAliveIndex * 6, 0);
		}
		else if (emitter->m_firstAliveIndex > emitter->m_firstDeadIndex)
		{
			// Draw first half (0 -> dead)
			context->DrawIndexed(emitter->m_firstDeadIndex * 6, 0, 0);

			// Draw second half (alive -> max)
			context->DrawIndexed((emitter->m_maxParticles - emitter->m_firstAliveIndex) * 6, emitter->m_firstAliveIndex * 6, 0);
		}

		// Reset to default states for next frame
		context->OMSetBlendState(0, blend, 0xffffffff);
		context->OMSetDepthStencilState(0, 0);
		context->RSSetState(0);
	}

	spriteBatch->Begin(SpriteSortMode_Deferred, m_states->NonPremultiplied());
	while (!uiEntities.empty()) {
		Entity* entity = uiEntities.front();
		uiEntities.pop();

		Transform* transform = entity->GetTransform();
		DirectX::XMFLOAT3 pos = transform->GetPosition();
		UITransform* uiTransform = entity->GetUITransform();

		XMFLOAT2 anchorOrigin = uiTransform->GetAnchorOrigin(screenWidth, screenHeight);
		XMFLOAT2 finalPosition = XMFLOAT2(anchorOrigin.x + pos.x, anchorOrigin.y + pos.y);

		XMFLOAT3 scale3 = transform->GetScale();
		XMFLOAT2 scale2(scale3.x, scale3.y);

		Material* mat = entity->GetMaterial();
		if (mat) {

			ID3D11ShaderResourceView* srv = mat->GetDiffuse();

			ID3D11Resource* resource;
			srv->GetResource(&resource);

			CD3D11_TEXTURE2D_DESC texDesc;
			ID3D11Texture2D* tex = static_cast<ID3D11Texture2D*>(resource);
			tex->GetDesc(&texDesc);

			tex->Release();

			XMFLOAT2 origin = XMFLOAT2(uiTransform->m_normalizedOrigin.x * texDesc.Width,
				uiTransform->m_normalizedOrigin.y * texDesc.Height);

			RECT bounds;
			bounds.left = finalPosition.x - (origin.x * scale2.x);
			bounds.right = bounds.left + (texDesc.Width * scale2.x);
			bounds.top = finalPosition.y - (origin.y * scale2.y);
			bounds.bottom = bounds.top + (texDesc.Height * scale2.y);
			uiTransform->StoreBounds(bounds);

			spriteBatch->Draw(srv, finalPosition, nullptr, Colors::White, uiTransform->m_rotation, origin, scale2);
		}
		UITextComponent* uiText = entity->GetComponent<UITextComponent>();
		if (uiText) {
			XMVECTOR dimensions = uiText->m_font->MeasureString(uiText->m_text.c_str());
			XMFLOAT2 dimensionsData;
			XMStoreFloat2(&dimensionsData, dimensions);
			XMFLOAT2 origin = XMFLOAT2(
				uiTransform->m_normalizedOrigin.x * dimensionsData.x,
				uiTransform->m_normalizedOrigin.y * dimensionsData.y
			);

			RECT bounds;
			bounds.left = finalPosition.x - (origin.x * scale2.x);
			bounds.right = bounds.left + (dimensionsData.x * scale2.x);
			bounds.top = finalPosition.y - (origin.y * scale2.y);
			bounds.bottom = bounds.top + (dimensionsData.y * scale2.y);
			uiTransform->StoreBounds(bounds);

			uiText->m_font->DrawString(
				spriteBatch, uiText->m_text.c_str(), finalPosition,
				uiText->m_color, uiTransform->m_rotation, origin, scale2
			);
		}
	}
	spriteBatch->End();

	// Reset any states that may be changed by sprite batch!
	float blendFactor[4] = { 1,1,1,1 };
	context->OMSetBlendState(0, blendFactor, 0xFFFFFFFF);
	context->RSSetState(0);
	context->OMSetDepthStencilState(0, 0);
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
	delete m_states;


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
	for (const auto& pair : m_fonts) {
		delete pair.second;
	}
	for (const auto& pair : m_SRVs) {
		pair.second->Release();
	}
	for (const auto& pair : m_cubeSRVs) {
		pair.second->Release();
	}
	for (const auto& pair : m_samplerStates) {
		pair.second->Release();
	}
	for (const auto& pair : m_rastStates) {
		pair.second->Release();
	}
	for (const auto& pair : m_depthStencilStates) {
		pair.second->Release();
	}
	for (const auto& pair : m_blendStates) {
		pair.second->Release();
	}
	for (const auto& pair : m_spriteBatches) {
		delete pair.second;
	}
	for (const auto& pair : m_sounds) {
		pair.second->release();
	}
	m_soundSystem->release();
}