Texture2D tex[5];
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};


float4 main(pixelInputType input) : SV_TARGET{
	//return tex[0].Sample(samp, input.texcoord) * (tex[1].Sample(samp, input.texcoord) + 0.1f);

	float4 alpha = tex[2].Sample(samp, input.texcoord);
	return tex[3].Sample(samp, input.texcoord) * alpha + tex[4].Sample(samp, input.texcoord) * (1-alpha);
} 