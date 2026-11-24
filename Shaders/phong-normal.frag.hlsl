#include "lighting.hlsli"
#include "uniforms.hlsli"

Texture2D<float4> base_texture : register(t0, FRAGMENT_TEXTURE_SPACE);
SamplerState base_sampler : register(s0, FRAGMENT_TEXTURE_SPACE);
Texture2D<float4> specular_texture : register(t1, FRAGMENT_TEXTURE_SPACE);
SamplerState specular_sampler : register(s1, FRAGMENT_TEXTURE_SPACE);
Texture2D<float4> emissive_texture : register(t2, FRAGMENT_TEXTURE_SPACE);
SamplerState emissive_sampler : register(s2, FRAGMENT_TEXTURE_SPACE);
Texture2D<float4> normal_texture : register(t3, FRAGMENT_TEXTURE_SPACE);
SamplerState normal_sampler : register(s3, FRAGMENT_TEXTURE_SPACE);

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
    float3 tangent : TEXCOORD3;
    
};

struct Output
{
	float4 color : SV_Target0;
};

Output main(Input input)
{
	Output output;
	
    float specularMask = specular_texture.Sample(specular_sampler, input.texcoord).r;
    float3 emissive = emissive_texture.Sample(emissive_sampler, input.texcoord).rgb * material.emissiveColor;
    
    float4 albedo = base_texture.Sample(base_sampler, input.texcoord) * material.baseColor;
    float3 diffuse = lights.ambient;
    float3 specular = 0;
    
    float3 normal = normal_texture.Sample(normal_sampler, input.texcoord).rgb * 2.0 - 1.0;
    normal.xy *= material.normalStrength;
    
    float3 nNormal = normalize(input.normal);
    float3 T = normalize(input.tangent);
    float3 B = cross(nNormal, T);
    float3x3 TBN = float3x3(T, B, nNormal);
    float3 N = normalize(mul(normal, TBN));
    
    float3 V = normalize(frame.cameraPosition - input.worldPosition);
    for (uint i = 0; i < lights.count; i++)
    {
        
        calculateLighting(lights.lights[i], material, input.worldPosition, normalize(input.normal), V, diffuse, specular);
    }
	
    
    output.color = float4(albedo.rgb * diffuse + specular * material.specularColor * specularMask + emissive, albedo.a);
	

	return output;
}
