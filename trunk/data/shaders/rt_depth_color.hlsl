////////////////////////////////////////
// Global variable
////////////////////////////////////////

matrix mat_world_view_projection     : register(vs, c0);
matrix mat_light_view_Projection     : register(vs, c4);


struct VS_INPUT
{
    float3 position : POSITION0;
	float3 colour   : COLOR0;
};


struct VS_OUTPUT
{
    float4 position : POSITION;
	float2 depthInfo : TEXCOORD0;
};

//
// VS Function
//
VS_OUTPUT vs_main(VS_INPUT input)
{
		VS_OUTPUT output = (VS_OUTPUT)0;

        output.position = mul(float4(input.position, 1.0f), mat_light_view_Projection);

		output.depthInfo = output.position.zw;
		
        return output;
}

//
// PS Function
//
float4 ps_main(VS_OUTPUT input):COLOR0
{
    return float4(input.depthInfo.x / input.depthInfo.y, 0.0, 0.0, 0.0);
}