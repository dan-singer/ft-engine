#include "UITransform.h"
#include "Transform.h"
#include "Entity.h"

using namespace DirectX;

void UITransform::Init(Anchor anchor, float rotation, DirectX::XMFLOAT2 normalizedOrigin, DirectX::XMFLOAT2 scale, DirectX::XMFLOAT2 position)
{
	Transform* transform = GetOwner()->GetTransform();
	m_anchor = anchor;
	m_rotation = rotation;
	m_normalizedOrigin = normalizedOrigin;
	transform->SetScale(XMFLOAT3(scale.x, scale.y, 1));
	transform->SetPosition(XMFLOAT3(position.x, position.y, 0));
}

void UITransform::StoreBounds(RECT newBounds)
{
	m_bounds = newBounds;
}

RECT UITransform::GetBounds()
{
	return m_bounds;
}

DirectX::XMFLOAT2 UITransform::GetAnchorOrigin(int screenWidth, int screenHeight)
{
	XMFLOAT2 anchorOrigin;
	switch (m_anchor)
	{
	case Anchor::TOP_LEFT:
	{
		anchorOrigin.x = 0;
		anchorOrigin.y = 0;
		break;
	}
	case Anchor::TOP_CENTER:
	{
		anchorOrigin.x = screenWidth / 2;
		anchorOrigin.y = 0;
		break;
	}
	case Anchor::TOP_RIGHT:
	{
		anchorOrigin.x = screenWidth;
		anchorOrigin.y = 0;
		break;
	}
	case Anchor::CENTER_LEFT:
	{
		anchorOrigin.x = 0;
		anchorOrigin.y = screenHeight / 2;
		break;
	}
	case Anchor::CENTER_CENTER:
	{
		anchorOrigin.x = screenWidth / 2;
		anchorOrigin.y = screenHeight / 2;
		break;
	}
	case Anchor::CENTER_RIGHT:
	{
		anchorOrigin.x = screenWidth;
		anchorOrigin.y = screenHeight / 2;
		break;
	}
	case Anchor::BOTTOM_LEFT:
	{
		anchorOrigin.x = 0;
		anchorOrigin.y = screenHeight;
		break;
	}
	case Anchor::BOTTOM_CENTER:
	{
		anchorOrigin.x = screenWidth / 2;
		anchorOrigin.y = screenHeight;
		break;
	}
	case Anchor::BOTTOM_RIGHT:
	{
		anchorOrigin.x = screenWidth;
		anchorOrigin.y = screenHeight;
		break;
	}
	}
	return anchorOrigin;
}

void UITransform::Start()
{
}

void UITransform::Tick(float deltaTime)
{
}
