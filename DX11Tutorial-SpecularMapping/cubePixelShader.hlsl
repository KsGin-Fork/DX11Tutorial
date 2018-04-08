Texture2D tex[3];
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD;
	float3 normal : NORMAL;
	float3 tangent : TANGENT;
	float3 bitangent : BITANGENT;
	float3 viewDir : TEXCOORD1;
};


float4 main(pixelInputType input) : SV_TARGET {
	float4 texColor;
	float4 bumpMap;
	float4 specularIntensity;
	
	float3 lightDir;
	float3 bumpNormal;
	float lightIntensity;
	float4 color;
	
	float3 reflection;
	float4 specularPower = 16;
	float4 specular;

	texColor = tex[0].Sample(samp, input.texcoord);
	bumpMap = tex[1].Sample(samp, input.texcoord);	
	specularIntensity = tex[2].Sample(samp, input.texcoord);
	
	lightDir = normalize(float3(0.0f, 0.0f, 1.0f));
	lightDir = -lightDir;

	bumpMap = bumpMap * 2.0f - 1.0f;
	bumpNormal = (bumpMap.x * input.tangent) + (bumpMap.y * input.bitangent) + (bumpMap.z * input.normal);	
	bumpNormal = normalize(bumpNormal);

	lightIntensity = saturate(dot(bumpNormal , lightDir));
	color = lightIntensity * float4(1.0f, 1.0f, 1.0f, 1.0f);
	color = color * texColor;

	if (lightIntensity > 0.0f) {		
		reflection = normalize(2 * lightIntensity * bumpNormal - float4(lightDir, 1.0f));
		specular = pow(saturate(dot(reflection, input.viewDir)), specularPower);
		specular = specularIntensity * specular;
	}

	return saturate(color + specular);
} 