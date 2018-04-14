cbuffer MatrixBuffer
{
	matrix worldMatrix;
	matrix viewMatrix;
	matrix projectionMatrix;
};

struct VertexInputType
{
	float4 pos : POSITION;
	float2 tex : TEXCOORD0;
};

struct PixelInputType
{
	float4 pos : SV_POSITION;
	float2 tex : TEXCOORD0;
};

PixelInputType FadeVertexShader(VertexInputType input) {
	
	PixelInputType output;   

    // Change the position vector to be 4 units for proper matrix calculations.
    input.pos.w = 1.0f;

    // Calculate the position of the vertex against the world, view, and projection matrices.
    output.pos = mul(input.pos, worldMatrix);
    output.pos = mul(output.pos, viewMatrix);
    output.pos = mul(output.pos, projectionMatrix);
    
    // Store the texture coordinates for the pixel shader.
    output.tex = input.tex;
    
    return output;
}