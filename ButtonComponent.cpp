#include "ButtonComponent.h"
#include "Entity.h"
#include "Transform.h"
#include "UITransform.h"

// @see https://developer.mozilla.org/en-US/docs/Games/Techniques/3D_collision_detection
bool ButtonComponent::MouseInBounds()
{
	RECT bounds = GetOwner()->GetUITransform()->GetBounds();
	return (m_mousePos.x >= bounds.left && m_mousePos.x <= bounds.right) &&
		(m_mousePos.y >= bounds.top && m_mousePos.y <= bounds.bottom);
}


void ButtonComponent::AddOnClick(Action action)
{
	m_onClick.push_back(action);
}

void ButtonComponent::AddOnEnter(Action action)
{
	m_onEnter.push_back(action);
}

void ButtonComponent::AddOnExit(Action action)
{
	m_onExit.push_back(action);

}

void ButtonComponent::Start()
{

}

void ButtonComponent::Tick(float deltaTime)
{
	bool inBounds = MouseInBounds();
	if (inBounds && !m_wasInBounds) {
		for (Action action : m_onEnter) {
			action();
		}
	}
	else if (!inBounds && m_wasInBounds) {
		for (Action action : m_onExit) {
			action();
		}
	}

	m_wasInBounds = MouseInBounds();
}

void ButtonComponent::OnMouseMove(WPARAM buttonState, int x, int y)
{
	m_mousePos.x = x;
	m_mousePos.y = y;
}

void ButtonComponent::OnMouseDown(WPARAM buttonState, int x, int y)
{
	if (MouseInBounds()) {
		for (Action action : m_onClick) {
			action();
		}
	}
}
