#include "Material.h"

Material::Material(
	SimpleVertexShader* vertexShader, SimplePixelShader* pixelShader, 
	ID3D11ShaderResourceView* shaderResourceView, ID3D11SamplerState* samplerState) :
	m_vertexShader(vertexShader), m_pixelShader(pixelShader),
	m_shaderResourceView(shaderResourceView), m_samplerState(samplerState)
{

}
