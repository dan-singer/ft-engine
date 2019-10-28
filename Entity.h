#pragma once
#include "Component.h"
#include "Transform.h"
#include "MeshComponent.h"
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>
#include "MaterialComponent.h"
#include "LightComponent.h"
#include "World.h"
#include "RigidBodyComponent.h"


// --------------------------------------------------------
// Base Entity class which contains a list of Components.
// Entities come with the Transform component by default.
// --------------------------------------------------------
class Entity
{
	friend class World;
protected:
	std::vector<Component*> m_components;
	std::string m_name;
	std::unordered_set<std::string> m_tags;

	// "Shortcut" references to common components 
	Transform* m_transform = nullptr;
	MeshComponent* m_meshComponent = nullptr;
	MaterialComponent* m_materialComponent = nullptr;
	RigidBodyComponent* m_rigidBodyComponent = nullptr;

	// Use the World to instantiate an Entity
	Entity(const std::string& name);
public:



	// --------------------------------------------------------
	// Add a component to this Entity. T must extend from Component!
	// @returns the new component
	// --------------------------------------------------------
	template <class T>
	T* AddComponent()
	{
		T* newComponent = new T(this);
		m_components.push_back(newComponent);

		// Use these to build references to the "shortcut" pointers
		MeshComponent* castedMesh = dynamic_cast<MeshComponent*>(newComponent);
		if (castedMesh) {
			m_meshComponent = castedMesh;
		}
		MaterialComponent* castedMaterial = dynamic_cast<MaterialComponent*>(newComponent);
		if (castedMaterial) {
			m_materialComponent = castedMaterial;
		}
		RigidBodyComponent* castedRB = dynamic_cast<RigidBodyComponent*>(newComponent);
		if (castedRB) {
			m_rigidBodyComponent = castedRB;
			// We need this component to be first in line, so that other components can use it at start
			// if they need access to it. So swap the back and front.
			Component* temp = m_components[0];
			m_components[0] = newComponent;
			m_components[m_components.size() - 1] = temp;
		}


		return newComponent;
	}

	// --------------------------------------------------------
	// Returns the attached component, or nullptr if it's not there.
	// --------------------------------------------------------
	template <class T>
	T* GetComponent()
	{
		for (int i = 0; i < m_components.size(); ++i) {
			T* castedComponent = dynamic_cast<T*>(m_components[i]);
			if (castedComponent) {
				return castedComponent;
			}
		}
		return nullptr;
	}

	// --------------------------------------------------------
	// Removes the component of type T from the Entity
	// --------------------------------------------------------
	template <class T>
	void RemoveComponent()
	{
		int indexToRemove = -1;
		for (int i = 0; i < m_components.size(); ++i) {
			if (dynamic_cast<T*>(m_components[i])) {
				indexToRemove = i;
				break;
			}
		}
		delete m_components[indexToRemove]; // Garbage collection
		m_components.erase(m_components.begin() + indexToRemove);
		
	}

	std::string GetName() { return m_name; }
	void SetName(const std::string& name) { m_name = name; }

	void AddTag(const std::string& tag) { m_tags.insert(tag); }
	bool HasTag(const std::string& tag) { return m_tags.count(tag) > 0; }
	void RemoveTag(const std::string& tag) { m_tags.erase(tag); }

	std::vector<Component*>& GetAllComponents() { return m_components; }

	Transform* GetTransform() { return m_transform; }

	RigidBodyComponent* GetRigidBody() { return m_rigidBodyComponent; }

	// --------------------------------------------------------
	// Returns the mesh component attached to this Entity.
	// Note that this CAN be nullptr if a mesh hasn't been attached.
	// --------------------------------------------------------
	Mesh* GetMesh() { return m_meshComponent ? m_meshComponent->m_mesh : nullptr; }

    // --------------------------------------------------------
	// Returns the material component attached to this Entity.
	// Note that this CAN be nullptr if a material hasn't been attached.
	// --------------------------------------------------------
	Material* GetMaterial() { return m_materialComponent ? m_materialComponent->m_material : nullptr; }


	// --------------------------------------------------------
	// Sets material's vertex and shader buffer data based on this entity and the camera.
	// Call this before drawing an entity
	// @param DirectX::XMFLOAT4X4 view View matrix to send to the vertex shader
	// @param DirectX::XMFLOAT4X4 projection Projection matrix to send to the vertex shader
	// @param DirectX::XMFLOAT3 cameraPos position of the camera in world space
	// --------------------------------------------------------
	void PrepareMaterial(DirectX::XMFLOAT4X4 view, DirectX::XMFLOAT4X4 projection, DirectX::XMFLOAT3 cameraPos, LightComponent::Light lights[], int numLights);


	// --------------------------------------------------------
	// Manually call start on all Components. Only use this if 
	// you've just manually instantiated an Entity.
	// --------------------------------------------------------
	void StartAllComponents();

	virtual ~Entity();


};

