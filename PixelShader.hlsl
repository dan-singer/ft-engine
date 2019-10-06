static const int lightCount = 1;
// Struct representing the data we expect to receive from earlier pipeline stages
// - Should match the output of our corresponding vertex shader
// - The name of the struct itself is unimportant
// - The variable names don't have to match other shaders (just the semantics)
// - Each variable must have a semantic, which defines its usage
struct VertexToPixel
{
	// Data type
	//  |
	//  |   Name          Semantic
	//  |    |                |
	//  v    v                v
	float4 position		: SV_POSITION;
	float3 normal		: NORMAL;
    float3 worldPos		: POSITION;
	float2 uv			: TEXCOORD;
};

struct DirectionalLight
{
	float4 AmbientColor;
	float4 DiffuseColor;
	float3 Direction;
    float __PADDING;
};

cbuffer externalData : register(b0)
{
    DirectionalLight lights[lightCount];
    float3 cameraPos;
};

Texture2D diffuseTexture : register(t0);
SamplerState samplerState : register(s0);

// --------------------------------------------------------
// The entry point (main method) for our pixel shader
// 
// - Input is the data coming down the pipeline (defined by the struct)
// - Output is a single color (float4)
// - Has a special semantic (SV_TARGET), which means 
//    "put the output of this into the current render target"
// - Named "main" because that's the default the shader compiler looks for5
// --------------------------------------------------------
float4 main(VertexToPixel input) : SV_TARGET
{
	input.normal = normalize(input.normal);
    float4 directionalColor = float4(0,0,0,1);
    float3 toCamera = normalize(cameraPos - input.worldPos);

	float4 surfaceColor = diffuseTexture.Sample(samplerState, input.uv);
	
    for (int i = 0; i < lightCount; ++i)
    {
		// Diffuse
        float3 toLight = normalize(-lights[i].Direction);
        float NdotL = saturate(dot(toLight, input.normal));
        float4 diffuse = surfaceColor * lights[i].DiffuseColor * NdotL;

		// Specular
        float3 h = normalize(toLight + toCamera);
        float NdotH = saturate(dot(input.normal, h));
		// TODO make shiniess based on a cbuffer variable, which should be set from a material
        float specAmt = pow(NdotH, 128.0f);
		
        directionalColor += lights[i].AmbientColor + diffuse + specAmt;
    }
	
    return directionalColor;


}