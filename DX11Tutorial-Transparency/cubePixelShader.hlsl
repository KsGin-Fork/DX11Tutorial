Texture2D tex[1];
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
};


float4 main(pixelInputType input) : SV_TARGET {
	float4 color = tex[0].Sample(samp, input.texcoord);
	color.a = 0.5f;
	return color;
} 