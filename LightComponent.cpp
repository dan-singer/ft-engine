#include "LightComponent.h"
#include "Transform.h"
#include "Entity.h"

void LightComponent::Start()
{

}

void LightComponent::Tick(float deltaTime)
{
	Transform* transform = GetOwner()->GetTransform();
	m_data.position = transform->GetPosition();
	m_data.direction = transform->GetForward();
}
