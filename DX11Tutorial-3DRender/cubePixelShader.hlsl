struct pixelInputType {
	float4 pos : SV_POSITION;
	float4 color : COLOR;
};

float4 main(pixelInputType input) : SV_TARGET{
	return input.color;
}