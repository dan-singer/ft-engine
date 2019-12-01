#define MAX_LIGHTS 128
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
	float2 uv			: TEXCOORD;
	float3 normal		: NORMAL;
	float3 tangent		: TANGENT;
	float3 worldPos		: POSITION;
};

struct LightStruct {
	int type;
	float3 direction; // 16 Bytes
	float range;
	float3 position; // 32 Bytes
	float intensity;
	float3 color; // 48 Bytes
	float spotFalloff;
	float3 padding; //64 bytes
};

cbuffer externalData : register(b0)
{
    LightStruct lights[MAX_LIGHTS];
    float3 cameraPos;
	int lightCount;
	float shininess;
	float metalness;
	float roughness;
	float3 specColor;
};

Texture2D diffuseTexture : register(t0);
Texture2D normalTexture : register(t1);
TextureCube reflectionTexture : register(t2);
SamplerState samplerState : register(s0);
static const float PI = 3.14159265f;

//MicrofacetBRDF
float SpecDistribution(float3 n, float3 h)
{
	//Pre-Calculations
	float NdotH = saturate(dot(n, h));
	float NdotH2 = NdotH * NdotH;
	float a = roughness * roughness;
	float a2 = max(a * a, 0);


	float denomToSquare = NdotH2 * (a2 - 1) + 1;

	return a2 / (PI * denomToSquare * denomToSquare);
}

float3 Fresnel(float3 v, float3 h, float3 f0)
{
	float VdotH = saturate(dot(v, h));

	return f0 + (1 - f0) * pow(1 - VdotH, 5);
}

float GeometricShadowing(float3 n, float3 v, float3 h)
{
	float k = pow(roughness + 1, 2) / 8.0f;
	float NdotV = saturate(dot(n, v));

	return NdotV / (NdotV * (1 - k) + k);
}


float3 microFacet(float D, float3 F, float G, float NdotL, float NdotV)
{
	return (D * F * G / (4 * max(NdotV, NdotL)));
}

float3 DiffuseEnergyConserve(float diffuse, float3 specular)
{
	return diffuse * ((1 - normalize(specular)) * (1 - metalness));
}

float4 directionalLight(int index, float4 surfaceColor, float3 normal, float3 toCamera) 
{
	// Diffuse
	float3 toLight = normalize(-lights[index].direction);
	float NdotL = saturate(dot(toLight, normal)) * lights[index].intensity;
	float NdotV = saturate(dot(toCamera, normal));
	float4 diffuse = surfaceColor * float4(lights[index].color, 1) * NdotL;

	// Specular
	float3 h = saturate((toLight + toCamera) / 2);
	
	float D = SpecDistribution(normal, h);
	float3 F = Fresnel(toCamera, h, specColor);
	float G = GeometricShadowing(normal, toCamera, h) * GeometricShadowing(normal, toLight, h);

	float3 specular = microFacet(D, F, G, NdotL, NdotV);
	float3 conservedEnergy = DiffuseEnergyConserve(diffuse, specular);

	return float4(conservedEnergy, 1);
}

float4 pointLight(int index, float4 surfaceColor, float3 normal, float3 toCamera, float3 worldPos)
{
	float3 lightDir = normalize(worldPos - lights[index].position);
	float NdotL = saturate(dot(normal, -lightDir)) * lights[index].intensity;
	float NdotV = saturate(dot(toCamera, normal));
	float4 diffuse = surfaceColor * float4(lights[index].color, 1) * NdotL;

	// Specular
	float3 h = normalize((-lightDir + toCamera) / 2);
	float NdotH = saturate(dot(normal, h));

	float D = SpecDistribution(normal, h);
	float3 F = Fresnel(toCamera, h, specColor);
	float G = GeometricShadowing(normal, toCamera, h) * GeometricShadowing(normal, lightDir, h);

	float3 specular = microFacet(D, F, G, NdotL, NdotV);
	float3 conservedEnergy = DiffuseEnergyConserve(diffuse, specular);

	// Range-based attenuation
	float dist = distance(worldPos, lights[index].position);
	float att = saturate(1.0 - dist * dist / (lights[index].range * lights[index].range));

	return att * float4(conservedEnergy, 1);
}

float4 spotLight(int index, float4 surfaceColor, float3 normal, float3 toCamera, float3 worldPos)
{
	float3 lightDir = normalize(worldPos - lights[index].position);

	float NdotL = saturate(dot(normal, -lightDir)) * lights[index].intensity;
	float NdotV = saturate(dot(toCamera, normal));
	float4 diffuse = surfaceColor * float4(lights[index].color, 1) * NdotL;

	float angleFromCenter = max(dot(-lightDir, lights[index].direction), 0.0f);
	float spotAmount = pow(angleFromCenter, lights[index].spotFalloff) * lights[index].intensity;

	// Specular
	float3 h = saturate((-lightDir + toCamera) / 2);
	float NdotH = saturate(dot(normal, h));

	float D = SpecDistribution(normal, h);
	float3 F = Fresnel(toCamera, h, specColor);
	float G = GeometricShadowing(normal, toCamera, h) * GeometricShadowing(normal, lightDir, h);

	float3 specular = microFacet(D, F, G, NdotL, NdotV);
	float3 conservedEnergy = DiffuseEnergyConserve(diffuse, specular);

	// Range-based attenuation
	float dist = distance(worldPos, lights[index].position);
	float att = saturate(1.0 - dist * dist / (lights[index].range * lights[index].range));

	return att * spotAmount * float4(conservedEnergy, 1);
}


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
	input.tangent = normalize(input.tangent);

    float4 finalColor = float4(0,0,0,1);
    float3 toCamera = normalize(cameraPos - input.worldPos);

	float4 surfaceColor = diffuseTexture.Sample(samplerState, input.uv);
	float4 normalVector = normalTexture.Sample(samplerState, input.uv);

	float3 refl = reflect(-toCamera, input.normal);
	float3 reflectionColor = reflectionTexture.Sample(samplerState, refl).rgb;

	// === Normal mapping here!  We need a new normal for the rest of the lighting steps ====

	// Expand (unpack) the normal from the normal map back to the [0,1]
	float3 normalFromMap = normalVector.rgb * 2 - 1;

	float3 N = input.normal;
	float3 T = normalize(input.tangent - N * dot(input.tangent, N)); // Orthogonalize!
	float3 B = cross(T, N); // The bi-tangent

	float3x3 TBN = float3x3(T, B, N);

	// Rotate the normal from the normal map by our TBN rotation matrix
	input.normal = normalize(mul(normalFromMap, TBN));

    for (int i = 0; i < lightCount; ++i)
    {
		if (lights[i].type == 0) {
			finalColor += directionalLight(i, surfaceColor, input.normal, toCamera);
		}
		else if (lights[i].type == 1) {
			finalColor += pointLight(i, surfaceColor, input.normal, toCamera, input.worldPos);
		}
		else if (lights[i].type == 2) {
			finalColor += spotLight(i, surfaceColor, input.normal, toCamera, input.worldPos);
		}
    }

	// Reflections
	// Quick and dirty fake "fresnel" 
	float nDotV = saturate(dot(input.normal, toCamera));
	return float4(lerp(reflectionColor, finalColor, nDotV), 1);
	


}