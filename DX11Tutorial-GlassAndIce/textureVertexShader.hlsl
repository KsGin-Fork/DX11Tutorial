cbuffer MatrixType 
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexShaderInput 
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelShaderInput 
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelShaderInput TextureVertexShader(VertexShaderInput input)
{
	PixelShaderInput output;
	output.pos = mul(input.pos, worldMatrix);
	output.pos = mul(output.pos, viewMatrix);
	output.pos = mul(output.pos, projectionMatrix);

	output.tex = input.tex;

	return output;
}