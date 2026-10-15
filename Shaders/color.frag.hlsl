cbuffer uniforms : register(b0, space3)
{
    float4 color;
};


struct Input
{
	
    float4 color : TEXCOORD0;
	
};

struct Output
{
	float4 color : SV_Target0;
};

Output main(Input input)
{
	Output output;
	output.color = input.color;
	//output.color = float4(1.0f, 0.0f, 0.0f, 1.0f);

	return output;
}
