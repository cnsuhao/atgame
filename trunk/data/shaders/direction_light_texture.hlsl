////////////////////////////////////////
// uniform variable
////////////////////////////////////////
matrix world_view_projection_matrix     : register(vs, c0);
matrix world_inverse_transpose_matrix   : register(vs, c4);


float3 material_diffuse_color           : register(ps, c0);
float3 material_ambient_color           : register(ps, c1);
float3 material_spacular_color          : register(ps, c2);
float3 material_emissive_color          : register(ps, c3);

float3 global_ambient_color             : register(ps, c6);

float3 light_position                   : register(ps, c10);
float3 light_color                      : register(ps, c11);

// float3 light_position[max_number_light];
// float3 light_color[max_number_light];
// float  light_number_using;


struct VS_INPUT
{
    float4 vs_vertexPosition    : POSITION0;
    float3 vs_vertexNormal	    : NORMAL0;
    float2 vs_textureCoord      : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 ps_position          : POSITION;
    float3 ps_vertex_normal     : TEXCOORD0;
    float2 ps_texture_coord     : TEXCOORD1;
};


//
// VS Function
//
VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.ps_position = mul(input.vs_vertexPosition, world_view_projection_matrix);

    output.ps_vertex_normal = mul(float4(input.vs_vertexNormal, 1.0), world_inverse_transpose_matrix).xyz;

    output.ps_texture_coord = input.vs_textureCoord;

    return output;
}

sampler matrial_diffsue_sampler : register(ps, s0);

////////////////////////////////////////
float half_lambert( in float3 normalDir, 
                    in float3 lightDir)
{
    // Half-Lambert漫反射
    float NdotL = dot(normalDir, lightDir);
    return max(0, (NdotL * 0.5 + 0.5));
}


////////////////////////////////////////
// PS Function
////////////////////////////////////////
float4 ps_main(VS_OUTPUT input):COLOR0
{

    // 光照方程 surface_colour = emissive + ambient + diffuse + specular

    float4 ambient_color = float4(global_ambient_color * material_ambient_color, 1.0);

    float3 vertex_normal = normalize(input.ps_vertex_normal);

    float3 light_dirction = normalize(light_position);

    float NdotL = half_lambert(vertex_normal, light_dirction);

    float4 diffuse_color =  tex2D(matrial_diffsue_sampler, input.ps_texture_coord) * float4(material_diffuse_color * light_color, 1.0) * NdotL;

    float4 emissive_color = float4(material_emissive_color, 1.0);

    float4 surface_colour =  ambient_color + diffuse_color + emissive_color;

	return surface_colour;
}

