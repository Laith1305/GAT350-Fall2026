#include "lighting.hlsli"
#include "uniforms.hlsli"

Texture2D<float4> u_texture : register(t0, space2);
SamplerState u_sampler : register(s0, space2);

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
	
    float4 albedo = u_texture.Sample(u_sampler, input.texcoord) * material.color;
    float3 diffuse = lights.ambient;
    float3 specular = 0;
    
    float3 V = normalize(frame.cameraPosition - input.worldPosition); 
    for (uint i = 0; i < lights.count; i++)
    {
        
        calculateLighting(lights.lights[i], material, input.worldPosition, normalize(input.normal), V, diffuse, specular);
    }
	
    
    output.color = float4(albedo.rgb * diffuse + specular, albedo.a);
	

	return output;
}
