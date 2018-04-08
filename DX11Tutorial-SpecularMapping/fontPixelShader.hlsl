Texture2D tex;
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};


float4 main(pixelInputType input) : SV_TARGET{
	float4 color = tex.Sample(samp, input.texcoord);
	if (color.x + color.y + color.z == 3.0f) return float4(1.0f, 1.0f, 0.0f, 1.0f);
	discard;
	return float4(1.0f, 1.0f, 1.0f, 1.0f);
} 