Texture2D tex;
SamplerState sam;

struct PixelShaderInput 
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

float4 TexturePixelShader(PixelShaderInput input) : SV_TARGET
{
	return tex.Sample(sam , input.tex);
}