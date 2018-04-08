cbuffer ConstantBuffer : register(b0) {
	matrix world;
	matrix view;
	matrix projection;
}

struct vertexInputType {
	float4 pos : POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
};

pixelInputType main( vertexInputType input ){
	pixelInputType output;

	output.pos = input.pos;	
	output.pos = mul(output.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	output.texcoord = input.texcoord;

	output.normal = mul(normalize(input.normal), (float3x3)world);
	output.normal = normalize(output.normal);

	output.tangent = mul(normalize(input.tangent), (float3x3)world);
	output.tangent = normalize(output.tangent);

	output.bitangent = mul(normalize(input.bitangent), (float3x3)world);
	output.bitangent = normalize(output.bitangent);

		
	return output;
}