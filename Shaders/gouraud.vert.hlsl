#include "Lighting.hlsli"

cbuffer VertexUniforms : register(b0, space1)
{
    float4x4 model;
    float4x4 mvp;
    float3 cameraPosition;
    float padding1;
}

cbuffer LightUniforms : register(b1, space1)
{
    Light light;
}


cbuffer MateralUniforms : register(b2, space1)
{
    Material material;
}

struct Input
{
    float3 position : TEXCOORD0;
    float2 texcoord : TEXCOORD1;
    float3 normal : TEXCOORD2;
};

struct Output
{
    float4 position : SV_Position;
    float2 texcoord : TEXCOORD0;
    float3 diffuse : TEXCOORD1;
    float3 specular : TEXCOORD2;
    
};
Output main(Input input)
{
    Output output;
    output.texcoord = (input.texcoord * material.tiling) + material.offset;
    output.position = mul(mvp, float4(input.position, 1.0f));
    
    float3 worldPosition = mul(model, float4(input.position, 1.0f)).xyz;
    float3 N = normalize(mul((float3x3) model, input.normal));
    float3 V = normalize(cameraPosition - worldPosition);
    
    calculateLighting(light, material, worldPosition, N, V, output.diffuse, output.specular);
    
    
    return output;
}

