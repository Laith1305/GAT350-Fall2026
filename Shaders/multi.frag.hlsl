#include "lighting.hlsli"
#include "uniforms.hlsli"

Texture2D<float4> base1_texture : register(t0, FRAGMENT_TEXTURE_SPACE);
SamplerState base1_sampler : register(s0, FRAGMENT_TEXTURE_SPACE);

Texture2D<float4> base2_texture : register(t1, FRAGMENT_TEXTURE_SPACE);
SamplerState base2_sampler : register(s1, FRAGMENT_TEXTURE_SPACE);

Texture2D<float4> base3_texture : register(t2, FRAGMENT_TEXTURE_SPACE);
SamplerState base3_sampler : register(s2, FRAGMENT_TEXTURE_SPACE);

cbuffer ObjectUniforms : register(OBJECT_SLOT, FRAGMENT_UNIFORM_SPACE)
{
    Object object;
}
cbuffer FrameUniforms : register(FRAME_SLOT, FRAGMENT_UNIFORM_SPACE)
{
    Frame frame;
}

cbuffer LightUniforms : register(LIGHT_SLOT, FRAGMENT_UNIFORM_SPACE)
{
    Lights lights;
}
cbuffer MateralUniforms : register(MATERIAL_SLOT, FRAGMENT_UNIFORM_SPACE)
{
    Material material;
}

struct Input
{
	
    float2 texcoord : TEXCOORD0;
    float3 worldPosition : TEXCOORD1;
    float3 normal : TEXCOORD2;
    
};

struct Output
{
	float4 color : SV_Target0;
};

Output main(Input input)
{
	Output output;
    
    float t = base3_texture.Sample(base3_sampler, input.texcoord).r;
    float4 albedo1 = base1_texture.Sample(base1_sampler, input.texcoord) * material.baseColor;
    float4 albedo2 = base2_texture.Sample(base2_sampler, input.texcoord) * material.baseColor;
    float4 albedo = lerp(albedo1, albedo2, t);
    
	
    float3 diffuse = lights.ambient;
    float3 specular = 0;
    
    float3 N = normalize(input.normal);
    float3 V = normalize(frame.cameraPosition - input.worldPosition); 
    for (uint i = 0; i < lights.count; i++)
    {
        
        calculateLighting(lights.lights[i], material, input.worldPosition, normalize(input.normal), V, diffuse, specular);
    }
	
    
    output.color = float4(albedo.rgb * diffuse + specular * material.specularColor, albedo.a);
	

	return output;
}
