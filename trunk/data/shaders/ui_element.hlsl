////////////////////////////////////////
// Global variable
////////////////////////////////////////
matrix world_view_projection_matrix    : register(vs, c0);

////////////////////////////////////////
// Structures
////////////////////////////////////////
struct VS_INPUT
{
    float3 vs_vertexPosition    : POSITION0;
    float3 vs_vertexColor       : COLOR0; 
    float2 vs_textureCoord      : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 ps_position      : POSITION;
    float4 ps_vertexColor	: COLOR0;
	float2 ps_textureCoord  : TEXCOORD1;
};

////////////////////////////////////////
// Vertex Shader Function
////////////////////////////////////////
VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;
    
    output.ps_position = mul(float4(input.vs_vertexPosition, 1.0f), world_view_projection_matrix);
    output.ps_vertexColor = float4(input.vs_vertexColor, 1.0);
    output.ps_textureCoord = input.vs_textureCoord;
    return output;
}

sampler ui_texture_sampler : register(ps, s0);

////////////////////////////////////////
// PS Function
////////////////////////////////////////
float4 ps_main(VS_OUTPUT input):COLOR0
{
    return input.ps_vertexColor * tex2D(ui_texture_sampler, input.ps_textureCoord);
}


