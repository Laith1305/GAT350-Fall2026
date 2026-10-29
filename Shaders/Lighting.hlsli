#pragma once

#define LIGHT_DIRECTIONAL   0
#define LIGHT_POINT         1
#define LIGHT_SPOT          2

static const uint MAX_LIGHTS = 8;

struct Material
{
    float4 color;
    float2 tiling;
    float2 offset;
    float shininess;
    float3 padding;
};
struct Light
{
    float3 position;
    uint type;
    float3 direction;
    float range;
    float3 color;
    float intensity;
    float3 ambient;
    float innerCutoff;
    float outerCutoff;
    float3 padding;
    
};

float calculateAttenuation(float distance, float range)
{
    
    return pow(1 - saturate(distance / range), 2);
}

float3 getLightVector(Light light, float3 position, out float attenuation)
{
    float3 L; 
    attenuation = 1.0f;
    
    switch (light.type)
    {
        case LIGHT_DIRECTIONAL:
            L = normalize(-light.direction);
            break;
        case LIGHT_POINT:
        {
            float3 toLight = light.position - position;
            float distance = length(toLight);
            attenuation = calculateAttenuation(distance, light.range);
            L = toLight / distance;
        }
            break;
        case LIGHT_SPOT:
            {
            float3 toLight = light.position - position;
            float distance = length(toLight);
            L = toLight / distance;
            attenuation = calculateAttenuation(distance, light.range);
            attenuation *= smoothstep(light.outerCutoff, light.innerCutoff, dot(-L, light.direction));
            }
            break;
        default:
            L = float3(0, 0, 0);
            break;
    }
    
    
    return L;
}

float3 calculateDiffuse(float3 radiance, float3 N, float3 L)
{
    // diffuse is the dot product of the normal and light vector
    return radiance * saturate(dot(N, L));
    
}

float3 calculateSpecular(float3 radiance, float3 N, float3 L, float3 V, Material material)
{
    float3 specular = float3(0, 0, 0);
    if (dot(N, L) > 0)
    {
        //Blinn-Phong Specular
        
        float3 H = normalize(V + L);
        float HdotN = saturate(dot(H, N));
        HdotN = pow(HdotN, material.shininess);
        specular = radiance * HdotN;
      
    }
    
    return specular;
}

void calculateLighting(Light light, Material material, float3 position, float3 N, float3 V,
                       out float3 diffuse, out float3 specular)
{
    float attenuation;
    float3 L = getLightVector(light, position, attenuation);

    // light arriving at the surface
    float3 radiance = light.color * light.intensity * attenuation;

    diffuse += calculateDiffuse(radiance, N, L);
    specular += calculateSpecular(radiance, N, L, V, material);
}