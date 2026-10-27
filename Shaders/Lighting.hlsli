#pragma once

#define LIGHT_DIRECTIONAL   0
#define LIGHT_POINT         1
#define LIGHT_SPOT          2


struct Material
{
    float2 tiling;
    float2 offset;
    
};
struct Light
{
    float3 position;
    uint type;
    float3 direction;
    float padding2;
    float3 color;
    float intensity;
    float3 ambient;
    float padding3;
};