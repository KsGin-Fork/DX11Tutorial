struct vertexInputType {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD0;
};

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

pixelInputType main( vertexInputType input )
{
	pixelInputType output;
	output.pos = input.pos;
	output.texcoord = input.texcoord;
	return output;
}