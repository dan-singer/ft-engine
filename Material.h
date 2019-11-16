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
	ID3D11BlendState* m_blendState;
	ID3D11DepthStencilState* m_depthStencilState;
public:
	float m_shiniess = 128.0f;

	// --------------------------------------------------------
	// Construct a Material from shaders and textures
	// @param SimpleVertexShader * vertexShader
	// @param SimplePixelShader * pixelShader
	// @param ID3D11ShaderResourceView * shaderResourceView
	// @param ID3D11SamplerState * samplerState
	// --------------------------------------------------------
	Material(SimpleVertexShader* vertexShader,
		SimplePixelShader* pixelShader, 
		ID3D11ShaderResourceView* diffuseSRV,
		ID3D11ShaderResourceView* normalSRV,
		ID3D11SamplerState* samplerState,
		ID3D11BlendState* blendState = nullptr,
		ID3D11DepthStencilState* depthStencilState = nullptr
	);

	SimpleVertexShader* GetVertexShader() { return m_vertexShader; }
	SimplePixelShader*  GetPixelShader()  { return m_pixelShader;  }
	ID3D11ShaderResourceView* GetDiffuse() { return m_diffuseSRV; }
	ID3D11ShaderResourceView* GetNormals() { return m_normalSRV; }
	ID3D11SamplerState* GetSamplerState() { return m_samplerState; }
	ID3D11BlendState* GetBlendState() { return m_blendState; }
	ID3D11DepthStencilState* GetDepthStencilState() { return m_depthStencilState; }
};
