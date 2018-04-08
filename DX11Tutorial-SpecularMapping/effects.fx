matrix world;
matrix view;
matrix projection;
Texture2D tex[2];
SamplerState samp
{
    Filter = MIN_MAG_MIP_LINEAR;
    AddressU = Wrap;
    AddressV = Wrap;
};

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

pixelInputType BumpMapVertexShader( vertexInputType input ){
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

float4 BumpMapPixelShader(pixelInputType input) : SV_TARGET{
	float3 lightDir;
	float4 bumpMap;
	float4 texColor;
	float3 bumpNormal;
	float color;
	lightDir = normalize(float3(-1.0f, -1.0f, 1.0f));
	bumpMap = tex[1].Sample(samp, input.texcoord);	
	bumpMap = bumpMap * 2.0f - 1.0f;
	bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.bitangent) + (bumpMap.z * input.normal);	
	bumpNormal = normalize(bumpNormal);
	color = dot(bumpNormal , -lightDir);
	texColor = tex[0].Sample(samp, input.texcoord);

	return color * float4(1.0f,1.0f,1.0f,1.0f) * texColor;
} 


technique10 BumpMapTechnique
{
    pass pass0
    {
        SetVertexShader(CompileShader(vs_5_0, BumpMapVertexShader()));
        SetPixelShader(CompileShader(ps_5_0, BumpMapPixelShader()));
        SetGeometryShader(NULL);
    }
}