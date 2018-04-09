Texture2D tex[1];
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float3 viewDir : TEXCOORD1;
};


float4 main(pixelInputType input) : SV_TARGET {
	return tex[0].Sample(samp, input.texcoord);
} 