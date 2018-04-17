SamplerState sampleType;

Texture2D colorTex : register(t0);
Texture2D normalTex : register(t1);
Texture2D refractionTex : register(t2);

cbuffer GlassBuffer
{
	float refractionScale;
	float3 padding;
};

struct PixelInputType 
{
	float4 position : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float4 refractionPosition : TEXCOORD1;
};

float4 GlassPixelShader(PixelInputType input) : SV_TARGET
{
	float2 refractTexcoord;
	float4 normalMap;
	float3 normal;
	float4 refractionColor;
	float4 textureColor;
	float4 color;
 
	refractTexcoord.x = input.refractionPosition.x / input.refractionPosition.w / 2.0f + 0.5f;
	refractTexcoord.y = input.refractionPosition.y / input.refractionPosition.w / 2.0f + 0.5f;

	normalMap = normalTex.Sample(sampleType, input.texcoord);
	normal = (normalMap.xyz * 2.0f) - 1.0f;

	refractTexcoord = refractTexcoord + (normal.xy * refractionScale);

	refractionColor = refractionTex.Sample(sampleType, refractTexcoord);

	textureColor = colorTex.Sample(sampleType, input.texcoord);

	color = lerp(refractionColor, textureColor, 0.5f);

	return color;
}