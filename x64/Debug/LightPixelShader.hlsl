Texture2D colorTexture : register(t0);
Texture2D normalTexture : register(t1);
Texture2D specularTexture : register(t2);
Texture2D positionTexture : register(t3);
Texture2D roughnessTexture : register(t4);

TextureCube envmapTexture : register(t5);


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

// ===============================================================================================
// http://holger.dammertz.org/stuff/notes_HammersleyOnHemisphere.html
// ===============================================================================================
float2 Hammersley(uint i, uint N)
{
    float ri = reversebits(i) * 2.3283064365386963e-10f;
    return float2(float(i) / float(N), ri);
}

//http://blog.selfshadow.com/publications/s2013-shading-course/karis/s2013_pbs_epic_notes_v2.pdf
float3 ImportanceSampleGGX(float2 Xi, float roughness, float3 N)
{
    float a = roughness * roughness;

    float Phi = 2 * PI * Xi.x;
    float CosTheta = sqrt((1 - Xi.y) / (1 + (a * a - 1) * Xi.y));
    float SinTheta = sqrt(1 - CosTheta * CosTheta);

    float3 H;
    H.x = SinTheta * cos(Phi);
    H.y = SinTheta * sin(Phi);
    H.z = CosTheta;
    float3 UpVector = abs(N.z) < 0.999 ? float3(0, 0, 1) : float3(1, 0, 0);
    float3 TangentX = normalize(cross(UpVector, N));
    float3 TangentY = cross(N, TangentX);

	// Tangent to world space
    return TangentX * H.x + TangentY * H.y + N * H.z;
}

float2 IntegrateBRDF(float Roughness, float NoV, float3 N)
{
    float3 V;
    V.x = sqrt(1.0f - NoV * NoV); // sin
    V.y = 0;
    V.z = NoV; // cos
    float A = 0;
    float B = 0;
    const uint NumSamples = 24;
    for (uint i = 0; i < NumSamples; i++)
    {
        float2 Xi = Hammersley(i, NumSamples);
        float3 H = ImportanceSampleGGX(Xi, Roughness, N);
        float3 L = 2 * dot(V, H) * H - V;
        float NoL = saturate(L.z);
        float NoH = saturate(H.z);
        float VoH = saturate(dot(V, H));
        if (NoL > 0)
        {
			//float G = G_Smith(Roughness, NoV, NoL);
            float G = Geometric_Smith_GGX(Roughness, NoV, NoL);
            float G_Vis = G * VoH / (NoH * NoV);
            float Fc = pow(1 - VoH, 5);
            A += (1 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }
    return float2(A, B) / NumSamples;
}


float3 PrefilterEnvMap(float Roughness, float3 R)
{
    float3 N = R;
    float3 V = R;
    float3 PrefilteredColor = 0;
    const uint NumSamples = 100;
    float TotalWeight = 0;
    for (uint i = 0; i < NumSamples; i++)
    {
        float2 Xi = Hammersley(i, NumSamples);
        float3 H = ImportanceSampleGGX(Xi, Roughness, N);
        float3 L = 2 * dot(V, H) * H - V;
        float NoL = saturate(dot(N, L));
        if (NoL > 0)
        {
            float4 envMap = envmapTexture.SampleLevel(SamplerAnisotropic, L, NumSamples);
            PrefilteredColor += envMap.xyz * NoL;
            TotalWeight += NoL;
        }
    }
    return PrefilteredColor / TotalWeight;
}

float3 ApproximateSpecularIBL(float3 SpecularColor, float Roughness, float3 N, float3 V)
{
    float NoV = saturate(dot(N, V));
    float3 R = 2 * dot(V, N) * N - V;
    float3 PrefilteredColor = PrefilterEnvMap(Roughness, R);
    float2 EnvBRDF = IntegrateBRDF(Roughness, NoV, R);
    return PrefilteredColor * (SpecularColor * EnvBRDF.x + EnvBRDF.y);
}









static const float globalAmbient = 0.2f;
static const float AttenPow = 550.0f;

//Cook torrance lighting N = normal, V = View, L = light, a = roughness, albedo = color, metallic = specular texture
float3 CookTorrance(float3 N, float3 V, float3 L, float a, float3 albedo, float metallic, float radiance)
{
    float H = normalize(V + L);
    float3 F0 = 0.04f;
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
    float3 F  = FresnelSchlick(saturate(HoV), F0);

    float3 kS = F;
    float kD = float3(1.0f, 1.0f, 1.0f) - kS;
    kD *= 1.0 - metallic1;

    float3 nominator = NDF * G * F;
    float denom = 4 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.001;
    float3 specular = nominator / denom;

    NoL = saturate(NoL);
    float3 diffuseC = DiffuseLambertian(albedo);
    float3 specularC = specular + CalculateEnvironmentMap(N, V, a);
    
    Cook = ( kD * diffuseC + specularC * radiance * NoL + globalAmbient * albedo);

    Cook = Cook / (Cook + 1.0f);
    Cook = pow(Cook, 1.0f / 2.2f);

    return Cook;

}


float3 Phong(float3 N, float3 V, float3 L, float a, float3 albedo, float metallic, float radiance)
{
    float NoL = dot(N, L);

    float3 F0 = 0.03f;
    F0 = lerp(F0, albedo, metallic);

    float3 diffuse = albedo * saturate(NoL) * radiance;

    float3 reflection = reflect(-L, N);

    float3 specular = pow(max(dot(N, reflection), 0.0), 25.0);

    

    return diffuse + specular;

}

float3 BlinnPhong(float3 N, float3 V, float3 L, float a, float3 albedo, float metallic, float radiance)
{
    float NoL = dot(N, L);
    float H = normalize(V + L);
    float3 F0 = 0.03f;
    F0 = lerp(F0, albedo, metallic);

    float3 diffuse = albedo * saturate(NoL) * radiance;



    
    float3 specular = pow(max(dot(N, H), 0.0), 55.0);

    

    return diffuse + specular;

}


float4 LightPixelShader(PixelInputType input) : SV_TARGET
{
    float3 normals = normalTexture.Sample(SampleTypePoint, input.tex).xyz;
    normals = normalize(normals);

    float3 position = positionTexture.Sample(SampleTypePoint, input.tex).xyz;
    float3 albedo   = pow(colorTexture.Sample(SampleTypePoint, input.tex).xyz,2.2);
    float metallic  = specularTexture.Sample(SampleTypePoint, input.tex).r;
    float roughenss = roughnessTexture.Sample(SampleTypePoint, input.tex).r;
   
    float3 viewDirection = normalize(input.viewDir - position.xyz);
    float3 color = 0.0f;
    
   //for (int i = 0; i < POINT_LIGHT_COUNT; i++)
   //{
   //    float3 lightDir = normalize(PointLightPosition[i].xyz - position);
   //    float distance = length(PointLightPosition[i].xyz - position);
   //    float atten = AttenPow / (distance * distance);
   //    float radiance = atten * PointLightColor[i].xyz;
   //    color += CookTorrance(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);
   //
   //    //color += BlinnPhong(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);
   //}

   float3 lightDir = -normalize(lightDirection.xyz);
   float radiance = 1.5f * lightColor.xyz;
   color += CookTorrance(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);
   
    //color += BlinnPhong(normals, viewDirection, lightDir, roughenss, albedo, metallic, radiance);


    return float4(1.0f,1.0f,1.0f, 1.0);
}



