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
	float fogFactor : FOG;
};

pixelInputType main( vertexInputType input ){

	float fogStart = 1.0f;
	float fogEnd = 4.0f;
	float4 modelPosition;
	float4 cameraPosition;
	float distToEye;

	pixelInputType output;

	output.pos = input.pos;	
	output.pos = mul(output.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	output.texcoord = input.texcoord;

	modelPosition = mul(input.pos, world);
	modelPosition = mul(modelPosition, view);

	cameraPosition = float4(1.0f, 1.0f, -1.0f, 0.0f);

	distToEye = length(cameraPosition - modelPosition);
		
	output.fogFactor = saturate((distToEye - fogStart) / (fogEnd - fogStart));

	return output;
}