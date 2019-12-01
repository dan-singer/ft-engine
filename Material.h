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
	ID3D11ShaderResourceView* m_reflectionSRV;
	ID3D11SamplerState* m_samplerState;
	ID3D11BlendState* m_blendState;
	ID3D11DepthStencilState* m_depthStencilState;
public:
	float m_shiniess = 128.0f;
	float m_roughness = 0; //How rouch the object is 0 is a mirror
	float m_metalness = 0; //How metallic the object is 1 is a mirror
	DirectX::XMFLOAT3 m_specColor = DirectX::XMFLOAT3(0.04, 0.04, 0.04);
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
		ID3D11ShaderResourceView* reflectionSRV,
		ID3D11SamplerState* samplerState,
		ID3D11BlendState* blendState = nullptr,
		ID3D11DepthStencilState* depthStencilState = nullptr
	);

	SimpleVertexShader* GetVertexShader() { return m_vertexShader; }
	SimplePixelShader*  GetPixelShader()  { return m_pixelShader;  }
	ID3D11ShaderResourceView* GetDiffuse() { return m_diffuseSRV; }
	ID3D11ShaderResourceView* GetNormals() { return m_normalSRV; }
	ID3D11ShaderResourceView* GetReflectionSRV() { return m_reflectionSRV; }
	ID3D11SamplerState* GetSamplerState() { return m_samplerState; }
	ID3D11BlendState* GetBlendState() { return m_blendState; }
	ID3D11DepthStencilState* GetDepthStencilState() { return m_depthStencilState; }
};
