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
	float2 texcoord : TEXCOORD0;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float3 viewDir : TEXCOORD1;
};

pixelInputType main( vertexInputType input ){
	pixelInputType output;
	float4 worldPosition;
	float3 cameraPos = float3(1.0f, 1.0f, -1.0f);

	output.pos = input.pos;	
	output.pos = mul(output.pos, world);
	output.pos = mul(output.pos, view);
	output.pos = mul(output.pos, projection);

	output.texcoord = input.texcoord;

	output.normal = mul(input.normal, (float3x3)world);
	output.normal = normalize(output.normal);

	output.tangent = mul(input.tangent, (float3x3)world);
	output.tangent = normalize(output.tangent);

	output.bitangent = mul(input.bitangent, (float3x3)world);
	output.bitangent = normalize(output.bitangent);

	worldPosition = mul(worldPosition, world);
	output.viewDir = cameraPos - output.pos.xyz;
	output.viewDir = normalize(output.viewDir);

	return output;
}