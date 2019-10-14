#pragma once
#include "Component.h"
#include "Transform.h"
#include <DirectXMath.h>
#include <vector>
#include <string>
#include <unordered_set>
#include <algorithm>


// --------------------------------------------------------
// Base Entity class which contains a list of Components.
// Entities come with the Transform component by default.
// --------------------------------------------------------
class Entity
{
protected:
	std::vector<Component*> m_components;
	std::string m_name;
	std::unordered_set<std::string> m_tags;
public:
	Transform* m_transform = nullptr;

	Entity(const std::string& name);

	// --------------------------------------------------------
	// Add a component to this Entity. T must extend from Component!
	// @returns the new component
	// --------------------------------------------------------
	template <class T>
	T* AddComponent()
	{
		T* newComponent = new T(this);
		m_components.push_back(newComponent);
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

	virtual ~Entity();


};

