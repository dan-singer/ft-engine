#include "Material.h"

Material::Material(
	SimpleVertexShader* vertexShader,
	SimplePixelShader* pixelShader,
	ID3D11ShaderResourceView* diffuseSRV,
	ID3D11ShaderResourceView* normalSRV,
	ID3D11SamplerState* samplerState,
	ID3D11BlendState* blendState,
	ID3D11DepthStencilState* depthStencilState
) :
	m_vertexShader(vertexShader), m_pixelShader(pixelShader),
	m_diffuseSRV(diffuseSRV), m_normalSRV(normalSRV), m_samplerState(samplerState),
	m_blendState(blendState), m_depthStencilState(depthStencilState)	
{

}
