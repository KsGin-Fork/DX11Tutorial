Texture2D tex[2];
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};


float4 main(pixelInputType input) : SV_TARGET{

	float3 lightDir = float3(0.0f, 0.0f, -1.0f);

	float4 bumpMap = (tex[1].Sample(samp, input.texcoord) * 2.0f) - 1.0f;
	float3 bumpNormal = normalize((bumpMap.x * input.tangent) + (bumpMap.y * input.bitangent) + (bumpMap.z * input.normal));
	float color = saturate(dot(input.normal , lightDir));
	return (color + 0.3f) * tex[0].Sample(samp, input.texcoord);
} 