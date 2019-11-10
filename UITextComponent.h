#pragma once
#include "Component.h"
#include <string>
#include <SpriteFont.h>
class UITextComponent : public Component
{
public:
	std::string m_text;
	DirectX::SpriteFont* m_font = nullptr;
	DirectX::XMVECTOR m_color = DirectX::Colors::White;

	void Init(const std::string& text, DirectX::SpriteFont* font, DirectX::XMVECTOR color);

	UITextComponent(Entity* entity) : Component(entity) { }

	virtual void Start() override;
	virtual void Tick(float deltaTime) override;

};

