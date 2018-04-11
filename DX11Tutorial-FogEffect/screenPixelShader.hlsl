Texture2D tex;
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

float4 main(pixelInputType input) : SV_TARGET{
	return tex.Sample(samp, input.texcoord);
} 