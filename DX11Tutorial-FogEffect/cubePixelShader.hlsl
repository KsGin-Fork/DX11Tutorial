Texture2D tex[1];
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float fogFactor : FOG;
};


float4 main(pixelInputType input) : SV_TARGET {
	float4 fogColor = float4(0.5f, 0.5f, 0.5f, 1.0f);
	float4 texColor = tex[0].Sample(samp, input.texcoord);
	
	return fogColor * input.fogFactor + texColor * (1 - input.fogFactor);
} 