////////////////////////////////////////////////////////////////////////////////
// Filename: watershaderclass.h
////////////////////////////////////////////////////////////////////////////////
#ifndef _WATERSHADERCLASS_H_
#define _WATERSHADERCLASS_H_


//////////////
// INCLUDES //
//////////////
#include <D3D11.h>
#include <d3dx11async.h>
#include <fstream>
#include <DirectXMath.h>
using namespace std;


////////////////////////////////////////////////////////////////////////////////
// Class name: WaterShaderClass
////////////////////////////////////////////////////////////////////////////////
class WaterShaderClass
{
private:
	struct MatrixBufferType
	{
		DirectX::XMMATRIX world;
		DirectX::XMMATRIX view;
		DirectX::XMMATRIX projection;
	};

	struct ReflectionBufferType
	{
		DirectX::XMMATRIX reflection;
	};

	struct WaterBufferType
	{
		float waterTranslation;
		float reflectRefractScale;
		DirectX::XMFLOAT2 padding;
	};

public:
	WaterShaderClass();
	WaterShaderClass(const WaterShaderClass&);
	~WaterShaderClass();

	bool Initialize(ID3D11Device*, HWND);
	void Shutdown();
	bool Render(ID3D11DeviceContext*, int, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*, 
				ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float);

private:
	bool InitializeShader(ID3D11Device*, HWND, CHAR*, CHAR*);
	void ShutdownShader();
	void OutputShaderErrorMessage(ID3D10Blob*, HWND, CHAR*);

	bool SetShaderParameters(ID3D11DeviceContext*, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, DirectX::XMMATRIX, ID3D11ShaderResourceView*,
							 ID3D11ShaderResourceView*, ID3D11ShaderResourceView*, float, float);
	void RenderShader(ID3D11DeviceContext*, int);

private:
	ID3D11VertexShader* m_vertexShader;
	ID3D11PixelShader* m_pixelShader;
	ID3D11InputLayout* m_layout;
	ID3D11SamplerState* m_sampleState;
	ID3D11Buffer* m_matrixBuffer;
	ID3D11Buffer* m_reflectionBuffer;
	ID3D11Buffer* m_waterBuffer;
};

#endif