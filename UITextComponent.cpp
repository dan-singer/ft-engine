#include "UITextComponent.h"

void UITextComponent::Init(const std::string& text, DirectX::SpriteFont* font, DirectX::XMVECTOR color)
{
	m_text = text;
	m_font = font;
	m_color = color;
}

void UITextComponent::Start()
{
}

void UITextComponent::Tick(float deltaTime)
{
}
