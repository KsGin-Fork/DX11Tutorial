#include <D3D11.h>
#include <minwinbase.h>
#include <iostream>
#include <Windows.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>


const int width = 1920, height = 1080;

HRESULT CALLBACK WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);

using namespace std;
using namespace DirectX;

struct Vertex
{
	XMFLOAT3 pos;
	XMFLOAT2 texcoord;
	XMFLOAT3 normal;
};

struct Constant {
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

/**
 * \brief ����������
 * \param hInstance ����ʵ��
 * \param hPrevInstance ��һ������ʵ��
 * \param lpCmdLine �����в���
 * \param nCmdShow ��ʾ��ʽ
 * \return �˳���
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {

	///////////////////////////////////////////////////////////////// INIT WIN32 ///////////////////////////////////////////////////////

	WNDCLASS wc;
	ZeroMemory(&wc, sizeof(WNDCLASS));
	wc.hInstance = hInstance;
	wc.lpfnWndProc = static_cast<WNDPROC>(WndProc);
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDC_ICON);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(WHITE_BRUSH));
	wc.lpszClassName = "mWndClass";
	wc.lpszMenuName = nullptr;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	if (!RegisterClass(&wc)) {
		MessageBox(nullptr, "ERROR::RegisterClass_Error", "Error", MB_OK);
		return -1;
	}
	const auto hWnd = CreateWindow("mWndClass", "DX11Tutorial-TextureTriangle", WS_EX_TOPMOST | WS_OVERLAPPEDWINDOW, 0, 0, width, height, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		MessageBox(nullptr, "ERROR::CreateWindow_Error", "Error", MB_OK);
		return -1;
	}

	ShowWindow(hWnd, SW_NORMAL);
	UpdateWindow(hWnd);

	////////////////////////////////////////////////////////////////////////////////// INIT DX //////////////////////////////////////////////////////////////

	DXGI_SWAP_CHAIN_DESC dc;
	ZeroMemory(&dc, sizeof(DXGI_SWAP_CHAIN_DESC));
	dc.BufferDesc.Width = width;
	dc.BufferDesc.Height = height;
	dc.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	dc.BufferDesc.RefreshRate.Numerator = 60;
	dc.BufferDesc.RefreshRate.Denominator = 1;
	dc.BufferCount = 1;
	dc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	dc.SampleDesc.Count = 1;
	dc.SampleDesc.Quality = 0;
	dc.OutputWindow = hWnd;
	dc.Windowed = true;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGISwapChain *pSwapChain = nullptr;
	ID3D11RenderTargetView *pRenderTargetView = nullptr;
	ID3D11Device *pDevice = nullptr;
	ID3D11DeviceContext *pImmediateContext = nullptr;
	D3D_FEATURE_LEVEL eFeatureLevel = D3D_FEATURE_LEVEL_11_0;

	D3D_FEATURE_LEVEL aFeatureLevels[] = {
		D3D_FEATURE_LEVEL_11_0 ,
		D3D_FEATURE_LEVEL_10_1 ,
		D3D_FEATURE_LEVEL_10_0
	};
	const auto nFeatureLevel = ARRAYSIZE(aFeatureLevels);

	HRESULT hr = S_OK;
	hr = D3D11CreateDeviceAndSwapChain(
		nullptr,
		D3D_DRIVER_TYPE_HARDWARE,
		nullptr,
		createDeviceFlags,
		aFeatureLevels,
		nFeatureLevel,
		D3D11_SDK_VERSION,
		&dc,
		&pSwapChain,
		&pDevice,
		&eFeatureLevel,
		&pImmediateContext);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateDeviceAndSwapChain_Error", "Error", MB_OK);
		return hr;
	}

	ID3D11Texture2D *pTexture2D = nullptr;
	hr = pSwapChain->GetBuffer(0, __uuidof(pTexture2D), reinterpret_cast<LPVOID*>(&pTexture2D));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::SetTextureBuffer_Error", "Error", MB_OK);
		return hr;
	}
	hr = pDevice->CreateRenderTargetView(pTexture2D, nullptr, &pRenderTargetView);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateRenderTargetView_Error", "Error", MB_OK);
		return hr;
	}
	pImmediateContext->OMSetRenderTargets(1, &pRenderTargetView, nullptr);
	D3D11_VIEWPORT dv{ 0 , 0 , width , height , 0 , 1 };
	pImmediateContext->RSSetViewports(1, &dv);

	//////////////////////////////////////////////////////////////////////// Vertex ///////////////////////////////////////////////////////////////////////

	Vertex vertices[] = {
		// ����
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0, 1) , XMFLOAT3(0.0f , 0.0f , -1.0f)},
		{ XMFLOAT3(-0.5f, +0.5f, -0.5f), XMFLOAT2(0, 0) , XMFLOAT3(0.0f , 0.0f , -1.0f)},
		{ XMFLOAT3(+0.5f, +0.5f, -0.5f), XMFLOAT2(1, 0) , XMFLOAT3(0.0f , 0.0f , -1.0f)},
		{ XMFLOAT3(+0.5f, -0.5f, -0.5f), XMFLOAT2(1, 1) , XMFLOAT3(0.0f , 0.0f , -1.0f)},

		// ����
		{ XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT2(1, 1) , XMFLOAT3(0.0f , 0.0f , +1.0f)},
		{ XMFLOAT3(-0.5f, +0.5f, +0.5f), XMFLOAT2(1, 0) , XMFLOAT3(0.0f , 0.0f , +1.0f)},
		{ XMFLOAT3(+0.5f, +0.5f, +0.5f), XMFLOAT2(0, 0) , XMFLOAT3(0.0f , 0.0f , +1.0f)},
		{ XMFLOAT3(+0.5f, -0.5f, +0.5f), XMFLOAT2(0, 1) , XMFLOAT3(0.0f , 0.0f , +1.0f)},

		// ����
		{ XMFLOAT3(-0.5f, +0.5f, -0.5f), XMFLOAT2(0, 1) , XMFLOAT3(0.0f , +1.0f , 0.0f)},
		{ XMFLOAT3(-0.5f, +0.5f, +0.5f), XMFLOAT2(0, 0) , XMFLOAT3(0.0f , +1.0f , 0.0f)},
		{ XMFLOAT3(+0.5f, +0.5f, +0.5f), XMFLOAT2(1, 0) , XMFLOAT3(0.0f , +1.0f , 0.0f)},
		{ XMFLOAT3(+0.5f, +0.5f, -0.5f), XMFLOAT2(1, 1) , XMFLOAT3(0.0f , +1.0f , 0.0f)},

		// ����
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(1, 1) , XMFLOAT3(0.0f , -1.0f , 0.0f)},
		{ XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT2(1, 0) , XMFLOAT3(0.0f , -1.0f , 0.0f)},
		{ XMFLOAT3(+0.5f, -0.5f, +0.5f), XMFLOAT2(0, 0) , XMFLOAT3(0.0f , -1.0f , 0.0f)},
		{ XMFLOAT3(+0.5f, -0.5f, -0.5f), XMFLOAT2(0, 1) , XMFLOAT3(0.0f , -1.0f , 0.0f)},

		// ���
		{ XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT2(0, 1) , XMFLOAT3(-1.0f , 0.0f , 0.0f)},
		{ XMFLOAT3(-0.5f, +0.5f, +0.5f), XMFLOAT2(0, 0) , XMFLOAT3(-1.0f , 0.0f , 0.0f)},
		{ XMFLOAT3(-0.5f, +0.5f, -0.5f), XMFLOAT2(1, 0) , XMFLOAT3(-1.0f , 0.0f , 0.0f)},
		{ XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(1, 1) , XMFLOAT3(-1.0f , 0.0f , 0.0f)},

		// �Ҳ�
		{ XMFLOAT3(+0.5f, -0.5f, -0.5f), XMFLOAT2(0, 1) , XMFLOAT3(+1.0f , 0.0f , 0.0f)},
		{ XMFLOAT3(+0.5f, +0.5f, -0.5f), XMFLOAT2(0, 0) , XMFLOAT3(+1.0f , 0.0f , 0.0f)},
		{ XMFLOAT3(+0.5f, +0.5f, +0.5f), XMFLOAT2(1, 0) , XMFLOAT3(+1.0f , 0.0f , 0.0f)},
		{ XMFLOAT3(+0.5f, -0.5f, +0.5f), XMFLOAT2(1, 1) , XMFLOAT3(+1.0f , 0.0f , 0.0f)},
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA verticesSourceData;
	ZeroMemory(&verticesSourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	verticesSourceData.pSysMem = vertices;
	ID3D11Buffer *pVertexBufferObject = nullptr;
	pDevice->CreateBuffer(&vertexBufferDesc, &verticesSourceData, &pVertexBufferObject);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pImmediateContext->IASetVertexBuffers(0, 1, &pVertexBufferObject, &stride, &offset);
	pImmediateContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	/////////////////////////////////////////////////////////////////////// index //////////////////////////////////////////////////////////////
	UINT indices[] = {
		// front face
		0, 1, 2,
		0, 2, 3,
		// back face
		4, 6, 5,
		4, 7, 6,
		// left face
		16, 17, 18,
		16, 18, 19,
		// right face
		20, 21, 22,
		20, 22, 23,
		// top face
		8, 9, 10,
		8, 10, 11,
		// bottom face
		12, 14, 13,
		12, 15, 14
	};

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(indexDesc));
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(UINT) * 36;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices;

	ID3D11Buffer *pIndexBufferObject = nullptr;
	hr = pDevice->CreateBuffer(&indexDesc, &indexData, &pIndexBufferObject);
	if (FAILED(hr))
	{
		return false;
	}

	pImmediateContext->IASetIndexBuffer(pIndexBufferObject, DXGI_FORMAT_R32_UINT, 0);


	///////////////////////////////////////////////////////////// Constant ////////////////////////////////////////////////////

	Constant cb = {
		XMMatrixTranspose(XMMatrixIdentity()) ,
		XMMatrixTranspose(XMMatrixLookAtLH(
			XMVectorSet(0.0f, 1.0f, -1.0f, 0.0f),
			XMVectorSet(0.0f, 0.0f, 0.0f, 0.0f),
			XMVectorSet(0.0f, 1.0f, 0.0f, 0.0f)
		)),
		XMMatrixTranspose(XMMatrixPerspectiveFovLH(90, static_cast<FLOAT>(width) / height, 0.1f, 100.0f))
	};

	D3D11_BUFFER_DESC constantBufferDesc;
	ZeroMemory(&constantBufferDesc, sizeof(constantBufferDesc));
	constantBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	constantBufferDesc.BindFlags = D3D10_BIND_CONSTANT_BUFFER;
	constantBufferDesc.ByteWidth = sizeof(Constant);

	D3D11_SUBRESOURCE_DATA constantSourceData;
	ZeroMemory(&constantSourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	constantSourceData.pSysMem = &cb;

	ID3D11Buffer *pConstantBuffer = nullptr;
	hr = pDevice->CreateBuffer(&constantBufferDesc, &constantSourceData, &pConstantBuffer);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateConstantBuffer", "ERROR", MB_OK);
		return hr;
	}

	///////////////////////////////////////////////////////// Shader ////////////////////////////////////////////////////////////

	ID3D11VertexShader *pVertexShader = nullptr;
	ID3D11PixelShader *pPixelShader = nullptr;
	ID3D11InputLayout *pInputLayout = nullptr;

	ID3D10Blob* pErrorMessage = nullptr;
	ID3D10Blob* pVertexShaderBlob = nullptr;
	ID3D10Blob* pPixelShaderBlob = nullptr;

	hr = D3DX11CompileFromFile("./triangleVertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", D3DCOMPILER_STRIP_DEBUG_INFO, 0, nullptr, &pVertexShaderBlob, &pErrorMessage, nullptr);
	if (FAILED(hr)) {
		if (pErrorMessage) MessageBox(NULL, static_cast<CHAR*>(pErrorMessage->GetBufferPointer()), "Error", MB_OK);
		else MessageBox(NULL, "Triangle.vs File Not Found", "Error", MB_OK);
		return hr;
	}
	hr = D3DX11CompileFromFile("./trianglePixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", D3DCOMPILER_STRIP_DEBUG_INFO, 0, nullptr, &pPixelShaderBlob, &pErrorMessage, nullptr);
	if (FAILED(hr)) {
		if (pErrorMessage) MessageBox(NULL, static_cast<CHAR*>(pErrorMessage->GetBufferPointer()), "Error", MB_OK);
		else MessageBox(NULL, "Triangle.vs File Not Found", "Error", MB_OK);
		return hr;
	}

	hr = pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, &pVertexShader);
	if (FAILED(hr)) {
		MessageBox(NULL, "ERROR::CreateVertexShader", "Error", MB_OK);
		return hr;
	}

	hr = pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, &pPixelShader);
	if (FAILED(hr)) {
		MessageBox(NULL, "ERROR::CreatePixelShader", "Error", MB_OK);
		return hr;
	}

	D3D11_SAMPLER_DESC samplerDesc;
	samplerDesc.Filter = D3D11_FILTER_MIN_MAG_MIP_LINEAR;
	samplerDesc.AddressU = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressV = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.AddressW = D3D11_TEXTURE_ADDRESS_WRAP;
	samplerDesc.MipLODBias = 0.0f;
	samplerDesc.MaxAnisotropy = 1;
	samplerDesc.ComparisonFunc = D3D11_COMPARISON_ALWAYS;
	samplerDesc.BorderColor[0] = 0;
	samplerDesc.BorderColor[1] = 0;
	samplerDesc.BorderColor[2] = 0;
	samplerDesc.BorderColor[3] = 0;
	samplerDesc.MinLOD = 0;
	samplerDesc.MaxLOD = D3D11_FLOAT32_MAX;

	ID3D11SamplerState *pSamplerState = nullptr;
	ID3D11ShaderResourceView *pShaderResourceView;
	hr = pDevice->CreateSamplerState(&samplerDesc, &pSamplerState);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateSampler", "Error", MB_OK);
		return hr;
	}

	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, "./triangle.jpg", nullptr, nullptr, &pShaderResourceView, nullptr);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateShaderResourceView", "Error", MB_OK);
		return hr;
	}

	pImmediateContext->PSSetShaderResources(0, 1, &pShaderResourceView);
	pImmediateContext->PSSetSamplers(0, 1, &pSamplerState);

	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } ,
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 } ,
		{ "NORMAL", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	const UINT numElements = ARRAYSIZE(layout);
	hr = pDevice->CreateInputLayout(layout, numElements, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &pInputLayout);
	if (FAILED(hr)) {
		MessageBox(NULL, "ERROR::CreateInputLayout", "Error", MB_OK);
		return hr;
	}
	pImmediateContext->IASetInputLayout(pInputLayout);

	////////////////////////////////////////////////////////////////////// MSG ///////////////////////////////////////////////////////////
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		float color[] = { 0.1f , 0.2f , 0.3f , 1.0f };

		cb.world = cb.world * XMMatrixRotationY(0.0001f);

		pImmediateContext->UpdateSubresource(pConstantBuffer, 0, nullptr, &cb, 0, 0);
		pImmediateContext->VSSetConstantBuffers(0, 1, &pConstantBuffer);
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, color);
		pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);
		pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);
		pImmediateContext->DrawIndexed(36, 0, 0);
		pSwapChain->Present(0, 0);
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

	pSamplerState->Release();
	pShaderResourceView->Release();
	pConstantBuffer->Release();
	pSwapChain->Release();
	pImmediateContext->Release();
	pRenderTargetView->Release();
	pDevice->Release();
	pTexture2D->Release();
	pVertexBufferObject->Release();
	pVertexShader->Release();
	pPixelShader->Release();
	pVertexShaderBlob->Release();
	pPixelShaderBlob->Release();
	pInputLayout->Release();
	if (pErrorMessage) {
		pErrorMessage->Release();
	}


	return 0;
}



/**
 * \brief ��Ϣ�����ص�����
 * \param hWnd ���ھ��
 * \param uMsg ��Ϣ
 * \param wParam ��Ϣ���Ӳ���
 * \param lParam ��Ϣ���Ӳ���
 * \return ����Ĭ����Ϣ����
 */
HRESULT CALLBACK WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hWnd);
		}
		break;
	default: break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}