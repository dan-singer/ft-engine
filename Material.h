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
	ID3D11ShaderResourceView* m_diffuseSRV;
	ID3D11ShaderResourceView* m_normalSRV;
	ID3D11SamplerState* m_samplerState;
public:
	float m_shiniess = 128.0f;

	// --------------------------------------------------------
	// Construct a Material from shaders and textures
	// @param SimpleVertexShader * vertexShader
	// @param SimplePixelShader * pixelShader
	// @param ID3D11ShaderResourceView * shaderResourceView
	// @param ID3D11SamplerState * samplerState
	// --------------------------------------------------------
	Material(SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, 
		ID3D11ShaderResourceView* diffuseSRV, ID3D11ShaderResourceView* normalSRV, ID3D11SamplerState* samplerState);

	SimpleVertexShader* GetVertexShader() { return m_vertexShader; }
	SimplePixelShader*  GetPixelShader()  { return m_pixelShader;  }
	ID3D11ShaderResourceView* GetDiffuse() { return m_diffuseSRV; }
	ID3D11ShaderResourceView* GetNormals() { return m_normalSRV; }
	ID3D11SamplerState* GetSamplerState() { return m_samplerState; }
};


// Material and Shader TODO
// 1. Implement point lights
// 2. Implement spot lights
// 3. Implement surface color in material class
// 4. Implement Normal Mapping
