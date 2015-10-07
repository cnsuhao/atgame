////////////////////////////////////////
// Global variable
////////////////////////////////////////
matrix mat_world_view_projection    : register(vs, c0);

////////////////////////////////////////
// Structures
////////////////////////////////////////
struct VS_INPUT
{
	float3 position : POSITION0;
	float2 texcoord : TEXCOORD0;
};


struct VS_OUTPUT
{
    float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
};

//
// VS Function
//
VS_OUTPUT vs_main(VS_INPUT input)
{
		VS_OUTPUT output = (VS_OUTPUT)0;

        output.position = mul(float4(input.position, 1.0f), mat_world_view_projection);
		
		output.texcoord = input.texcoord;

        return output;
}

sampler textureSampler              : register(ps, s0);
//
// PS Function
//
float4 ps_main(VS_OUTPUT input):COLOR0
{
	return tex2D(textureSampler, input.texcoord);
}
