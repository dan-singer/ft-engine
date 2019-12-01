

cbuffer externalData : register(b0)
{
	matrix view;
	matrix projection;
};

struct VertexShaderInput
{
	float3 position		: POSITION;     // XYZ position
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
};

struct VertexToPixel
{
	float4 position			: SV_POSITION;
	float3 sampleDirection  : TEXCOORD;
};

VertexToPixel main(VertexShaderInput input)
{
	// Set up output struct
	VertexToPixel output;

	matrix viewNoTranslation = view;
	viewNoTranslation._41 = 0;
	viewNoTranslation._42 = 0;
	viewNoTranslation._43 = 0;

	matrix viewProj = mul(viewNoTranslation, projection);
	output.position = mul(float4(input.position, 1.0f), viewProj);

	output.position.z = output.position.w;

	output.sampleDirection = input.position;

	return output;
}