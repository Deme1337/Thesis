Texture2D shaderTexture : register(t0);
Texture2D shaderSpecular : register(t1);
Texture2D shaderBumpMap : register(t2);
Texture2D shaderRoughness : register(t3);


TextureCube envmapTexture : register(t4);


SamplerState SampleTypeWrap : register(s0);

cbuffer ObjectData
{
    float4 objColor;
    float4 objColorSpec;
    int UseTextures;
    float roughnessOffset;
    float metallic;
};

struct PixelInputType
{
    float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
    float3 normal : NORMAL;
    float3 tangent : TANGENT;
    float3 binormal : BINORMAL;
    float4 worldPosition : TEXCOORD1;
    int HasAlpha : TEXCOORD2;
};

struct PixelOutputType
{
    float4 color : SV_Target0;
    float4 normal : SV_Target1;
    float4 specular : SV_Target2;
    float4 position : SV_Target3;
    float4 roughness : SV_Target4;
    float4 tangent : SV_TARGET5;
    float4 binormal : SV_TARGET6;
};

PixelOutputType GeometryPixelShader(PixelInputType input) : SV_TARGET
{
    PixelOutputType output;


    
		// Sample the color from the texture and store it for output to the render target.
    output.color = shaderTexture.Sample(SampleTypeWrap, input.tex);
    output.specular.x = shaderSpecular.Sample(SampleTypeWrap, input.tex).x;
    output.specular.y = metallic;
        
    //output.specular = objColorSpec;

    float4 bumpMap = shaderBumpMap.Sample(SampleTypeWrap, input.tex);

    bumpMap = (bumpMap * 2.0f) - 1.0f;

	// Calculate the normal from the data in the bump map.
    float3 bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.binormal) + (bumpMap.z * input.normal);
	
    output.normal = float4(bumpNormal, 1.0f);

    output.position = input.worldPosition / input.worldPosition.w;

    output.roughness = shaderRoughness.Sample(SampleTypeWrap, input.tex);

    output.tangent = float4(input.tangent, 1.0f) * 2.0f - 1.0f;
    output.binormal = float4(input.binormal, 1.0) * 2.0f - 1.0f;



  
	
    return output;
}