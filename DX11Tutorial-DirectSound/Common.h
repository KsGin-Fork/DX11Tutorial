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
#include <string.h>

#pragma comment(lib, "dsound.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "pdh.lib")

#define DIRECTINPUT8_VERSION 0x0800	// dxinput 版本

const int width = 1920, height = 1080;

using namespace std;
using namespace DirectX;

struct Vertex {
	XMFLOAT3 pos;
	XMFLOAT2 tex;
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

struct WaveHeader
{
	char chunkId[4];
	unsigned long chunkSize;
	char format[4];
	char subChunkId[4];
	unsigned long subChunkSize;
	unsigned short audioFormat;
	unsigned short numChannels;
	unsigned long sampleRate;
	unsigned long bytesPerSecond;
	unsigned short blockAlign;
	unsigned short bitsPerSample;
	char dataChunkId[4];
	unsigned long dataSize;
};


inline HRESULT CALLBACK WndProc(
	const HWND hWnd,
	const UINT uMsg,
	const WPARAM wParam,
	const LPARAM lParam) {
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

inline HRESULT SetDepthStencil(
	ID3D11Device* const pDevice,
	ID3D11DeviceContext ** pImmediateContext,
	ID3D11RenderTargetView ** pRenderTargetView) {
	HRESULT hr;
	///////////////////////////////////////////////////////////////////// 创建深度纹理贴图 /////////////////////////////////////////////////////////////////
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

	//////////////////////////////////////////////////////////////////////// 创建深度模板 ///////////////////////////////////////////////////////////////////
	D3D11_DEPTH_STENCIL_DESC ddDisableStencilopDesc;
	ZeroMemory(&ddDisableStencilopDesc, sizeof(ddDisableStencilopDesc));
	ddDisableStencilopDesc.DepthEnable = false;	//////////////////////////////// 2D中禁用深度 //////////////////////////////////////////////////////////////////////
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

	ID3D11DepthStencilState *pDisableDepthStencilState = nullptr;
	hr = pDevice->CreateDepthStencilState(&ddDisableStencilopDesc, &pDisableDepthStencilState);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateDepthStencilState", "ERROR", MB_OK);
		return hr;
	}
	(*pImmediateContext)->OMSetDepthStencilState(pDisableDepthStencilState, 1);

	D3D11_DEPTH_STENCIL_VIEW_DESC depthStencilViewDesc;
	ZeroMemory(&depthStencilViewDesc, sizeof(depthStencilViewDesc));
	depthStencilViewDesc.Format = DXGI_FORMAT_D24_UNORM_S8_UINT;
	depthStencilViewDesc.ViewDimension = D3D11_DSV_DIMENSION_TEXTURE2D;
	depthStencilViewDesc.Texture2D.MipSlice = 0;
	ID3D11DepthStencilView *pDepthStencilView = nullptr;
	hr = pDevice->CreateDepthStencilView(pDepthBuffer, &depthStencilViewDesc, &pDepthStencilView);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateDepthStencilView", "ERROR", MB_OK);
		return hr;
	}
	(*pImmediateContext)->OMSetRenderTargets(1, pRenderTargetView, pDepthStencilView);

	D3D11_RASTERIZER_DESC rasterizerDesc;
	rasterizerDesc.AntialiasedLineEnable = false;
	rasterizerDesc.CullMode = D3D11_CULL_BACK;
	rasterizerDesc.DepthBias = 0;
	rasterizerDesc.DepthBiasClamp = 0.0f;
	rasterizerDesc.DepthClipEnable = true;
	rasterizerDesc.FillMode = D3D11_FILL_SOLID;
	rasterizerDesc.FrontCounterClockwise = false;
	rasterizerDesc.MultisampleEnable = false;
	rasterizerDesc.ScissorEnable = false;
	rasterizerDesc.SlopeScaledDepthBias = 0.0f;

	ID3D11RasterizerState *pRasterizerState = nullptr;
	hr = pDevice->CreateRasterizerState(&rasterizerDesc, &pRasterizerState);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateRasterizerState", "ERROR", MB_OK);
		return hr;
	}
	(*pImmediateContext)->RSSetState(pRasterizerState);

	pDisableDepthStencilState->Release();
	pDepthStencilView->Release();
	pRasterizerState->Release();

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
}

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
}

inline HRESULT InitSentenceVertex(
	ID3D11Device * const pDevice,
	const char * sentence,
	const float drawX,
	const float drawY,
	Font * fonts,
	ID3D11DeviceContext **pImmediateContext,
	int* nNumVertex,
	Vertex **vertices) {

	if (*vertices) {
		delete[] * vertices;
		*vertices = nullptr;
	}

	*nNumVertex = strlen(sentence);
	*vertices = new Vertex[*nNumVertex * 6];

	int idx = 0;
	float posX = drawX;
	const float posY = drawY;
	for (auto i = 0; i < *nNumVertex; ++i) {
		const Font letter = fonts[static_cast<int>(sentence[i]) - 32];
		if (sentence[i] - 32 == 0) {
			posX += 3;
		}
		else {
			(*vertices)[idx].pos = XMFLOAT3(posX, posY, 0.0f);	// 左上
			(*vertices)[idx++].tex = XMFLOAT2(letter.left, 0.0f);
			(*vertices)[idx].pos = XMFLOAT3(posX + letter.size, posY - 16, 0.0f);	// 右下
			(*vertices)[idx++].tex = XMFLOAT2(letter.right, 1.0f);
			(*vertices)[idx].pos = XMFLOAT3(posX, posY - 16, 0.0f);	// 左下
			(*vertices)[idx++].tex = XMFLOAT2(letter.left, 1.0f);
			(*vertices)[idx].pos = XMFLOAT3(posX, posY, 0.0f);	// 左上
			(*vertices)[idx++].tex = XMFLOAT2(letter.left, 0.0f);
			(*vertices)[idx].pos = XMFLOAT3(posX + letter.size, posY, 0.0f);	// 右上
			(*vertices)[idx++].tex = XMFLOAT2(letter.right, 0.0f);
			(*vertices)[idx].pos = XMFLOAT3(posX + letter.size, posY - 16, 0.0f);	// 右下
			(*vertices)[idx++].tex = XMFLOAT2(letter.right, 1.0f);
			posX += letter.size + 1.0f;
		}
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * (*nNumVertex) * 6;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA verticesSourceData;
	ZeroMemory(&verticesSourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	verticesSourceData.pSysMem = *vertices;

	ID3D11Buffer *pVertexBufferObject = nullptr;

	pDevice->CreateBuffer(&vertexBufferDesc, &verticesSourceData, &pVertexBufferObject);

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	(*pImmediateContext)->IASetVertexBuffers(0, 1, &pVertexBufferObject, &stride, &offset);
	(*pImmediateContext)->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	return S_OK;
}

inline HRESULT Init2DConstant(
	ID3D11Device *pDevice,
	ID3D11DeviceContext **pImmediateContext) {
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

	ID3D11Buffer *pMatrixDBuffer2D = nullptr;
	hr = pDevice->CreateBuffer(&dbDesc2D, &dsData2D, &pMatrixDBuffer2D);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateMatrixBuffer", "ERROR", MB_OK);
		return hr;
	}
	(*pImmediateContext)->VSSetConstantBuffers(0, 1, &pMatrixDBuffer2D);
}

inline HRESULT InitShader(
	ID3D11Device *pDevice,
	ID3D11DeviceContext **pImmediateContext,
	ID3D11VertexShader **pVertexShader,
	ID3D11PixelShader **pPixelShader,
	ID3D11InputLayout **pInputLayout) {

	HRESULT hr;

	ID3D10Blob* pErrorMessage = nullptr;
	ID3D10Blob* pVertexShaderBlob = nullptr;
	ID3D10Blob* pPixelShaderBlob = nullptr;

	hr = D3DX11CompileFromFile("./vertexShader.hlsl", nullptr, nullptr, "main", "vs_5_0", D3DCOMPILER_STRIP_DEBUG_INFO, 0, nullptr, &pVertexShaderBlob, &pErrorMessage, nullptr);
	if (FAILED(hr)) {
		if (pErrorMessage) MessageBox(NULL, static_cast<CHAR*>(pErrorMessage->GetBufferPointer()), "Error", MB_OK);
		else MessageBox(NULL, "vertexShader File Not Found", "Error", MB_OK);
		return hr;
	}
	hr = D3DX11CompileFromFile("./pixelShader.hlsl", nullptr, nullptr, "main", "ps_5_0", D3DCOMPILER_STRIP_DEBUG_INFO, 0, nullptr, &pPixelShaderBlob, &pErrorMessage, nullptr);
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

	ID3D11SamplerState *pSamplerState = nullptr;
	ID3D11ShaderResourceView *pShaderResourceView;
	hr = pDevice->CreateSamplerState(&samplerDesc, &pSamplerState);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateSampler", "Error", MB_OK);
		return hr;
	}

	hr = D3DX11CreateShaderResourceViewFromFile(pDevice, "./font.dds", nullptr, nullptr, &pShaderResourceView, nullptr);
	if (FAILED(hr)) {
		MessageBox(nullptr, "ERROR::CreateShaderResourceView", "Error", MB_OK);
		return hr;
	}

	(*pImmediateContext)->PSSetShaderResources(0, 1, &pShaderResourceView);
	(*pImmediateContext)->PSSetSamplers(0, 1, &pSamplerState);

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
}


inline HRESULT DrawText(
	const string& text,
	const int drawX,
	const int drawY,
	Font *fonts,
	ID3D11VertexShader *pVertexShader,
	ID3D11PixelShader *pPixelShader,
	ID3D11InputLayout *pInputLayout,
	ID3D11Device *pDevice,
	Vertex **vertices,
	ID3D11DeviceContext **pImmediateContext) {

	int nNumVertices = 0;
	InitSentenceVertex(pDevice, text.c_str(), drawX, drawY, fonts, pImmediateContext, &nNumVertices, vertices);
	(*pImmediateContext)->VSSetShader(pVertexShader, 0, 0);
	(*pImmediateContext)->PSSetShader(pPixelShader, 0, 0);
	(*pImmediateContext)->IASetInputLayout(pInputLayout);
	(*pImmediateContext)->Draw(nNumVertices * 6, 0);

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