cbuffer TessellationBuffer
{
	float tessellationAmount;
	float3 padding;
};


// 输入控制点
struct HullInputType
{
	float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

// 输出控制点
struct HullOutputType
{
	float3 position : POSITION;
    float2 tex : TEXCOORD0;
};


// 输出修补程序常量数据。
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // 例如，对于四象限域，将为 [4]
	float InsideTessFactor			: SV_InsideTessFactor; // 例如，对于四象限域，将为 Inside[2]
};

#define NUM_CONTROL_POINTS 3

// 修补程序常量函数
HS_CONSTANT_DATA_OUTPUT CalcHSPatchConstants(
	InputPatch< HullInputType , NUM_CONTROL_POINTS> ip,
	uint PatchID : SV_PrimitiveID)
{
	HS_CONSTANT_DATA_OUTPUT Output;

	Output.EdgeTessFactor[0] = tessellationAmount;
	Output.EdgeTessFactor[1] = tessellationAmount;
	Output.EdgeTessFactor[2] = tessellationAmount;
	Output.InsideTessFactor = tessellationAmount; 

	return Output;
}

[domain("tri")]
[partitioning("integer")]
[outputtopology("triangle_cw")]
[outputcontrolpoints(3)]
[patchconstantfunc("CalcHSPatchConstants")]
HullOutputType TextureHullShader( 
	InputPatch< HullInputType , NUM_CONTROL_POINTS> ip, 
	uint i : SV_OutputControlPointID,
	uint PatchID : SV_PrimitiveID )
{
	HullOutputType output;

	output.position = ip[i].position;
	output.tex = ip[i].tex;

	return output;
}
