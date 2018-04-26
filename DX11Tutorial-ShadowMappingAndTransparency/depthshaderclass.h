////////////////////////////////////////////////////////////////////////////////
// Filename: depthshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _DEPTHSHADERCLASS_H_
#define _DEPTHSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <D3D11.h>
#include <D3DX11async.h>
#include <fstream>
#include <DirectXMath.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: DepthShaderClass
////////////////////////////////////////////////////////////////////////////////
class DepthShaderClass
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

public:
	DepthShaderClass();
	DepthShaderClass(const DepthShaderClass&);
	~DepthShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*);

private:
	bool InitializeShader(ID3D11Device*, HWND, CHAR*, CHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, CHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11SamplerState* m_sampleState;
};

#endif