#pragma once
#include <map>
#include "Entity.h"

template <class T>
class ComponentManager 
{
public:
	std::map<Entity*, T*> m_componentMap;

	static ComponentManager<T>* Get() {
		static ComponentManager<T> instance;
		return &instance;
	}
	T* GetComponent(Entity* entity) {
		if (m_componentMap.count(entity) > 0) {
			return m_componentMap[entity];
		}
		return nullptr;
	}
	T* AddComponent(Entity* entity) {
		m_componentMap[entity] = new T();
		return m_componentMap[entity];
	}
};


