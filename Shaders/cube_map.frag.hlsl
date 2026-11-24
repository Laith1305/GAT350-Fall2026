#include "Uniforms.hlsli"
TextureCube<float4> cubemap_texture : register(t0, FRAGMENT_TEXTURE_SPACE);
SamplerState cubemap_sampler : register(s0, FRAGMENT_TEXTURE_SPACE);
struct Input
{
    float3 texcoord : TEXCOORD0;
};
 
struct Output
{
    float4 color : SV_Target0;
};
 
Output main(Input input)
{
    Output output;
    output.color = cubemap_texture.Sample(cubemap_sampler, input.texcoord);
    return output;
}