#include "EmitterComponent.h"
#include "Transform.h"
#include "Entity.h"
#include <iostream>
#include <fstream>

using namespace DirectX;
using namespace rapidjson;

void EmitterComponent::UpdateSingleParticle(float deltaTime, int index)
{
	// Check for valid particle age before doing anything
	if (m_particles[index].Age >= m_lifetime)
		return;

	// Update and check for death
	m_particles[index].Age += deltaTime;
	if (m_particles[index].Age >= m_lifetime)
	{
		// Recent death, so retire by moving alive count
		m_firstAliveIndex++;
		m_firstAliveIndex %= m_maxParticles;
		m_livingParticleCount--;
		return;
	}

	// Calculate age percentage for lerp
	float agePercent = m_particles[index].Age / m_lifetime;

	// Interpolate the color
	XMStoreFloat4(
		&m_particles[index].Color,
		XMVectorLerp(
			XMLoadFloat4(&m_startColor),
			XMLoadFloat4(&m_endColor),
			agePercent));

	// Interpolate the rotation
	float rotStart = m_particles[index].RotationStart;
	float rotEnd = m_particles[index].RotationEnd;
	m_particles[index].Rotation = rotStart + agePercent * (rotEnd - rotStart);

	// Interpolate the size
	m_particles[index].Size = m_startSize + agePercent * (m_endSize - m_startSize);

	// Adjust the position
	XMVECTOR startPos = XMLoadFloat3(&m_particles[index].StartPosition);
	XMVECTOR startVel = XMLoadFloat3(&m_particles[index].StartVelocity);
	XMVECTOR accel = XMLoadFloat3(&m_emitterAcceleration);
	float t = m_particles[index].Age;

	// Use constant acceleration function
	XMStoreFloat3(
		&m_particles[index].Position,
		XMVectorAdd(XMVectorAdd(accel * t * t / 2.0f, startVel * t), startPos)
	);
}

void EmitterComponent::SpawnParticle()
{
	// Any left to spawn?
	if (m_livingParticleCount == m_maxParticles)
		return;

	// Reset the first dead particle
	m_particles[m_firstDeadIndex].Age = 0;
	m_particles[m_firstDeadIndex].Size = m_startSize;
	m_particles[m_firstDeadIndex].Color = m_startColor;

	Transform* transform = GetOwner()->GetTransform();
	m_particles[m_firstDeadIndex].StartPosition = transform->GetPosition();
	m_particles[m_firstDeadIndex].StartPosition.x += (((float)rand() / RAND_MAX) * 2 - 1) * m_positionRandomRange.x;
	m_particles[m_firstDeadIndex].StartPosition.y += (((float)rand() / RAND_MAX) * 2 - 1) * m_positionRandomRange.y;
	m_particles[m_firstDeadIndex].StartPosition.z += (((float)rand() / RAND_MAX) * 2 - 1) * m_positionRandomRange.z;
				
	m_particles[m_firstDeadIndex].Position = m_particles[m_firstDeadIndex].StartPosition;
				
	m_particles[m_firstDeadIndex].StartVelocity = m_startVelocity;
	m_particles[m_firstDeadIndex].StartVelocity.x += (((float)rand() / RAND_MAX) * 2 - 1) * m_velocityRandomRange.x;
	m_particles[m_firstDeadIndex].StartVelocity.y += (((float)rand() / RAND_MAX) * 2 - 1) * m_velocityRandomRange.y;
	m_particles[m_firstDeadIndex].StartVelocity.z += (((float)rand() / RAND_MAX) * 2 - 1) * m_velocityRandomRange.z;

	float rotStartMin = m_rotationRandomRanges.x;
	float rotStartMax = m_rotationRandomRanges.y;
	m_particles[m_firstDeadIndex].RotationStart = ((float)rand() / RAND_MAX) * (rotStartMax - rotStartMin) + rotStartMin;

	float rotEndMin = m_rotationRandomRanges.z;
	float rotEndMax = m_rotationRandomRanges.w;
	m_particles[m_firstDeadIndex].RotationEnd = ((float)rand() / RAND_MAX) * (rotEndMax - rotEndMin) + rotEndMin;

	// Increment and wrap
	m_firstDeadIndex++;
	m_firstDeadIndex %= m_maxParticles;

	m_livingParticleCount++;
}

void EmitterComponent::CopyParticlesToGPU(ID3D11DeviceContext* context, CameraComponent* camera)
{
	// Update local buffer (living particles only as a speed up)

	// Check cyclic buffer status
	if (m_firstAliveIndex < m_firstDeadIndex)
	{
		for (int i = m_firstAliveIndex; i < m_firstDeadIndex; i++)
			CopyOneParticle(i, camera);
	}
	else
	{
		// Update first half (from firstAlive to max particles)
		for (int i = m_firstAliveIndex; i < m_maxParticles; i++)
			CopyOneParticle(i, camera);

		// Update second half (from 0 to first dead)
		for (int i = 0; i < m_firstDeadIndex; i++)
			CopyOneParticle(i, camera);
	}

	// All particles copied locally - send whole buffer to GPU
	D3D11_MAPPED_SUBRESOURCE mapped = {};
	context->Map(m_vertexBuffer, 0, D3D11_MAP_WRITE_DISCARD, 0, &mapped);

	memcpy(mapped.pData, m_localParticleVertices, sizeof(ParticleVertex) * 4 * m_maxParticles);

	context->Unmap(m_vertexBuffer, 0);
}

DirectX::XMFLOAT3 EmitterComponent::ParseFloat3(rapidjson::Document& document, const char* key)
{
	auto& a = document[key];
	XMFLOAT3 float3 = XMFLOAT3(
		a[0].GetDouble(),
		a[1].GetDouble(),
		a[2].GetDouble()
	);
	return float3;
}

DirectX::XMFLOAT4 EmitterComponent::ParseFloat4(Document& document, const char* key)
{
	auto& a = document[key];
	XMFLOAT4 float4 = XMFLOAT4(
		a[0].GetDouble(),
		a[1].GetDouble(),
		a[2].GetDouble(),
		a[3].GetDouble()
	);
	return float4;
}

void EmitterComponent::InitInternal()
{
	m_timeSinceEmit = 0;
	m_livingParticleCount = 0;
	m_firstAliveIndex = 0;
	m_firstDeadIndex = 0;
	m_age = 0;

	m_particles = new Particle[m_maxParticles]{};
	// Create local particle vertices
	m_defaultUVs[0] = XMFLOAT2(0, 0);
	m_defaultUVs[1] = XMFLOAT2(1, 0);
	m_defaultUVs[2] = XMFLOAT2(1, 1);
	m_defaultUVs[3] = XMFLOAT2(0, 1);

	m_localParticleVertices = new ParticleVertex[4 * m_maxParticles];
	for (int i = 0; i < m_maxParticles * 4; i += 4) {
		m_localParticleVertices[i + 0].UV = m_defaultUVs[0];
		m_localParticleVertices[i + 1].UV = m_defaultUVs[1];
		m_localParticleVertices[i + 2].UV = m_defaultUVs[2];
		m_localParticleVertices[i + 3].UV = m_defaultUVs[3];
	}

	// Dynamic Vertex Buffer
	D3D11_BUFFER_DESC vbDesc = {};
	vbDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vbDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	vbDesc.Usage = D3D11_USAGE_DYNAMIC;
	vbDesc.ByteWidth = sizeof(ParticleVertex) * 4 * m_maxParticles;
	m_device->CreateBuffer(&vbDesc, 0, &m_vertexBuffer);

	unsigned int* indices = new unsigned int[m_maxParticles * 6];
	int indexCount = 0;
	for (int i = 0; i < m_maxParticles * 4; i += 4)
	{
		indices[indexCount++] = i;
		indices[indexCount++] = i + 1;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i;
		indices[indexCount++] = i + 2;
		indices[indexCount++] = i + 3;
	}
	D3D11_SUBRESOURCE_DATA indexData = {};
	indexData.pSysMem = indices;

	// Regular (static) index buffer
	D3D11_BUFFER_DESC ibDesc = {};
	ibDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	ibDesc.CPUAccessFlags = 0;
	ibDesc.Usage = D3D11_USAGE_DEFAULT;
	ibDesc.ByteWidth = sizeof(unsigned int) * m_maxParticles * 6;
	m_device->CreateBuffer(&ibDesc, &indexData, &m_indexBuffer);

	delete[] indices;
}

void EmitterComponent::CopyOneParticle(int index, CameraComponent* camera)
{
	int i = index * 4;

	m_localParticleVertices[i + 0].Position = CalcParticleVertexPosition(index, 0, camera);
	m_localParticleVertices[i + 1].Position = CalcParticleVertexPosition(index, 1, camera);
	m_localParticleVertices[i + 2].Position = CalcParticleVertexPosition(index, 2, camera);
	m_localParticleVertices[i + 3].Position = CalcParticleVertexPosition(index, 3, camera);
	
	m_localParticleVertices[i + 0].Color = m_particles[index].Color;
	m_localParticleVertices[i + 1].Color = m_particles[index].Color;
	m_localParticleVertices[i + 2].Color = m_particles[index].Color;
	m_localParticleVertices[i + 3].Color = m_particles[index].Color;
}

DirectX::XMFLOAT3 EmitterComponent::CalcParticleVertexPosition(int particleIndex, int quadCornerIndex, CameraComponent* camera)
{
	// Get the right and up vectors out of the view matrix
	// (Remember that it is probably already transposed)
	XMFLOAT4X4 view = camera->GetViewMatrix();
	XMVECTOR camRight = XMVectorSet(view._11, view._12, view._13, 0);
	XMVECTOR camUp = XMVectorSet(view._21, view._22, view._23, 0);

	// Determine the offset of this corner of the quad
	// Since the UV's are already set when the emitter is created, 
	// we can alter that data to determine the general offset of this corner
	XMFLOAT2 offset = m_defaultUVs[quadCornerIndex];
	offset.x = offset.x * 2 - 1;	// Convert from [0,1] to [-1,1]
	offset.y = (offset.y * -2 + 1);	// Same, but flip the Y

	// Load into a vector, which we'll assume is float3 with a Z of 0
	// Create a Z rotation matrix and apply it to the offset
	XMVECTOR offsetVec = XMLoadFloat2(&offset);
	XMMATRIX rotMatrix = XMMatrixRotationZ(m_particles[particleIndex].Rotation);
	offsetVec = XMVector3Transform(offsetVec, rotMatrix);

	// Add and scale the camera up/right vectors to the position as necessary
	XMVECTOR posVec = XMLoadFloat3(&m_particles[particleIndex].Position);
	posVec += camRight * XMVectorGetX(offsetVec) * m_particles[particleIndex].Size;
	posVec += camUp * XMVectorGetY(offsetVec) * m_particles[particleIndex].Size;

	// This position is all set
	XMFLOAT3 pos;
	XMStoreFloat3(&pos, posVec);
	return pos;
}

void EmitterComponent::Init(
	int maxParticles,
	int particlesPerSecond,
	float lifetime,
	float emitterLifetime,
	float startSize,
	float endSize,
	DirectX::XMFLOAT4 startColor,
	DirectX::XMFLOAT4 endColor,
	DirectX::XMFLOAT3 startVelocity,
	DirectX::XMFLOAT3 velocityRandomRange,
	DirectX::XMFLOAT3 emitterPosition,
	DirectX::XMFLOAT3 positionRandomRange,
	DirectX::XMFLOAT4 rotationRandomRange,
	DirectX::XMFLOAT3 emitterAcceleration,
	ID3D11Device* device
)
{
	Transform* transform = GetOwner()->GetTransform();

	m_maxParticles = maxParticles;
	m_particlesPerSecond = particlesPerSecond;
	m_secondsPerParticle = 1.0f / m_particlesPerSecond;
	m_lifetime = lifetime;
	m_emitterLifetime = emitterLifetime;
	m_startSize = startSize;
	m_endSize = endSize;
	m_startColor = startColor;
	m_endColor = endColor;
	m_startVelocity = startVelocity;
	m_velocityRandomRange = velocityRandomRange;
	transform->SetPosition(emitterPosition);
	m_positionRandomRange = positionRandomRange;
	m_rotationRandomRanges = rotationRandomRange;
	m_emitterAcceleration = emitterAcceleration;
	m_device = device;

	InitInternal();
}

void EmitterComponent::Init(const std::string& configPath, ID3D11Device* device)
{
	// Read the entire document into a string
	std::ifstream input(configPath);

	// @see https://stackoverflow.com/questions/2602013/read-whole-ascii-file-into-c-stdstring
	std::string fileContents((std::istreambuf_iterator<char>(input)), std::istreambuf_iterator<char>());

	// Use RapidJson to parse the configuration file
	Document document;
	document.Parse(fileContents.c_str());
	input.close();

	Transform* transform = GetOwner()->GetTransform();

	m_maxParticles = document["max-particles"].GetInt();
	m_particlesPerSecond = document["particles-per-second"].GetInt();
	m_secondsPerParticle = 1.0f / m_particlesPerSecond;
	m_lifetime = document["lifetime"].GetDouble();
	m_emitterLifetime = document["emitter-lifetime"].GetDouble();
	m_startSize = document["start-size"].GetDouble();
	m_endSize = document["end-size"].GetDouble();

	m_startColor = ParseFloat4(document, "start-color");
	m_endColor = ParseFloat4(document, "end-color");

	m_startVelocity = ParseFloat3(document, "start-velocity");
	m_velocityRandomRange = ParseFloat3(document, "velocity-random-range");
	transform->SetPosition(ParseFloat3(document, "emitter-position"));
	m_positionRandomRange = ParseFloat3(document, "position-random-range");
	m_rotationRandomRanges = ParseFloat4(document, "rotation-random-ranges");
	m_emitterAcceleration = ParseFloat3(document, "acceleration");

	m_device = device;
	InitInternal();
}

void EmitterComponent::Start()
{
}

void EmitterComponent::Tick(float deltaTime)
{
	m_age += deltaTime;
	// Update all particles - Check cyclic buffer first
	if (m_firstAliveIndex < m_firstDeadIndex)
	{
		for (int i = m_firstAliveIndex; i < m_firstDeadIndex; i++)
			UpdateSingleParticle(deltaTime, i);
	}
	else if (m_firstDeadIndex > m_firstDeadIndex)
	{
		// Update first half (from firstAlive to max particles)
		for (int i = m_firstAliveIndex; i < m_maxParticles; i++)
			UpdateSingleParticle(deltaTime, i);

		// Update second half (from 0 to first dead)
		for (int i = 0; i < m_firstDeadIndex; i++)
			UpdateSingleParticle(deltaTime, i);
	}


	if (m_age < m_emitterLifetime) {
		// Add to the time
		m_timeSinceEmit += deltaTime;

		// Enough time to emit?
		while (m_timeSinceEmit > m_secondsPerParticle)
		{
			SpawnParticle();
			m_timeSinceEmit -= m_secondsPerParticle;
		}
	}

}

EmitterComponent::~EmitterComponent()
{
	delete[] m_particles;
	delete[] m_localParticleVertices;
	m_vertexBuffer->Release();
	m_indexBuffer->Release();
}
