////////////////////////////////////////
// Global variable
////////////////////////////////////////
matrix WVPMatrix : register(vs, c0);

float4 ambient   : register(ps, c0);
float4 intensity : register(ps, c1);
sampler texture0 : register(ps, s0);

////////////////////////////////////////
// Structures
////////////////////////////////////////
struct VS_INPUT
{
	float4 position : POSITION0;
	//float4 normal	: NORMAL0;
	float2 texcoord : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 position : POSITION;
	float2 texcoord : TEXCOORD0;
};


////////////////////////////////////////
// VS Function
////////////////////////////////////////
VS_OUTPUT vs_main(VS_INPUT input)
{
       VS_OUTPUT output = (VS_OUTPUT)0;

       output.position = mul(input.position, WVPMatrix);
	   output.texcoord = input.texcoord;

       return output;
}

////////////////////////////////////////
// PS Function
////////////////////////////////////////
float4 ps_main(VS_OUTPUT input):COLOR0
{
	//float4 color = input.color;
	float4 color = tex2D(texture0, input.texcoord);// + (ambient * intensity.x);
	return color;
}


