////////////////////////////////////////
// Global variable
////////////////////////////////////////

matrix mat_world_view_projection     : register(vs, c0);

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
sampler waterHeightSampler          : register(ps, s1);

float4  u_d                          :register(ps, c0);  //坐标.

//
// PS Function
//
float4 ps_main(VS_OUTPUT input):COLOR0
{
    float2 x1 = float2(saturate(input.texcoord.x + u_d.x), input.texcoord.y);
    float2 x2 = float2(saturate(input.texcoord.x - u_d.x), input.texcoord.y);
    float2 y1 = float2(input.texcoord.x, saturate(input.texcoord.y + u_d.y));
    float2 y2 = float2(input.texcoord.x, saturate(input.texcoord.y - u_d.y));
    float xoff = tex2D(waterHeightSampler, x1).x - tex2D(waterHeightSampler, x2).x;
    float yoff = tex2D(waterHeightSampler, y1).x - tex2D(waterHeightSampler, y2).x;

    float2 uv = float2(saturate(input.texcoord.x + xoff / 40.0), saturate(input.texcoord.y + yoff / 40.0));
    float4 color = tex2D(textureSampler, uv);
    color.rgb += (xoff + yoff) * 0.5;
    return color;
}