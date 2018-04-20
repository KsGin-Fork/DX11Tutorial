cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct HullOutputType
{
	float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 position : SV_POSITION;
    float2 tex : TEXCOORD0;
};


// 输出修补程序常量数据。
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; 
	float InsideTessFactor			: SV_InsideTessFactor; 
};

#define NUM_CONTROL_POINTS 3

[domain("tri")]
PixelInputType TextureDomainShader(
	HS_CONSTANT_DATA_OUTPUT input,
	float3 domain : SV_DomainLocation,
	const OutputPatch< HullOutputType , NUM_CONTROL_POINTS> patch)
{
	PixelInputType output;
	float3 vertexPosition;

	vertexPosition = domain.x * patch[0].position + domain.y * patch[1].position + domain.z * patch[2].position;

	// Calculate the position of the vertex against the world, view, and projection matrices.W
    output.position = mul(float4(vertexPosition , 1.0f), worldMatrix);
    output.position = mul(output.position, viewMatrix);
    output.position = mul(output.position, projectionMatrix);

	output.tex = domain.x * patch[0].tex + domain.y * patch[1].tex + domain.z * patch[2].tex;

	return output;
}
