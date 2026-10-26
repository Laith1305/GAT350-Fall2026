Texture2D<float4> texture0 : register(t0, space3);
SamplerState u_sampler : register(s0, space3);


cbuffer uniforms : register(b0, space3)
{
    float4 color;
};


struct Input
{
	
    float2 texcoord : TEXCOORD0;
	
};

struct Output
{
	float4 color : SV_Target0;
};

Output main(Input input)
{
	Output output;
	output.color = texture0.Sample(u_sampler, input.texcoord) * color;

	return output;
}
