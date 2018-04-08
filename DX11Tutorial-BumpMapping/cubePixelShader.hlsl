Texture2D tex[2];
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};


float4 main(pixelInputType input) : SV_TARGET{
	float3 lightDir;
	float4 bumpMap;
	float4 texColor;
	float3 bumpNormal;
	float color;
	lightDir = normalize(float3(-1.0f, -1.0f, 1.0f));
	bumpMap = tex[1].Sample(samp, input.texcoord);	
	bumpMap = bumpMap * 2.0f - 1.0f;
	bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.bitangent) + (bumpMap.z * input.normal);	
	bumpNormal = normalize(bumpNormal);
	color = dot(bumpNormal , -lightDir);
	texColor = tex[0].Sample(samp, input.texcoord);

	return color * float4(1.0f,1.0f,1.0f,1.0f) * texColor;
} 