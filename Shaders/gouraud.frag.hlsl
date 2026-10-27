Texture2D<float4> u_texture : register(t0, space3);
SamplerState u_sampler : register(s0, space3);

cbuffer uniforms : register(b0, space3)
{
    float4 tint;
};


struct Input
{
	
    float2 texcoord : TEXCOORD0;
    float3 diffuse : TEXCOORD1;
    float3 specular : TEXCOORD2;
};

struct Output
{
	float4 color : SV_Target0;
};

Output main(Input input)
{
	Output output;
	
    float4 albedo = u_texture.Sample(u_sampler, input.texcoord) * tint;
	output.color =  float4(albedo.rgb * input.diffuse + input.specular, 1.0);
	

	return output;
}
