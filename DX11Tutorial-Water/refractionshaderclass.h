////////////////////////////////////////////////////////////////////////////////
// Filename: refractionshaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _REFRACTIONSHADERCLASS_H_
#define _REFRACTIONSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <D3D11.h>
#include <d3dx10math.h>
#include <fstream>
#include <DirectXMath.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: RefractionShaderClass
////////////////////////////////////////////////////////////////////////////////
class RefractionShaderClass
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct LightBufferType
	{
		DirectX::XMFLOAT4 ambientColor;
		DirectX::XMFLOAT4 diffuseColor;
		DirectX::XMFLOAT3 lightDirection;
		float padding;
	};

	struct ClipPlaneBufferType
	{
		DirectX::XMFLOAT4 clipPlane;
	};

public:
	RefractionShaderClass();
	RefractionShaderClass(const RefractionShaderClass&);
	~RefractionShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*, 
		DirectX::XMFLOAT3, DirectX::XMFLOAT4, DirectX::XMFLOAT4, DirectX::XMFLOAT4);

private:
	bool InitializeShader(ID3D11Device*, HWND, CHAR*, CHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, CHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*, 
		DirectX::XMFLOAT3, DirectX::XMFLOAT4, DirectX::XMFLOAT4, DirectX::XMFLOAT4);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_lightBuffer;
	ID3D11Buffer* m_clipPlaneBuffer;
};

#endif