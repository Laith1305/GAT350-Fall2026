#include "uniforms.hlsli"
cbuffer FrameUniforms : register(FRAME_SLOT, VERTEX_UNIFORM_SPACE)
{
    Frame frame;
};
struct Input
{
    float3 Position : TEXCOORD0;
};
struct Output
{
    float3 TexCoord : TEXCOORD0;
    float4 Position : SV_Position;
};
Output main(Input input)
{
    Output output;
    output.TexCoord = input.Position;
    float3 dir = mul((float3x3) frame.view, input.Position);
    float4 pos = mul(frame.projection, float4(dir, 1.0));
    output.Position = pos.xyww;
    return output;
}