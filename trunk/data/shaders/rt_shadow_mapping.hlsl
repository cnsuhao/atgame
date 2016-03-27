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
    float4 colour   : COLOR0;
    float3 Vertex   : TEXCOORD0;
    float4 LigViewPos : TEXCOORD1;
};

//
// VS Function
//
VS_OUTPUT vs_main(VS_INPUT input)
{
        VS_OUTPUT output = (VS_OUTPUT)0;

        output.position = mul(float4(input.position, 1.0f), mat_world_view_projection);

        output.colour = float4(input.colour, 1.0);
        
        output.Vertex = input.position;
        
        output.LigViewPos = mul(float4(input.position, 1.0f), mat_light_view_Projection);
        
        return output;
}

sampler rtDepthSampler              : register(ps, s0);

float3 LightPosition;
float3 LightDirection;
float spot_outer_cone;
float spot_inner_cone;
float bias;

float4 ambient;
float2 fViewportDimensions;

float texture2DCompare(sampler depths, float2 uv, float compare){
   float depth = tex2D(depths, uv).x;
   return step(compare, depth);
}

float texture2DShadowLerp(sampler depths, float2 size, float2 uv, float compare)
{
   float2 texelSize = 1.0/size;
   float2 f = frac(uv*size+0.5);
   float2 centroidUV = floor(uv*size+0.5)/size;

   float lb = texture2DCompare(depths, centroidUV+texelSize*float2(0.0, 0.0), compare);
   float lt = texture2DCompare(depths, centroidUV+texelSize*float2(0.0, 1.0), compare);
   float rb = texture2DCompare(depths, centroidUV+texelSize*float2(1.0, 0.0), compare);
   float rt = texture2DCompare(depths, centroidUV+texelSize*float2(1.0, 1.0), compare);
   float a = lerp(lb, lt, f.y);
   float b = lerp(rb, rt, f.y);
   float c = lerp(a, b, f.x);
   return c;
}

float PCF(sampler depths, float2 size, float2 uv, float compare){
   float result = 0.0;
   for(int x=-1; x<=1; x++){
       for(int y=-1; y<=1; y++){
           float2 off = float2(x,y)/size;
           result += texture2DShadowLerp(depths, size, uv+off, compare);
       }
   }
   return result/9.0;
}

#define _1_DIV_PI 0.017453

//
// PS Function
//
float4 ps_main(VS_OUTPUT input):COLOR0
{
    float3 v2l_dir = normalize(LightPosition - input.Vertex);

    float cos_outer_cone = cos(clamp(spot_outer_cone, 0.0, 90.0)*_1_DIV_PI);
    float cos_inner_cone = cos(clamp(spot_inner_cone, 0.0, 90.0)*_1_DIV_PI);
   
    float con = dot(normalize(LightDirection), -v2l_dir);
    float effect = smoothstep(cos_outer_cone, cos_inner_cone, con);
    float shadow = 1.0f;
    if(effect > 0.0)
    {
        float d1 = input.LigViewPos.z / input.LigViewPos.w;
      
        float2 st = float2(input.LigViewPos.xy / input.LigViewPos.w);
        st.x = st.x * 0.5 + 0.5;
        st.y = 1.0 - (st.y * 0.5 + 0.5);
      
        //shadow = PCF(rtDepthSampler, fViewportDimensions, st, d1 - bias * 0.01);
        shadow = texture2DCompare(rtDepthSampler, st, d1 - bias * 0.01);
    }
   
    return ambient * input.colour + shadow * effect * input.colour;
}