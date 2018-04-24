////////////////////////////////////////////////////////////////////////////////
// Filename: shadowshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _SHADOWSHADERCLASS_H_
#define _SHADOWSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <D3D11.h>
#include <D3DX11async.h>
#include <fstream>
#include <DirectXMath.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: ShadowShaderClass
////////////////////////////////////////////////////////////////////////////////
class ShadowShaderClass
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
		DirectX::XMMATRIX lightView;
		DirectX::XMMATRIX lightProjection;
		DirectX::XMMATRIX lightView2;
		DirectX::XMMATRIX lightProjection2;
	};

	struct LightBufferType
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT4 diffuseColor2;
	};

	struct LightBufferType2
	{
		DirectX::XMFLOAT3 lightPosition;
		float padding;
		DirectX::XMFLOAT3 lightPosition2;
		float padding2;
	};

public:
	ShadowShaderClass();
	ShadowShaderClass(const ShadowShaderClass&);
	~ShadowShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*, 
				ID3D11ShaderResourceView*, DirectX::XMFLOAT3, DirectX::XMFLOAT4, DirectX::XMFLOAT4, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*,
				DirectX::XMFLOAT3, DirectX::XMFLOAT4);

private:
	bool InitializeShader(ID3D11Device*, HWND, CHAR*, CHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, CHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*, 
							 ID3D11ShaderResourceView*, DirectX::XMFLOAT3, DirectX::XMFLOAT4, DirectX::XMFLOAT4, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*,
							 DirectX::XMFLOAT3, DirectX::XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleStateWrap;
	ID3D11SamplerState* m_sampleStateClamp;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_lightBuffer2;
};

#endif