Texture2D tex;
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(pixelInputType input) : SV_TARGET{

	float ambient = 0.1f;

	return tex.Sample(samp , input.texcoord) * (ambient);
}