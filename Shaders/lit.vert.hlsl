cbuffer Uniforms : register(b0, space1)
{
    float4x4 model;
    float4x4 mvp;
}

cbuffer LightUniforms : register(b1, space1)
{
    float3 lightDirection;
    float padding1;
    float3 lightColor;
    float lightIntensity;
    float3 ambient;
    float padding2;
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
    
};

Output main(Input input)
{
    Output output;
    output.texcoord = input.texcoord;
    output.position = mul(model, float4(input.position, 1.0f));

    
    //Diffuse
    float3 N = normalize(mul((float3x3) model, input.normal));
    float3 L = -lightDirection;
    float NdotL = saturate(dot(N, L));
    
    float3 diffuse = lightColor * lightIntensity * NdotL;
    
    output.diffuse = ambient + diffuse;
    
    return output;
}