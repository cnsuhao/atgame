////////////////////////////////////////
// Global variable
////////////////////////////////////////

matrix mat_world_view_projection     : register(vs, c0);
float4 vec_solid_color               : register(ps, c0);


struct VS_OUTPUT
{
    float4 position : POSITION;
};

//
// VS Function
//
VS_OUTPUT vs_main(float3 position : POSITION0)
{
		VS_OUTPUT output = (VS_OUTPUT)0;

        output.position = mul(float4(position, 1.0f), mat_world_view_projection);

        return output;
}

//
// PS Function
//
float4 ps_main(VS_OUTPUT input):COLOR0
{
    return vec_solid_color;
}