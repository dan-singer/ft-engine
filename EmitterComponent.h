#pragma once
#include "Component.h"

#include <d3d11.h>
#include <DirectXMath.h>
#include <rapidjson/document.h>
#include "CameraComponent.h"

struct Particle
{
	DirectX::XMFLOAT4 Color;
	DirectX::XMFLOAT3 StartPosition;
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT3 StartVelocity;
	float Size;
	float Age;
	float RotationStart;
	float RotationEnd;
	float Rotation;
};

struct ParticleVertex
{
	DirectX::XMFLOAT3 Position;
	DirectX::XMFLOAT2 UV;
	DirectX::XMFLOAT4 Color;
};

// --------------------------------------------------------
// Particle Emitter Component
// Based on Chris Cascioli's example CPU particle system
// --------------------------------------------------------
class EmitterComponent : public Component
{
	friend class World;
private:
	// Emission Properties
	int m_maxParticles;
	int m_particlesPerSecond;
	float m_secondsPerParticle;
	float m_timeSinceEmit;
	int m_livingParticleCount;
	float m_age;

	float m_lifetime;
	float m_emitterLifetime;
	float m_startSize;
	float m_endSize;
	DirectX::XMFLOAT4 m_startColor;
	DirectX::XMFLOAT4 m_endColor;
	DirectX::XMFLOAT3 m_startVelocity;
	DirectX::XMFLOAT3 m_velocityRandomRange;
	DirectX::XMFLOAT3 m_positionRandomRange;
	DirectX::XMFLOAT4 m_rotationRandomRanges;
	DirectX::XMFLOAT3 m_emitterAcceleration;
	ID3D11Device* m_device;

	DirectX::XMFLOAT2 m_defaultUVs[4];

	Particle* m_particles;
	int m_firstDeadIndex;
	int m_firstAliveIndex;

	// Rendering
	ParticleVertex* m_localParticleVertices;
	ID3D11Buffer* m_vertexBuffer;
	ID3D11Buffer* m_indexBuffer;

	void UpdateSingleParticle(float deltaTime, int index);
	void SpawnParticle();
	void CopyOneParticle(int index, CameraComponent* camera);
	DirectX::XMFLOAT3 CalcParticleVertexPosition(int particleIndex, int quadCornerIndex, CameraComponent* camera);
	void CopyParticlesToGPU(ID3D11DeviceContext* context, CameraComponent* camera);

	// --------------------------------------------------------
	// Parses a float3 out of a JSON array
	// --------------------------------------------------------
	DirectX::XMFLOAT3 ParseFloat3(rapidjson::Document& document, const char* key);

	// --------------------------------------------------------
	// Parses a float4 out of a JSON array
	// --------------------------------------------------------
	DirectX::XMFLOAT4 ParseFloat4(rapidjson::Document& document, const char* key);


	// --------------------------------------------------------
	// Initialize private variables. This should be called from 
	// the public Init methods.
	// --------------------------------------------------------
	void InitInternal();

public:
	EmitterComponent(Entity* entity) : Component(entity) { }

	// --------------------------------------------------------
	// Setup method for this component.
	// --------------------------------------------------------
	void Init(
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
		DirectX::XMFLOAT4 rotationRandomRanges,
		DirectX::XMFLOAT3 emitterAcceleration,
		ID3D11Device* device
	);

	// --------------------------------------------------------
	// Setup method for this component accepting a configuration json file
	// @param const std::string & configPath path to json file
	// --------------------------------------------------------
	void Init(const std::string& configPath, ID3D11Device* device);

	virtual void Start() override;

	virtual void Tick(float deltaTime) override;

	virtual ~EmitterComponent();
};

