#pragma once
#include "Component.h"
#include "Mesh.h" // TODO turn Mesh into a component
#include "Material.h"
// --------------------------------------------------------
// Transform Component. Maintains position, rotation, and scale.
// --------------------------------------------------------
class Transform : public Component
{
private:
	DirectX::XMFLOAT4X4 m_world;
	DirectX::XMFLOAT3 m_position;
	DirectX::XMFLOAT3 m_scale;
	DirectX::XMFLOAT4 m_rotation;

	bool m_worldDirty = true;
public:
	Material* m_material;

	Transform(Entity* entity);

	// Getters
	DirectX::XMFLOAT3 GetPosition() { return m_position; }
	DirectX::XMFLOAT3 GetScale() { return m_scale; }
	DirectX::XMFLOAT4 GetRotation() { return m_rotation; }
	DirectX::XMFLOAT4X4 GetWorldMatrix() { return m_world; }

	// --------------------------------------------------------
	// Returns the forward vector of this entity
	// @returns DirectX::XMFLOAT3
	// --------------------------------------------------------
	DirectX::XMFLOAT3 GetForward();

	// Setters
	void SetPosition(DirectX::XMFLOAT3 position);
	void SetScale(DirectX::XMFLOAT3 scale);
	void SetRotation(DirectX::XMFLOAT4 rotation);

	// --------------------------------------------------------
	// Translate this entity
	// @param DirectX::XMFLOAT3 translation
	// @returns void
	// --------------------------------------------------------
	void Translate(DirectX::XMFLOAT3 translation);
	// --------------------------------------------------------
	// Scale this entity 
	// @param DirectX::XMFLOAT3 scaleAmt
	// @returns void
	// --------------------------------------------------------
	void Scale(DirectX::XMFLOAT3 scaleAmt);
	// --------------------------------------------------------
	// Rotate this entity. 
	// @param DirectX::XMFLOAT4 rotationAmt a quaternion
	// @returns void
	// --------------------------------------------------------
	void Rotate(DirectX::XMFLOAT4 rotationAmt);


	// --------------------------------------------------------
	// Recalculates the world matrix. Call this before drawing and after updating.
	// @param bool force Force an update to occur even if the world matrix isn't dirty
	// @returns void
	// --------------------------------------------------------
	void RecalculateWorldMatrix(bool force = false);

	// --------------------------------------------------------
	// Sets material's vertex and shader buffer data based on this entity and the camera.
	// Call this before drawing an entity
	// @param DirectX::XMFLOAT4X4 view View matrix to send to the vertex shader
	// @param DirectX::XMFLOAT4X4 projection Projection matrix to send to the vertex shader
	// @param DirectX::XMFLOAT3 cameraPos position of the camera in world space
	// --------------------------------------------------------
	void PrepareMaterial(DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection, DirectX::XMFLOAT3 cameraPos);


	virtual void Start() override;

	virtual void Tick(float deltaTime) override;

};

