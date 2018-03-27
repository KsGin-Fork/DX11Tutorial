cbuffer ConstantBuffer : register(b0) {
	matrix world;
	matrix view;
	matrix projection;
}

float4 main( float4 pos : POSITION ) : SV_POSITION{
	pos = mul(pos , world);
	pos = mul(pos , view);
	pos = mul(pos , projection);
	return pos;
}