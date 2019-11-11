#pragma once
#include "Component.h"
#include <vector>
#include <DirectXMath.h>
#include <set>
#include <functional>

typedef std::function<void()> Action;

class ButtonComponent : public Component
{
private:
	std::vector<Action> m_onClick;
	std::vector<Action> m_onEnter;
	std::vector<Action> m_onExit;

	bool m_wasInBounds = false;

	DirectX::XMFLOAT2 m_mousePos;

	bool MouseInBounds();

public:
	ButtonComponent(Entity* entity) : Component(entity) { }

	void AddOnClick(Action action);
	void AddOnEnter(Action action);
	void AddOnExit(Action action);

	virtual void Start() override;
	virtual void Tick(float deltaTime) override;
	virtual void OnMouseMove(WPARAM buttonState, int x, int y) override;

	virtual void OnMouseDown(WPARAM buttonState, int x, int y) override;

};

