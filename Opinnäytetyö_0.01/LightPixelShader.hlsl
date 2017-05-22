Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D positionTexture : register(t3);
Texture2D roughnessTexture : register(t4);
Texture2D tangentTexture : register(t5);
Texture2D binormalTexture : register(t6);

TextureCube envmapTexture : register(t7);


SamplerState SampleTypePoint : register(s0);
SamplerComparisonState SampleTypeShadow : register(s1);

//test anistotropic
SamplerState SamplerAnisotropic
{
    Filter = ANISOTROPIC;
    MaxAnisotropy = 16;
    AddressU = Wrap;
    AddressV = Wrap;
};

cbuffer LightBuffer : register(b0)
{
    float4 lightDirection; // position
    float4 lightColor;
    //float globalAmbient;
};


#define POINT_LIGHT_COUNT 4

cbuffer PointLightBuffer : register(b1)
{
    float4 PointLightPosition[POINT_LIGHT_COUNT];
    float4 PointLightColor[POINT_LIGHT_COUNT];
};

cbuffer SettingsBuffer : register(b2)
{
    int BLinnPhongs;
    int Phongs;
    int CookTorrances;
    int Wards;
    int ShowGBuffer;
    int ONDiffuse;
    int Lambertian;
    float4 vvv;
}



struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 viewDir : TEXCOORD1;
};

struct LightPixelShaderOutput
{
    float4 color : SV_Target0;
};





#include "Commons.hlsl"


float3 CalculateEnvironmentMap(float3 N, float3 V, float roughness)
{
    float a = roughness;
    float3 reflectVector = reflect(-V, N);
    float mipIndex = a * a * 8.0f;

    float4 envMap = envmapTexture.SampleLevel(SamplerAnisotropic, reflectVector, mipIndex);
    envMap = pow(envMap, 2.2);
    return envMap.xyz;
}


float Geometric_GGX(float a, float dotValue)
{
    float a2 = a * a;
    return (2.0f * dotValue) / (dotValue + sqrt(a2 + ((1.0f - a2) * (dotValue * dotValue))));
}

float Geometric_Smith_GGX(float a, float NdV, float NdL)
{
    return Geometric_GGX(a, NdV) * Geometric_GGX(a, NdL);
}






static const float globalAmbient = 0.02f;
static const float AttenPow = 450.0f;


float3 WardSpecular(float3 N, float3 V, float3 L, float a, float3 albedo, float metallic, float3 radiance, float3 tangent, float3 binormal)
{
    float3 ward = 0.0f.xxx;
    float _AlphaX = 1.0f;
    float _AlphaY = 0.2f;
    float3 F0 = 0.03f.xxx;
    
    float3 metallic1 = float3(metallic, metallic, metallic);
   
    F0 = lerp(F0, albedo, metallic1);

    float3 H = normalize(V + L);

    float NoL = dot(L, N);
    float HoN = dot(H, N);
    float HoV = dot(H, V);
    float VoN = dot(V, N);
    float AlphaX = dot(H, tangent) / _AlphaX;
    float AlphaY = dot(H, binormal) / _AlphaY;

    float3 F = FresnelSchlick1(max(HoV, 0.0), F0);

    float3 kS = F;
    float kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0 - metallic1;


    float3 spec =  F0 * sqrt(saturate(dot(NoL, VoN))) * exp(-2.0 * (AlphaX * AlphaX + AlphaY * AlphaY) / (1.0f + HoN));

    float3 diff = DiffuseLambertian(albedo) * radiance * NoL;

    ward = diff +  spec;

    return ward;
}

//Cook torrance lighting N = normal, V = View, L = light, a = roughness, albedo = color, metallic = specular texture
float3 CookTorrance(float3 N, float3 V, float3 L, float a, float3 albedo, float metallic, float3 radiance)
{
    float3 H = normalize(V + L);
    float3 F0 = 0.03f.xxx;
    float3 Cook = 0.0f;

    float3 metallic1 = float3(metallic, metallic, metallic);

    F0 = lerp(F0, albedo, metallic1);
    

    float NoH = dot(N, H);
    float NoV = dot(N, V);
    float NoL = dot(N, L);
    float VoH = dot(V, H);
    float HoV = dot(H, V);


    float NDF = DistributionGGX(N, H, a);
    float G   = GeometrySmith(N, V, L, a);
    float3 F  = FresnelSchlick1(max(HoV,0.0), F0);

    float3 kS = F;
    float kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0 - metallic1;

    float3 nominator = NDF * G * F;
    float denom = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    float3 specular = nominator / denom;

    NoL = saturate(NoL);
    float3 diffuseC = Diffuse_OrenNayar(albedo, a, NoV, NoL, VoH); //DiffuseLambertian(albedo);
    float3 specularC = specular;

    Cook = (kD * diffuseC + specularC) * radiance * NoL;

    Cook = Cook / (Cook + 1.0f);
    Cook = pow(Cook, 1.0f / 2.2f);

    return Cook;

}


float3 Phong(float3 N, float3 V, float3 L, float a, float3 albedo, float metallic, float3 radiance)
{
    float NoL = dot(N, L);
    float3 F0 = 0.03f.xxx;
    float3 metallic1 = float3(metallic, metallic, metallic);

    F0 = lerp(F0, albedo, metallic1);


    float3 diffuse = DiffuseLambertian(albedo);

    float3 reflection = reflect(-L, N);

    float3 specular = pow(max(dot(N, reflection), 0.0), 5.0);

    

    return diffuse + specular * radiance * NoL;

}

float3 BlinnPhong(float3 N, float3 V, float3 L, float a, float3 albedo, float metallic, float3 radiance)
{
    float NoL = dot(N, L);
    float3 H = normalize(V + L);
    float3 F0 = 0.03f.xxx;
    float3 metallic1 = float3(metallic, metallic, metallic);

    F0 = lerp(F0, albedo, metallic1);

    float3 diffuse = DiffuseLambertian(albedo);

   
    
    float3 specular = pow(max(dot(N, H), 0.0), 5.0);

    

    return diffuse + (specular * F0) * radiance * NoL;

}





float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float3 normals = normalTexture.Sample(SampleTypePoint, input.tex).xyz;


    float3 position = positionTexture.Sample(SampleTypePoint, input.tex).xyz;
    float3 albedo   = pow(colorTexture.Sample(SampleTypePoint, input.tex).xyz,2.2);
    float3 tangent  = tangentTexture.Sample(SampleTypePoint, input.tex).xyz;
    float3 binormal = binormalTexture.Sample(SampleTypePoint, input.tex).xyz;


    float metallic  = specularTexture.Sample(SampleTypePoint, input.tex).r;
    float roughenss = roughnessTexture.Sample(SampleTypePoint, input.tex).r;
   
    float3 viewDirection = normalize(input.viewDir - position.xyz);
    float3 color = 0.0f;
    

    //Gbuffer debugging
    if (ShowGBuffer == 1)
    {
        return float4(albedo,1.0);
    }
    if(ShowGBuffer == 2)
    {
        return float4(normals, 1.0);
    }
    if (ShowGBuffer == 3)
    {
        return float4(metallic,metallic,metallic, 1.0);
    }
    if (ShowGBuffer == 4)
    {
        return float4(position, 1.0);
    }
    if (ShowGBuffer == 5)
    {
        return float4(roughenss,roughenss,roughenss, 1.0);
    }
    if (ShowGBuffer == 6)
    {
        return float4(tangent, 1.0);
    }
    if (ShowGBuffer == 7)
    {
        return float4(binormal, 1.0);
    }



    float3 lightDir = normalize(PointLightPosition[1].xyz - position);


   for (int i = 1; i < 2; i++)
   {
       //float3 lightDir = normalize(PointLightPosition[i].xyz - position);
       float distance = length(PointLightPosition[i].xyz - position);
       float atten = AttenPow / (distance * distance);
       float3 radiance = atten * PointLightColor[i].xyz;
       
        if (BLinnPhongs == 1)
        {
            color += BlinnPhong(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);
        }
        if (Phongs == 1)
        {
            color += Phong(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);
        }
           
         
        if (CookTorrances == 1)
        {
            color += CookTorrance(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);
        }
   
        if (Wards == 1)
        {
            color += WardSpecular(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance, tangent, binormal);
        }
    }

   //float3 lightDir = -normalize(lightDirection.xyz);
   //float3 radiance = 1.5f * lightColor.xyz;

    float3 H = normalize(viewDirection + lightDir);

    //if (BLinnPhongs == 1)
    //{
    //    color += BlinnPhong(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);
    //}
    //if(Phongs == 1)
    //{
    //    color += Phong(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);
    //}
    //      
    //if (CookTorrances == 1)
    //{
    //    color += CookTorrance(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);
    //}


    if(ONDiffuse == 1)
    {
        color += Diffuse_OrenNayar(albedo, roughenss, dot(normals, viewDirection), dot(normals, lightDir), dot(viewDirection, H));
    }

    if(Lambertian == 1)
    {
        color +=  DiffuseLambertian(albedo) + globalAmbient * albedo;
    }
     
    return float4(color  + globalAmbient * albedo, 1.0);
}



