#pragma once
#include "SimpleShader.h"
#include <DirectXMath.h>

// --------------------------------------------------------
// Material class which is a container for a vertex and 
// pixel shader
// --------------------------------------------------------
class Material
{
private:
	SimpleVertexShader* m_vertexShader;
	SimplePixelShader* m_pixelShader;
public:
	// --------------------------------------------------------
	// Construct a Material from a vertex and pixel shader
	// @param SimpleVertexShader* vertexShader
	// @param SimplePixelShader* pixelShader
	// --------------------------------------------------------
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader);

	SimpleVertexShader* GetVertexShader() { return m_vertexShader; }
	SimplePixelShader*  GetPixelShader()  { return m_pixelShader;  }
};


// Material and Shader TODO
// 1. Implement point lights
// 2. Implement spot lights
// 3. Implement surface color in material class
