#pragma once

//////////////
// INCLUDES //
//////////////
#include <D3D11.h>
#include <D3DX10math.h>
#include <d3dx11async.h>
#include <fstream>
#include <DirectXMath.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: GlassShaderClass
////////////////////////////////////////////////////////////////////////////////
class GlassShaderClass
{
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct GlassBufferType 
	{
		float refractionScale;
		DirectX::XMFLOAT3 padding;
	};

public:
	GlassShaderClass();
	GlassShaderClass(const GlassShaderClass&);
	~GlassShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView* , ID3D11ShaderResourceView* , ID3D11ShaderResourceView* , float);

private:
	bool InitializeShader(ID3D11Device*, HWND, char*, char*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, char*);

	bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView* , ID3D11ShaderResourceView* , ID3D11ShaderResourceView* , float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_glassBuffer;
};
