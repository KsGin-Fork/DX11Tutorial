Texture2D tex[2];
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};


float4 main(pixelInputType input) : SV_TARGET{
	return tex[0].Sample(samp, input.texcoord) * 0.5f + tex[1].Sample(samp, input.texcoord) * 0.5;
} 