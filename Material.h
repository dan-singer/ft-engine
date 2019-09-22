#pragma once
#include "SimpleShader.h"
#include <DirectXMath.h>
class Material
{
private:
	SimpleVertexShader* m_vertexShader;
	SimplePixelShader* m_pixelShader;
public:
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader);

	SimpleVertexShader* GetVertexShader() { return m_vertexShader; }
	SimplePixelShader*  GetPixelShader()  { return m_pixelShader;  }
};

