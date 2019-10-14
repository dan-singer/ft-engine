#include "Entity.h"

Entity::Entity(const std::string& name) : m_name(name)
{
	m_transform = AddComponent<Transform>();
}

Entity::~Entity()
{
	for (Component* component : m_components) {
		delete component;
	}
}

