cbuffer ConstantBuffer : register(b0) {
	matrix world;
	matrix view;
	matrix projection;
}

struct vertexInputType {
	float4 pos : POSITION;
	float4 color : COLOR;
};

struct pixelInputType {
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

pixelInputType main( vertexInputType input ){
	pixelInputType output;
	output.pos = input.pos;
	output.pos = mul(output.pos , world);
	output.pos = mul(output.pos , view);
	output.pos = mul(output.pos , projection);
	output.color = input.color;
	return output;
}