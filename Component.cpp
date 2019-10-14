#include "Component.h"

Component::Component(Entity* owner) : m_owner(owner)
{

}

Component::~Component()
{
	// Nothing to do right now...
}
