#include "UITransform.h"

void UITransform::Init(Anchor anchor, float rotation, DirectX::XMFLOAT2 normalizedOrigin, DirectX::XMFLOAT2 dimensions)
{
	m_anchor = anchor;
	m_rotation = rotation;
	m_normalizedOrigin = normalizedOrigin;
	m_dimensions = dimensions;
}

void UITransform::Start()
{
}

void UITransform::Tick(float deltaTime)
{
}
