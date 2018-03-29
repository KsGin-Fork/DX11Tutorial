Texture2D tex;
SamplerState samp;

struct pixelInputType {
	float4 pos : SV_POSITION;
	float2 texcoord : TEXCOORD0;
};

float4 Inversion(float4 color) {
	color = float4(1.0f , 1.0f , 1.0f , 1.0f) - color;
	color.w = 1.0f;
	return color;
}

float4 Grayscale(float4 color) {
	float rgbValue = (color.x + color.y + color.z) / 3;
	return float4(rgbValue, rgbValue, rgbValue, 1.0f);
}


float4 main(pixelInputType input) : SV_TARGET{
	float4 color = tex.Sample(samp, input.texcoord);
	if (input.pos.y > 540) {
		color = Grayscale(color);
	}
	if (input.pos.x > 960) {
		color = Inversion(color);
	} 
	return color;
} 