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


float4 doFilter(float3x3 fil , float2 texSize , float2 uv) {
	float2 posDelta[3][3] = {		// 3x3偏移坐标
		{float2(-1.0f , -1.0f) , float2(0.0f , -1.0f) , float2(1.0f , -1.0f)} ,
		{float2(-1.0f , 0.0f) , float2(0.0f , 0.0f) , float2(1.0f , 0.0f)},
		{float2(-1.0f , 1.0f) , float2(0.0f , 1.0f) , float2(1.0f , 1.0f)}
	};

	float4 color = float4(0.0f, 0.0f, 0.0f, 1.0f);

	for (int i = 0; i < 3; i++) {
		for (int j = 0; j < 3; j++) {
			float2 tUV = float2(uv.x + posDelta[i][j].x , uv.y + posDelta[i][j].y);
			// 加到最终颜色上
			color += tex.Sample(samp, tUV / texSize) * fil[i][j];
		};
	}
	color.w = 1.0f;	// 透明度矫正
	return color;
}

float4 main(pixelInputType input) : SV_TARGET{
	float2 texSize = float2(1920 , 1200);		// 图片的大小
	float3x3 fil = float3x3(
		1.0f, 2.0f, 1.0f, 
		2.0f, 4.0f, 2.0f, 
		1.0f, 2.0f, 1.0f
	) / 16;
	float2 uv = input.texcoord * texSize;
	return doFilter(fil , texSize , uv);	
} 