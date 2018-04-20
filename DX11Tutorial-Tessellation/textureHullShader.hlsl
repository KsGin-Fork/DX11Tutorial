cbuffer TessellationBuffer
{
	float tessellationAmount;
	float3 padding;
};


// ������Ƶ�
struct HullInputType
{
	float3 position : POSITION;
    float2 tex : TEXCOORD0;
};

// ������Ƶ�
struct HullOutputType
{
	float3 position : POSITION;
    float2 tex : TEXCOORD0;
};


// ����޲����������ݡ�
struct HS_CONSTANT_DATA_OUTPUT
{
	float EdgeTessFactor[3]			: SV_TessFactor; // ���磬�����������򣬽�Ϊ [4]
	float InsideTessFactor			: SV_InsideTessFactor; // ���磬�����������򣬽�Ϊ Inside[2]
};

#define NUM_CONTROL_POINTS 3

// �޲�����������
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
