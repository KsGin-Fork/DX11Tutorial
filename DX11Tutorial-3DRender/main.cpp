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

struct Vertex {
	DirectX::XMFLOAT3 pos;
	DirectX::XMFLOAT2 tex;
};


/**
 * \brief 启动主方法
 * \param hInstance 程序实例
 * \param hPrevInstance 上一个程序实例
 * \param lpCmdLine 命令行参数
 * \param nCmdShow 显示方式
 * \return 退出码
 */
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {

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
	

	static const Vertex vertices[] = {
			{ DirectX::XMFLOAT3(0.3f,0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
			{ DirectX::XMFLOAT3(0.3f,-0.3f, 1.0f)	, DirectX::XMFLOAT2(1.0f,1.0f) },
			{ DirectX::XMFLOAT3(-0.3f, -0.3f, 1.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
			{ DirectX::XMFLOAT3(-0.3f, -0.3f,1.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
			{ DirectX::XMFLOAT3(-0.3f, 0.3f,1.0f)	, DirectX::XMFLOAT2(0.0f,0.0f) },
			{ DirectX::XMFLOAT3(0.3f, 0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },

//			{ DirectX::XMFLOAT3(0.3f,-0.3f, 3.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
//			{ DirectX::XMFLOAT3(-0.3f,-0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f,-0.3f,1.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f, 0.3f,1.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f,-0.3f,1.0f)	, DirectX::XMFLOAT2(0.0f,0.0f) },
//			{ DirectX::XMFLOAT3(-0.3f,-0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
//			{ DirectX::XMFLOAT3(-0.3f,-0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
//			{ DirectX::XMFLOAT3(-0.3f, 0.3f, 3.0f)	, DirectX::XMFLOAT2(1.0f,1.0f) },
//			{ DirectX::XMFLOAT3(-0.3f, 0.3f,1.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f,-0.3f, 3.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(-0.3f,-0.3f, 3.0f)	, DirectX::XMFLOAT2(0.0f,0.0f) },
//			{ DirectX::XMFLOAT3(-0.3f,-0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
//			{ DirectX::XMFLOAT3(-0.3f, 0.3f, 3.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
//			{ DirectX::XMFLOAT3(-0.3f,-0.3f, 3.0f)	, DirectX::XMFLOAT2(1.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f,-0.3f, 3.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f, 0.3f, 3.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f,-0.3f,1.0f)	, DirectX::XMFLOAT2(0.0f,0.0f) },
//			{ DirectX::XMFLOAT3(0.3f, 0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
//			{ DirectX::XMFLOAT3(0.3f,-0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
//			{ DirectX::XMFLOAT3(0.3f, 0.3f,3.0f)	, DirectX::XMFLOAT2(1.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f,-0.3f,3.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f, 0.3f,3.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f, 0.3f,1.0f)	, DirectX::XMFLOAT2(0.0f,0.0f) },
//			{ DirectX::XMFLOAT3(-0.3f, 0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
//			{ DirectX::XMFLOAT3(0.3f, 0.3f, 3.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) },
//			{ DirectX::XMFLOAT3(-0.3f, 0.3f,1.0f)	, DirectX::XMFLOAT2(1.0f,1.0f) },
//			{ DirectX::XMFLOAT3(-0.3f, 0.3f, 3.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(0.3f, 0.3f, 3.0f)	, DirectX::XMFLOAT2(0.0f,1.0f) },
//			{ DirectX::XMFLOAT3(-0.3f, 0.3f, 3.0f)	, DirectX::XMFLOAT2(0.0f,0.0f) },
//			{ DirectX::XMFLOAT3(0.3f,-0.3f, 3.0f)	, DirectX::XMFLOAT2(1.0f,0.0f) }
	};


	// 缓存信息描述
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 6;
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
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	const UINT numElements = ARRAYSIZE(layout);
	hr = pDevice->CreateInputLayout(layout, numElements, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), &pInputLayout);
	if (FAILED(hr)) {
		MessageBox(NULL, "ERROR::CreateInputLayout", "Error", MB_OK);
		return hr;
	}
	pImmediateContext->IASetInputLayout(pInputLayout);

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		float color[] = { 0.1f , 0.2 , 0.5f , 1.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, color);
		pImmediateContext->VSSetShader(pVertexShader, nullptr, 0);
		pImmediateContext->PSSetShader(pPixelShader, nullptr, 0);
		pImmediateContext->Draw(6, 0);
		pSwapChain->Present(0, 0);
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

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
 * \brief 消息处理回调函数
 * \param hWnd 窗口句柄
 * \param uMsg 消息
 * \param wParam 消息附加参数
 * \param lParam 消息附加参数
 * \return 返回默认消息函数
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