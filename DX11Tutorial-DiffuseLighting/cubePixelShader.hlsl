Texture2D tex;
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
};

float4 main(pixelInputType input) : SV_TARGET{

	float ambient = 0.1f;
	float3 lightPos = float3(0.0f, 3.0f, 0.0f);
	float3 lightDir = - lightPos;
	float diffuse = saturate(dot(normalize(input.normal), normalize(-lightDir)));

	return tex.Sample(samp , input.texcoord) * (ambient + diffuse);
}