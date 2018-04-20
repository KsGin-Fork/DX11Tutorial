struct VertexInputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct HullInputType
{
    float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

HullInputType TextureVertexShader(VertexInputType input)
{
	HullInputType output;
	output.position = input.position;
	output.tex = input.tex;
    return output;
}