struct VertexToPixel
{
	float4 position				: SV_POSITION;
	float3 sampleDirection		: TEXCOORD;
};


TextureCube skyTexture		: register(t0);
SamplerState samplerOptions : register(s0);


float4 main(VertexToPixel input) : SV_TARGET
{
	return skyTexture.Sample(samplerOptions, input.sampleDirection);
}