////////////////////////////////////////
// Global variable
////////////////////////////////////////
matrix mat_world          			: register(vs, c0);
matrix mat_world_view_projection    : register(vs, c4);
matrix mat_world_inverse_transpose  : register(vs, c8);

static const int c_numberOfLights = 8;

float3 u_globalAmbient              :register(ps, c0);  // 全局环境光.

float3 u_materialAmbient            :register(ps, c1);
float3 u_materialDiffuse            :register(ps, c2);
float3 u_materialSpecular           :register(ps, c3);
float4 u_materialData0              :register(ps, c4);

float u_numberOfLights              :register(ps, c5);

float3 u_lightPosition[c_numberOfLights]              :register(ps, c10);  // transformed in view space.
float3 u_lightDirection[c_numberOfLights]             :register(ps, c18);  // transformed in view space.
float4 u_lightData0[c_numberOfLights]                 :register(ps, c26);  // light type | 预留 | 预留 | 预留 
float4 u_lightData1[c_numberOfLights]                 :register(ps, c34);  // intensity | range | outer cone | inner cone
float3 u_lightDiffuse[c_numberOfLights]               :register(ps, c42);  // light diffuse
float3 u_lightSpecular[c_numberOfLights]              :register(ps, c50);  // light specular

struct VS_INPUT
{
    float4 vs_vertexPosition    : POSITION0;
    float3 vs_vertexNormal      : NORMAL0;
    float2 vs_textureCoord      : TEXCOORD0;
};

struct VS_OUTPUT
{
    float4 ps_vertexFinalPosition   : POSITION;
    float3 ps_vertexPosition        : TEXCOORD0;
    float3 ps_vertexNormal          : TEXCOORD1;
    float2 ps_textureCoord          : TEXCOORD2;
};

//
// VS Function
//
VS_OUTPUT vs_main(VS_INPUT input)
{
    VS_OUTPUT output = (VS_OUTPUT)0;

    output.ps_vertexFinalPosition = mul(input.vs_vertexPosition, mat_world_view_projection);
    output.ps_vertexPosition = mul(input.vs_vertexPosition, mat_world).xyz;
    output.ps_vertexNormal = mul(float4(input.vs_vertexNormal, 1.0), mat_world_inverse_transpose).xyz;
    output.ps_textureCoord = input.vs_textureCoord;

    return output;
}

sampler sampler_diffuse : register(ps, s5);

////////////////////////////////////////
float half_lambert( in float3 normalDir, 
                    in float3 lightDir, in float factor)
{
    // Half-Lambert漫反射
    float NdotL = dot(normalDir, lightDir);
    return max(0, (NdotL * (1.0 - factor) + factor));
}


////////////////////////////////////////
// PS Function
////////////////////////////////////////
float4 ps_main(VS_OUTPUT input):COLOR0
{
    float3 ambient = u_globalAmbient * u_materialAmbient * tex2D(sampler_diffuse, input.ps_textureCoord).xyz;
    float3 diffuse_total = float3(0.0, 0.0, 0.0);
    float3 specular_total = float3(0.0, 0.0, 0.0);
    
    for(int i = 0; i < c_numberOfLights; ++i)
    {
        if(i < u_numberOfLights)
        {
            float light_intensity = u_lightData1[i].x;
            float attenuation = light_intensity;
            float effect = 1.0;

            float3 lightDirection = normalize(u_lightDirection[i]);
            float3 vertexToLightDirection = -lightDirection;
            
            if(u_lightData0[i].x > 0.01) // point light or spot light.
            {
                float3 vertexToLight = u_lightPosition[i] - input.ps_vertexPosition;
                vertexToLightDirection = normalize(vertexToLight);
                float light_rang = u_lightData1[i].y;
                attenuation = light_intensity * clamp((light_rang - length(vertexToLight)) / light_rang, 0.0, 1.0);
                if(u_lightData0[i].x > 1.01) // is spot light
                {
                    //float cos_outer_cone = u_lightData1[i].z; // cos(radians_for_outer_cone_);
                    //float cos_inner_cone = u_lightData1[i].w; // cos(radians_for_int_cone);
                    float con = dot(lightDirection, -vertexToLightDirection);
                    //effect = smoothstep(cos_outer_cone, cos_inner_cone, con);
                    effect = smoothstep(u_lightData1[i].z, u_lightData1[i].w, con);
                }
            }
            
            float ndl = half_lambert(normalize(input.ps_vertexNormal), vertexToLightDirection, u_lightData0[i].y);
            //float ndl = max(0.0, dot(normalize(input.ps_vertexNormal), vertexToLightDirection));
            float3 diffuse = effect * u_materialDiffuse * u_lightDiffuse[i] * ndl;
            diffuse = diffuse * tex2D(sampler_diffuse, input.ps_textureCoord).xyz;
            diffuse_total += attenuation * diffuse;
            
            //float3 h = vertexToLightDirection + normalize(-input.ps_vertexPosition);
            //h = normalize(h);
            //float facing = max(0.0, dot(input.ps_vertexNormal, vertexToLightDirection));
            //if(facing > 0.0)
            //    facing = 1.0;
            
            // u_materialData0.x is material shininess.
            //float3 specular = effect * u_materialSpecular * u_lightSpecular[i] * 
            //                facing * pow(max(0.0, dot(input.ps_vertexNormal, h)), u_materialData0.x); 

            //specular_total += attenuation * specular;
        }
    }
    
    //return float4(ambient + diffuse_total + specular_total, 1.0);
    return float4(ambient + diffuse_total, 1.0);
}

