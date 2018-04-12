cbuffer ConstantBuffer : register(b0) {
	matrix world;
	matrix view;
	matrix projection;
}

struct vertexInputType {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
	
};

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

pixelInputType main( vertexInputType input ){

	pixelInputType output;
	output.pos = input.pos;	
	output.pos = mul(output.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);
	output.texcoord = input.texcoord;

	return output;
}