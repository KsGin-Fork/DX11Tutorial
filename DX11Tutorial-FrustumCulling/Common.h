#pragma once

#include <D3D11.h>
#include <D3DX11tex.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <fstream>
#include <string>
#include <Pdh.h>
#include <minwinbase.h>
#include <Windows.h>
#include <D3DX10math.h>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "pdh.lib")

#define DIRECTINPUT8_VERSION 0x0800	// dxinput 版本

const int width = 1920, height = 1080;

using namespace std;
using namespace DirectX;

struct Vertex {
	XMFLOAT3 pos{};
	XMFLOAT2 tex{};
	Vertex() {}
	Vertex(XMFLOAT3&& p, XMFLOAT2&& t) {
		pos = p;
		tex = t;
	}
};

struct MatrixXD {
	XMMATRIX world;
	XMMATRIX view;
	XMMATRIX projection;
};

struct Font {
	float left, right;
	int size;
};

inline HRESULT CALLBACK WndProc(
	const HWND hWnd,
	const UINT uMsg,
	const WPARAM wParam,
	const LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY: PostQuitMessage(0); break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hWnd);
		}
		break;
	default:break;
	}
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

inline HRESULT InitEnableDepth(
	ID3D11Device* const pDevice,
	ID3D11DepthStencilState **pEnableDepthStencilState) {

	HRESULT hr;
	D3D11_DEPTH_STENCIL_DESC ddEnableStencilopDesc;
	ZeroMemory(&ddEnableStencilopDesc, sizeof(ddEnableStencilopDesc));
	ddEnableStencilopDesc.DepthEnable = true;
	ddEnableStencilopDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ddEnableStencilopDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ddEnableStencilopDesc.StencilEnable = true;
	ddEnableStencilopDesc.StencilReadMask = 0xFF;
	ddEnableStencilopDesc.StencilWriteMask = 0xFF;
	ddEnableStencilopDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddEnableStencilopDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	ddEnableStencilopDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddEnableStencilopDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ddEnableStencilopDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddEnableStencilopDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	ddEnableStencilopDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddEnableStencilopDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = pDevice->CreateDepthStencilState(&ddEnableStencilopDesc, pEnableDepthStencilState);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateDepthStencilState", "ERROR", MB_OK);
		return hr;
	}

	return S_OK;
}

inline HRESULT InitDisableDepth(
	ID3D11Device* const pDevice,
	ID3D11DepthStencilState **pDisableDepthStencilState) {

	HRESULT hr;
	D3D11_DEPTH_STENCIL_DESC ddDisableStencilopDesc;
	ZeroMemory(&ddDisableStencilopDesc, sizeof(ddDisableStencilopDesc));
	ddDisableStencilopDesc.DepthEnable = false;
	ddDisableStencilopDesc.DepthWriteMask = D3D11_DEPTH_WRITE_MASK_ALL;
	ddDisableStencilopDesc.DepthFunc = D3D11_COMPARISON_LESS;
	ddDisableStencilopDesc.StencilEnable = true;
	ddDisableStencilopDesc.StencilReadMask = 0xFF;
	ddDisableStencilopDesc.StencilWriteMask = 0xFF;
	ddDisableStencilopDesc.FrontFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddDisableStencilopDesc.FrontFace.StencilDepthFailOp = D3D11_STENCIL_OP_INCR;
	ddDisableStencilopDesc.FrontFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddDisableStencilopDesc.FrontFace.StencilFunc = D3D11_COMPARISON_ALWAYS;
	ddDisableStencilopDesc.BackFace.StencilFailOp = D3D11_STENCIL_OP_KEEP;
	ddDisableStencilopDesc.BackFace.StencilDepthFailOp = D3D11_STENCIL_OP_DECR;
	ddDisableStencilopDesc.BackFace.StencilPassOp = D3D11_STENCIL_OP_KEEP;
	ddDisableStencilopDesc.BackFace.StencilFunc = D3D11_COMPARISON_ALWAYS;

	hr = pDevice->CreateDepthStencilState(&ddDisableStencilopDesc, pDisableDepthStencilState);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateDepthStencilState", "ERROR", MB_OK);
		return hr;
	}

	return S_OK;
}

inline HRESULT SetDepthStencil(
	ID3D11Device* const pDevice,
	ID3D11DepthStencilView **pDepthStencilView) {
	HRESULT hr;

	D3D11_TEXTURE2D_DESC depthBufferDesc;
	ZeroMemory(&depthBufferDesc, sizeof(depthBufferDesc));
	depthBufferDesc.Width = width;
	depthBufferDesc.Height = height;
	depthBufferDesc.MipLevels = 1;
	depthBufferDesc.ArraySize = 1;
	depthBufferDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthBufferDesc.SampleDesc.Count = 1;
	depthBufferDesc.SampleDesc.Quality = 0;
	depthBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	depthBufferDesc.BindFlags = D3D11_BIND_DEPTH_STENCIL;
	depthBufferDesc.CPUAccessFlags = 0;
	depthBufferDesc.MiscFlags = 0;

	ID3D11Texture2D *pDepthBuffer;
	hr = pDevice->CreateTexture2D(&depthBufferDesc, nullptr, &pDepthBuffer);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateDepthTexture2D", "ERROR", MB_OK);
		return hr;
	}

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;

	hr = pDevice->CreateDepthStencilView(pDepthBuffer, &depthStencilViewDesc, pDepthStencilView);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateDepthStencilView", "ERROR", MB_OK);
		return hr;
	}
	return S_OK;
}

inline HRESULT InitWindowAndD3D(
	const HINSTANCE hInstance,
	const char * windowName,
	HWND * hWnd,
	IDXGISwapChain **pSwapChain,
	ID3D11RenderTargetView **pRenderTargetView,
	ID3D11Device **pDevice,
	ID3D11DeviceContext **pImmediateContext) {

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
	(*hWnd) = CreateWindow("mWndClass", windowName, WS_EX_TOPMOST | WS_OVERLAPPEDWINDOW, 0, 0, width, height, nullptr, nullptr, hInstance, nullptr);
	if (!hWnd) {
		MessageBox(nullptr, "ERROR::CreateWindow_Error", "Error", MB_OK);
		return -1;
	}

	ShowWindow(*hWnd, SW_NORMAL);
	UpdateWindow(*hWnd);

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
	dc.OutputWindow = *hWnd;
	dc.Windowed = true;

	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

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
		pSwapChain,
		pDevice,
		&eFeatureLevel,
		pImmediateContext);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateDeviceAndSwapChain_Error", "Error", MB_OK);
		return hr;
	}

	ID3D11Texture2D *pTexture2D = nullptr;
	hr = (*pSwapChain)->GetBuffer(0, __uuidof(pTexture2D), reinterpret_cast<LPVOID*>(&pTexture2D));
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::SetTextureBuffer_Error", "Error", MB_OK);
		return hr;
	}
	hr = (*pDevice)->CreateRenderTargetView(pTexture2D, nullptr, pRenderTargetView);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateRenderTargetView_Error", "Error", MB_OK);
		return hr;
	}
	(*pImmediateContext)->OMSetRenderTargets(1, pRenderTargetView, nullptr);
	D3D11_VIEWPORT dv{ 0 , 0 , width , height , 0 , 1 };
	(*pImmediateContext)->RSSetViewports(1, &dv);

	pTexture2D->Release();
	return S_OK;
}

inline HRESULT InitShader(
	const char * vertexShaderName,
	const char * pixelShaderName,
	const char * texFileName,
	ID3D11Device *pDevice,
	ID3D11VertexShader **pVertexShader,
	ID3D11PixelShader **pPixelShader,
	ID3D11InputLayout **pInputLayout,
	ID3D11SamplerState **pSamplerState,
	ID3D11ShaderResourceView **pShaderResourceView) {

	HRESULT hr;

	ID3D10Blob* pErrorMessage = nullptr;
	ID3D10Blob* pVertexShaderBlob = nullptr;
	ID3D10Blob* pPixelShaderBlob = nullptr;

	hr = D3DX11CompileFromFile(vertexShaderName, nullptr, nullptr, "main", "vs_5_0", D3DCOMPILER_STRIP_DEBUG_INFO, 0, nullptr, &pVertexShaderBlob, &pErrorMessage, nullptr);
	if (FAILED(hr)) {
		if (pErrorMessage) MessageBox(NULL, static_cast<CHAR*>(pErrorMessage->GetBufferPointer()), "Error", MB_OK);
		else MessageBox(NULL, "vertexShader File Not Found", "Error", MB_OK);
		return hr;
	}
	hr = D3DX11CompileFromFile(pixelShaderName, nullptr, nullptr, "main", "ps_5_0", D3DCOMPILER_STRIP_DEBUG_INFO, 0, nullptr, &pPixelShaderBlob, &pErrorMessage, nullptr);
	if (FAILED(hr)) {
		if (pErrorMessage) MessageBox(NULL, static_cast<CHAR*>(pErrorMessage->GetBufferPointer()), "Error", MB_OK);
		else MessageBox(NULL, "pixelShader File Not Found", "Error", MB_OK);
		return hr;
	}

	hr = pDevice->CreateVertexShader(pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), nullptr, pVertexShader);
	if (FAILED(hr)) {
		MessageBox(NULL, "ERROR::CreateVertexShader", "Error", MB_OK);
		return hr;
	}

	hr = pDevice->CreatePixelShader(pPixelShaderBlob->GetBufferPointer(), pPixelShaderBlob->GetBufferSize(), nullptr, pPixelShader);
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

	hr = pDevice->CreateSamplerState(&samplerDesc, pSamplerState);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateSampler", "Error", MB_OK);
		return hr;
	}

	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, texFileName, nullptr, nullptr, pShaderResourceView, nullptr);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateShaderResourceView", "Error", MB_OK);
		return hr;
	}



	D3D11_INPUT_ELEMENT_DESC layout[] = {
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 } ,
		{ "TEXCOORD", 0, DXGI_FORMAT_R32G32_FLOAT, 0, D3D11_APPEND_ALIGNED_ELEMENT, D3D11_INPUT_PER_VERTEX_DATA, 0 }
	};
	const UINT numElements = ARRAYSIZE(layout);
	hr = pDevice->CreateInputLayout(layout, numElements, pVertexShaderBlob->GetBufferPointer(), pVertexShaderBlob->GetBufferSize(), pInputLayout);
	if (FAILED(hr)) {
		MessageBox(NULL, "ERROR::CreateInputLayout", "Error", MB_OK);
		return hr;
	}

	pVertexShaderBlob->Release();
	pPixelShaderBlob->Release();

	if (pErrorMessage) {
		pErrorMessage->Release();
	}
	return S_OK;
}

inline HRESULT Init3DConstant(
	MatrixXD matrix3d,
	ID3D11Device *pDevice,
	ID3D11Buffer **pMatrixDBuffer3D) {

	HRESULT hr;
	D3D11_BUFFER_DESC dbDesc3D;
	ZeroMemory(&dbDesc3D, sizeof(dbDesc3D));
	dbDesc3D.Usage = D3D11_USAGE_DYNAMIC;
	dbDesc3D.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbDesc3D.ByteWidth = sizeof(matrix3d);
	dbDesc3D.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	D3D11_SUBRESOURCE_DATA dsData3D;
	ZeroMemory(&dsData3D, sizeof(dsData3D));
	dsData3D.pSysMem = &matrix3d;

	hr = pDevice->CreateBuffer(&dbDesc3D, &dsData3D, pMatrixDBuffer3D);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateMatrixBuffer", "ERROR", MB_OK);
		return hr;
	}
	return S_OK;
}

inline HRESULT InitCubeVertexAndIndexBuffer(
	ID3D11Device *pDevice,
	ID3D11Buffer **pVertexBufferObject,
	ID3D11Buffer **pIndexBufferObject,
	Vertex *&vertices,
	UINT *&indices) {

	HRESULT hr;

	if (vertices) {
		delete[] vertices;
		vertices = nullptr;
	}
	if (indices) {
		delete[] indices;
		indices = nullptr;
	}

	vertices = new Vertex[24]{
		Vertex(XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(0, 1)),
		Vertex(XMFLOAT3(-0.5f, +0.5f, -0.5f), XMFLOAT2(0, 0)),
		Vertex(XMFLOAT3(+0.5f, +0.5f, -0.5f), XMFLOAT2(1, 0)),
		Vertex(XMFLOAT3(+0.5f, -0.5f, -0.5f), XMFLOAT2(1, 1)),
		Vertex(XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT2(1, 1)),
		Vertex(XMFLOAT3(-0.5f, +0.5f, +0.5f), XMFLOAT2(1, 0)),
		Vertex(XMFLOAT3(+0.5f, +0.5f, +0.5f), XMFLOAT2(0, 0)),
		Vertex(XMFLOAT3(+0.5f, -0.5f, +0.5f), XMFLOAT2(0, 1)),
		Vertex(XMFLOAT3(-0.5f, +0.5f, -0.5f), XMFLOAT2(0, 1)),
		Vertex(XMFLOAT3(-0.5f, +0.5f, +0.5f), XMFLOAT2(0, 0)),
		Vertex(XMFLOAT3(+0.5f, +0.5f, +0.5f), XMFLOAT2(1, 0)),
		Vertex(XMFLOAT3(+0.5f, +0.5f, -0.5f), XMFLOAT2(1, 1)),
		Vertex(XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(1, 1)),
		Vertex(XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT2(1, 0)),
		Vertex(XMFLOAT3(+0.5f, -0.5f, +0.5f), XMFLOAT2(0, 0)),
		Vertex(XMFLOAT3(+0.5f, -0.5f, -0.5f), XMFLOAT2(0, 1)),
		Vertex(XMFLOAT3(-0.5f, -0.5f, +0.5f), XMFLOAT2(0, 1)),
		Vertex(XMFLOAT3(-0.5f, +0.5f, +0.5f), XMFLOAT2(0, 0)),
		Vertex(XMFLOAT3(-0.5f, +0.5f, -0.5f), XMFLOAT2(1, 0)),
		Vertex(XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT2(1, 1)),
		Vertex(XMFLOAT3(+0.5f, -0.5f, -0.5f), XMFLOAT2(0, 1)),
		Vertex(XMFLOAT3(+0.5f, +0.5f, -0.5f), XMFLOAT2(0, 0)),
		Vertex(XMFLOAT3(+0.5f, +0.5f, +0.5f), XMFLOAT2(1, 0)),
		Vertex(XMFLOAT3(+0.5f, -0.5f, +0.5f), XMFLOAT2(1, 1))
	};

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 24;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA verticesSourceData;
	ZeroMemory(&verticesSourceData, sizeof(D3D11_SUBRESOURCE_DATA));

	verticesSourceData.pSysMem = vertices;
	pDevice->CreateBuffer(&vertexBufferDesc, &verticesSourceData, pVertexBufferObject);

	indices = new UINT[36]{
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

	hr = pDevice->CreateBuffer(&indexDesc, &indexData, pIndexBufferObject);
	if (FAILED(hr))
	{
		return hr;
	}
	return S_OK;
}


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
inline HRESULT InitLetter(Font **fonts) {
	ifstream ifs("./fontdata.txt");
	if (!ifs.is_open()) return false;
	int nNumFonts = 0;
	ifs >> nNumFonts;

	*fonts = new Font[nNumFonts];
	if (!(fonts)) return false;

	for (int i = 0; i < nNumFonts; ++i) {
		int ascii;
		char letter;
		ifs >> ascii >> letter >> (*fonts)[i].left >> (*fonts)[i].right >> (*fonts)[i].size;
	}

	ifs.close();
	return S_OK;
}

inline int GetFPS(
	unsigned long &startTime,
	int &fps,
	int &tc) {
	const auto time = timeGetTime();
	if (time >= (startTime + 1000)) {
		fps = tc;
		tc = 0;
		startTime = timeGetTime();
	}
	else {
		++tc;
	}
	return fps;
}

inline int GetCPUUsage(
	const bool canSamleUsage,
	int &usage,
	unsigned long &lastTime,
	const HQUERY hQuery,
	const HCOUNTER hCounter) {
	const auto time = timeGetTime();
	if (canSamleUsage && time >= (lastTime + 1000)) {
		PDH_FMT_COUNTERVALUE value;
		PdhCollectQueryData(hQuery);
		PdhGetFormattedCounterValue(hCounter, PDH_FMT_LONG, nullptr, &value);
		usage = value.longValue;
		lastTime = timeGetTime();
	}
	return usage;
}

inline void InitQueryAndCounter(
	HQUERY *hQuery,
	HCOUNTER *hCounter,
	bool *canSample) {
	PDH_STATUS status;
	status = PdhOpenQuery(nullptr, 0, hQuery);
	if (status != ERROR_SUCCESS) {
		MessageBox(nullptr, "ERROR::OpenQuery", "ERROR", MB_OK);
		*canSample = false;
	}
	status = PdhAddCounter(*hQuery, TEXT("\\Processor(_Total)\\% processor time"), 0, hCounter);
	if (status != ERROR_SUCCESS) {
		MessageBox(nullptr, "ERROR::AddCounter", "ERROR", MB_OK);
		canSample = false;
	}
}

inline HRESULT UpdateSentenceVertexAndIndexBuffer(
	const char * sentence,
	const float drawX,
	const float drawY,
	Font * fonts,
	ID3D11Buffer **pVertexBufferObject,
	ID3D11Buffer **pIndexBufferObject,
	ID3D11DeviceContext **pImmediateContext,
	UINT &nNumVertex,
	Vertex *&vertices,
	UINT *&indices) {
	if (vertices) {
		delete[] vertices;
		vertices = nullptr;
	}

	if (indices) {
		delete[] indices;
		indices = nullptr;
	}

	HRESULT hr;

	nNumVertex = strlen(sentence) * 6;
	vertices = new Vertex[nNumVertex];

	int idx = 0;
	float posX = drawX;
	const float posY = drawY;
	for (auto i = 0; i < nNumVertex / 6; ++i) {
		const Font letter = fonts[static_cast<int>(sentence[i]) - 32];
		if (sentence[i] - 32 == 0) {
			posX += 3;
		}
		else {
			vertices[idx].pos = XMFLOAT3(posX, posY, 0.0f);	// 左上
			vertices[idx++].tex = XMFLOAT2(letter.left, 0.0f);
			vertices[idx].pos = XMFLOAT3(posX + letter.size, posY - 16, 0.0f);	// 右下
			vertices[idx++].tex = XMFLOAT2(letter.right, 1.0f);
			vertices[idx].pos = XMFLOAT3(posX, posY - 16, 0.0f);	// 左下
			vertices[idx++].tex = XMFLOAT2(letter.left, 1.0f);
			vertices[idx].pos = XMFLOAT3(posX, posY, 0.0f);	// 左上
			vertices[idx++].tex = XMFLOAT2(letter.left, 0.0f);
			vertices[idx].pos = XMFLOAT3(posX + letter.size, posY, 0.0f);	// 右上
			vertices[idx++].tex = XMFLOAT2(letter.right, 0.0f);
			vertices[idx].pos = XMFLOAT3(posX + letter.size, posY - 16, 0.0f);	// 右下
			vertices[idx++].tex = XMFLOAT2(letter.right, 1.0f);
			posX += letter.size + 1.0f;
		}
	}

	indices = new UINT[nNumVertex];
	for (auto i = 0; i < nNumVertex; ++i) indices[i] = i;

	D3D11_MAPPED_SUBRESOURCE dms;
	hr = (*pImmediateContext)->Map(*pVertexBufferObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &dms);
	if (FAILED(hr)) {
		return hr;
	}
	auto *vertexPtr = static_cast<Vertex*>(dms.pData);
	memcpy(vertexPtr, vertices, sizeof(Vertex) * nNumVertex);
	(*pImmediateContext)->Unmap(*pVertexBufferObject, 0);
	hr = (*pImmediateContext)->Map(*pIndexBufferObject, 0, D3D11_MAP_WRITE_DISCARD, 0, &dms);
	if (FAILED(hr)) {
		return hr;
	}
	auto *indexPtr = static_cast<UINT*>(dms.pData);
	memcpy(indexPtr, indices, sizeof(UINT) * nNumVertex);
	(*pImmediateContext)->Unmap(*pIndexBufferObject, 0);

	return S_OK;
}

inline HRESULT InitSentenceVertexAndIndexBuffer(
	const char * sentence,
	const float drawX,
	const float drawY,
	Font * fonts,
	ID3D11Device * const pDevice,
	ID3D11Buffer **pVertexBufferObject,
	ID3D11Buffer **pIndexBufferObject,
	UINT &nNumVertex,
	Vertex *&vertices,
	UINT *&indices) {

	if (vertices) {
		delete[] vertices;
		vertices = nullptr;
	}

	if (indices) {
		delete[] indices;
		indices = nullptr;
	}

	HRESULT hr;

	nNumVertex = strlen(sentence) * 6;
	vertices = new Vertex[nNumVertex];

	int idx = 0;
	float posX = drawX;
	const float posY = drawY;
	for (auto i = 0; i < nNumVertex / 6; ++i) {
		const Font letter = fonts[static_cast<int>(sentence[i]) - 32];
		if (sentence[i] - 32 == 0) {
			posX += 3;
		}
		else {
			vertices[idx].pos = XMFLOAT3(posX, posY, 0.0f);	// 左上
			vertices[idx++].tex = XMFLOAT2(letter.left, 0.0f);
			vertices[idx].pos = XMFLOAT3(posX + letter.size, posY - 16, 0.0f);	// 右下
			vertices[idx++].tex = XMFLOAT2(letter.right, 1.0f);
			vertices[idx].pos = XMFLOAT3(posX, posY - 16, 0.0f);	// 左下
			vertices[idx++].tex = XMFLOAT2(letter.left, 1.0f);
			vertices[idx].pos = XMFLOAT3(posX, posY, 0.0f);	// 左上
			vertices[idx++].tex = XMFLOAT2(letter.left, 0.0f);
			vertices[idx].pos = XMFLOAT3(posX + letter.size, posY, 0.0f);	// 右上
			vertices[idx++].tex = XMFLOAT2(letter.right, 0.0f);
			vertices[idx].pos = XMFLOAT3(posX + letter.size, posY - 16, 0.0f);	// 右下
			vertices[idx++].tex = XMFLOAT2(letter.right, 1.0f);
			posX += letter.size + 1.0f;
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * nNumVertex;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

	D3D11_SUBRESOURCE_DATA verticesSourceData;
	ZeroMemory(&verticesSourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	verticesSourceData.pSysMem = vertices;

	hr = pDevice->CreateBuffer(&vertexBufferDesc, &verticesSourceData, pVertexBufferObject);
	if (FAILED(hr))
	{
		return hr;
	}

	indices = new UINT[nNumVertex];
	for (auto i = 0; i < nNumVertex; ++i) indices[i] = i;

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(indexDesc));
	indexDesc.Usage = D3D11_USAGE_IMMUTABLE;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(UINT) * nNumVertex;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(indexData));
	indexData.pSysMem = indices;

	hr = pDevice->CreateBuffer(&indexDesc, &indexData, pIndexBufferObject);
	if (FAILED(hr))
	{
		return hr;
	}
	return S_OK;
}


inline HRESULT Init2DConstant(
	ID3D11Device *pDevice,
	ID3D11Buffer **pMatrixDBuffer2D) {
	HRESULT hr;
	MatrixXD matrix2d = {
	XMMatrixIdentity() ,
	XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f , -100.0f , 0.0f),
		XMVectorSet(0.0f, 0.0f ,  0.0f , 0.0f),
		XMVectorSet(0.0f, 1.0f ,  0.0f , 0.0f)
	),
	XMMatrixOrthographicLH(static_cast<float>(width) , static_cast<float>(height) , 0.01f , 100.0f)
	};
	D3D11_BUFFER_DESC dbDesc2D;
	ZeroMemory(&dbDesc2D, sizeof(dbDesc2D));
	dbDesc2D.Usage = D3D11_USAGE_DEFAULT;
	dbDesc2D.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	dbDesc2D.ByteWidth = sizeof(matrix2d);
	D3D11_SUBRESOURCE_DATA dsData2D;
	ZeroMemory(&dsData2D, sizeof(dsData2D));
	dsData2D.pSysMem = &matrix2d;

	hr = pDevice->CreateBuffer(&dbDesc2D, &dsData2D, pMatrixDBuffer2D);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateMatrixBuffer", "ERROR", MB_OK);
		return hr;
	}
	return S_OK;
}


inline HRESULT DrawModelIndex(
	const UINT drawIndexCount,
	ID3D11Buffer *pVertexBufferObject,
	ID3D11Buffer *pIndexBufferObject,
	ID3D11Buffer *pMatrixDBuffer3D,
	ID3D11SamplerState *pSamplerState,
	ID3D11ShaderResourceView *pShaderResourceView,
	ID3D11VertexShader *pVertexShader,
	ID3D11PixelShader *pPixelShader,
	ID3D11InputLayout *pInputLayout,
	ID3D11DepthStencilView *pDepthStencilView,
	ID3D11DepthStencilState *pDepthStencilState,
	ID3D11RenderTargetView **pRenderTargetView,
	ID3D11DeviceContext **pImmediateContext) {

	UINT stride = sizeof(Vertex);
	UINT offset = 0;

	(*pImmediateContext)->OMSetDepthStencilState(pDepthStencilState, 1);
	(*pImmediateContext)->OMSetRenderTargets(1, pRenderTargetView, pDepthStencilView);

	(*pImmediateContext)->IASetVertexBuffers(0, 1, &pVertexBufferObject, &stride, &offset);
	(*pImmediateContext)->IASetIndexBuffer(pIndexBufferObject, DXGI_FORMAT_R32_UINT, 0);
	(*pImmediateContext)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	(*pImmediateContext)->VSSetConstantBuffers(0, 1, &pMatrixDBuffer3D);

	(*pImmediateContext)->VSSetShader(pVertexShader, 0, 0);
	(*pImmediateContext)->PSSetShader(pPixelShader, 0, 0);
	(*pImmediateContext)->IASetInputLayout(pInputLayout);
	(*pImmediateContext)->PSSetShaderResources(0, 1, &pShaderResourceView);
	(*pImmediateContext)->PSSetSamplers(0, 1, &pSamplerState);
	(*pImmediateContext)->DrawIndexed(drawIndexCount, 0, 0);
	return S_OK;
}


inline HRESULT Update3DModelPos(MatrixXD matrix , ID3D11Buffer *pMatrixDBuffer3D , ID3D11DeviceContext **pImmediateContext) {
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE dms;
	hr = (*pImmediateContext)->Map(pMatrixDBuffer3D, 0, D3D11_MAP_WRITE_DISCARD, 0, &dms);
	if (FAILED(hr)) {
		return hr;
	}
	auto* matrix3D = static_cast<MatrixXD*>(dms.pData);	
	memcpy(matrix3D, &matrix, sizeof(matrix));
	(*pImmediateContext)->Unmap(pMatrixDBuffer3D, 0);
	return S_OK;
}