#pragma once

#include <D3D11.h>
#include <D3DX11tex.h>
#include <DirectXMath.h>
#include <fstream>
#include <string>
#include <Pdh.h>
#include <minwinbase.h>
#include <Windows.h>
#include <D3DX10math.h>
#include <vector>

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
	FLOAT p;
	XMFLOAT3 nor;
	XMFLOAT3 tgt;
	XMFLOAT3 bit;
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

	pTexture2D->Release();
	return S_OK;
}

inline HRESULT InitShader(
	const char * vertexShaderName,
	const char * pixelShaderName,
	const char * texFileName[],
	const int nNumTex,
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

	hr = D3DX11CompileFromFile(vertexShaderName, nullptr, nullptr, "main", "vs_5_0", D3D10_SHADER_DEBUG, 0, nullptr, &pVertexShaderBlob, &pErrorMessage, nullptr);
	if (FAILED(hr)) {
		if (pErrorMessage) MessageBox(NULL, static_cast<CHAR*>(pErrorMessage->GetBufferPointer()), "Error", MB_OK);
		else MessageBox(NULL, "vertexShader File Not Found", "Error", MB_OK);
		return hr;
	}
	hr = D3DX11CompileFromFile(pixelShaderName, nullptr, nullptr, "main", "ps_5_0", D3D10_SHADER_DEBUG, 0, nullptr, &pPixelShaderBlob, &pErrorMessage, nullptr);
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

	for (int i = 0; i < nNumTex; ++i) {
		hr = D3DX11CreateShaderResourceViewFromFile(pDevice, texFileName[i], nullptr, nullptr, &pShaderResourceView[i], nullptr);
		if (FAILED(hr)) {
			MessageBox(nullptr, "ERROR::CreateShaderResourceView", "Error", MB_OK);
			return hr;
		}
	}

	D3D11_INPUT_ELEMENT_DESC layout[5];
	ZeroMemory(&layout[0], sizeof(layout[0]));
	layout[0].SemanticName = "POSITION";
	layout[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	ZeroMemory(&layout[1], sizeof(layout[1]));
	layout[1].SemanticName = "TEXCOORD";
	layout[1].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[1].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	ZeroMemory(&layout[2], sizeof(layout[2]));
	layout[2].SemanticName = "NORMAL";
	layout[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[2].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	ZeroMemory(&layout[3], sizeof(layout[3]));
	layout[3].SemanticName = "TANGENT";
	layout[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[3].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
	ZeroMemory(&layout[4], sizeof(layout[4]));
	layout[4].SemanticName = "BITANGENT";
	layout[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	layout[4].AlignedByteOffset = D3D11_APPEND_ALIGNED_ELEMENT;
	layout[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;
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

inline HRESULT CalculateTangentAndBiTangent(Vertex *&vertices, UINT *&indices, const UINT nNumIndices) {
	const UINT faceCount = nNumIndices / 3;
	FLOAT edge1[3], edge2[3];
	FLOAT tuVector[2], tvVector[2];
	XMFLOAT3 tangent, bitangent, normal;
	FLOAT f, l;
	for (UINT i = 0; i < faceCount; ++i) {
		Vertex& v1 = vertices[indices[3 * i]];
		Vertex& v2 = vertices[indices[3 * i + 1]];
		Vertex& v3 = vertices[indices[3 * i + 2]];
		/////////////////////////////////////////////// Edge
		edge1[0] = v2.pos.x - v1.pos.x;
		edge1[1] = v2.pos.y - v1.pos.y;
		edge1[2] = v2.pos.z - v1.pos.z;
		edge2[0] = v3.pos.x - v1.pos.x;
		edge2[1] = v3.pos.y - v1.pos.y;
		edge2[2] = v3.pos.z - v1.pos.z;

		/////////////////////////////////////////////// UV
		tuVector[0] = v2.tex.x - v1.tex.x;
		tvVector[0] = v2.tex.y - v1.tex.y;
		tuVector[1] = v3.tex.x - v1.tex.x;
		tvVector[1] = v3.tex.y - v1.tex.y;

		/////////////////////////////////////////////// f
		f = 1.0f / (tuVector[0] * tvVector[1] - tuVector[1] * tvVector[0]);

		tangent.x = (tvVector[1] * edge1[0] - tvVector[0] * edge2[0]) * f;
		tangent.y = (tvVector[1] * edge1[1] - tvVector[0] * edge2[1]) * f;
		tangent.z = (tvVector[1] * edge1[2] - tvVector[0] * edge2[2]) * f;
		l = sqrt(tangent.x * tangent.x + tangent.y * tangent.y + tangent.z * tangent.z);
		tangent.x /= l;
		tangent.y /= l;
		tangent.z /= l;
		v1.tgt = v2.tgt = v3.tgt = XMFLOAT3(tangent.x, tangent.y, tangent.z);

		bitangent.x = (tuVector[0] * edge2[0] - tuVector[1] * edge1[0]) * f;
		bitangent.y = (tuVector[0] * edge2[1] - tuVector[1] * edge1[1]) * f;
		bitangent.z = (tuVector[0] * edge2[2] - tuVector[1] * edge1[2]) * f;
		l = sqrt(bitangent.x * bitangent.x + bitangent.y * bitangent.y + bitangent.z * bitangent.z);
		bitangent.x /= l;
		bitangent.y /= l;
		bitangent.z /= l;
		v1.bit = v2.bit = v3.bit = XMFLOAT3(bitangent.x, bitangent.y, bitangent.z);
		//////////////////////////////////////////////////

		normal.x = tangent.y * bitangent.z - tangent.z * bitangent.y;
		normal.y = tangent.z * bitangent.x - tangent.x * bitangent.z;
		normal.z = tangent.x * bitangent.y - tangent.y * bitangent.x;
		l = sqrt(normal.x * normal.x + normal.y * normal.y + normal.z * normal.z);
		normal.x /= l;
		normal.y /= l;
		normal.z /= l;
		v1.nor = v2.nor = v3.nor = XMFLOAT3(normal.x, normal.y, normal.z);
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

	vertices = new Vertex[36];
	//font
	vertices[0 ].pos = XMFLOAT3(-0.5f, -0.5f, -0.5f); vertices[0 ].tex = XMFLOAT2(0, 1);
	vertices[1 ].pos = XMFLOAT3(-0.5f, +0.5f, -0.5f); vertices[1 ].tex = XMFLOAT2(0, 0);
	vertices[2 ].pos = XMFLOAT3(+0.5f, +0.5f, -0.5f); vertices[2 ].tex = XMFLOAT2(1, 0);
	vertices[3 ].pos = XMFLOAT3(-0.5f, -0.5f, -0.5f); vertices[3 ].tex = XMFLOAT2(0, 1);
	vertices[4 ].pos = XMFLOAT3(+0.5f, +0.5f, -0.5f); vertices[4 ].tex = XMFLOAT2(1, 0);
	vertices[5 ].pos = XMFLOAT3(+0.5f, -0.5f, -0.5f); vertices[5 ].tex = XMFLOAT2(1, 1);
	//back	   												     
	vertices[6 ].pos = XMFLOAT3(-0.5f, -0.5f, +0.5f); vertices[6 ].tex = XMFLOAT2(1, 1);
	vertices[7 ].pos = XMFLOAT3(+0.5f, +0.5f, +0.5f); vertices[7 ].tex = XMFLOAT2(0, 0);
	vertices[8 ].pos = XMFLOAT3(-0.5f, +0.5f, +0.5f); vertices[8 ].tex = XMFLOAT2(1, 0);
	vertices[9 ].pos = XMFLOAT3(-0.5f, -0.5f, +0.5f); vertices[9 ].tex = XMFLOAT2(1, 1);
	vertices[10].pos = XMFLOAT3(+0.5f, -0.5f, +0.5f); vertices[10].tex = XMFLOAT2(0, 1);
	vertices[11].pos = XMFLOAT3(+0.5f, +0.5f, +0.5f); vertices[11].tex = XMFLOAT2(0, 0);
	//top
	vertices[12].pos = XMFLOAT3(-0.5f, +0.5f, -0.5f); vertices[12].tex = XMFLOAT2(0, 1);
	vertices[13].pos = XMFLOAT3(-0.5f, +0.5f, +0.5f); vertices[13].tex = XMFLOAT2(0, 0);
	vertices[14].pos = XMFLOAT3(+0.5f, +0.5f, +0.5f); vertices[14].tex = XMFLOAT2(1, 0);
	vertices[15].pos = XMFLOAT3(-0.5f, +0.5f, -0.5f); vertices[15].tex = XMFLOAT2(0, 1);
	vertices[16].pos = XMFLOAT3(+0.5f, +0.5f, +0.5f); vertices[16].tex = XMFLOAT2(1, 0);
	vertices[17].pos = XMFLOAT3(+0.5f, +0.5f, -0.5f); vertices[17].tex = XMFLOAT2(1, 1);
	//bottom
	vertices[18].pos = XMFLOAT3(-0.5f, -0.5f, -0.5f); vertices[18].tex = XMFLOAT2(1, 1);
	vertices[19].pos = XMFLOAT3(+0.5f, -0.5f, +0.5f); vertices[19].tex = XMFLOAT2(0, 0);
	vertices[20].pos = XMFLOAT3(-0.5f, -0.5f, +0.5f); vertices[20].tex = XMFLOAT2(1, 0);
	vertices[21].pos = XMFLOAT3(-0.5f, -0.5f, -0.5f); vertices[21].tex = XMFLOAT2(1, 1);
	vertices[22].pos = XMFLOAT3(+0.5f, -0.5f, -0.5f); vertices[22].tex = XMFLOAT2(0, 1);
	vertices[23].pos = XMFLOAT3(+0.5f, -0.5f, +0.5f); vertices[23].tex = XMFLOAT2(0, 0);
	//left
	vertices[24].pos = XMFLOAT3(-0.5f, -0.5f, +0.5f); vertices[24].tex = XMFLOAT2(0, 1);
	vertices[25].pos = XMFLOAT3(-0.5f, +0.5f, +0.5f); vertices[25].tex = XMFLOAT2(0, 0);
	vertices[26].pos = XMFLOAT3(-0.5f, +0.5f, -0.5f); vertices[26].tex = XMFLOAT2(1, 0);
	vertices[27].pos = XMFLOAT3(-0.5f, -0.5f, +0.5f); vertices[27].tex = XMFLOAT2(0, 1);
	vertices[28].pos = XMFLOAT3(-0.5f, +0.5f, -0.5f); vertices[28].tex = XMFLOAT2(1, 0);
	vertices[29].pos = XMFLOAT3(-0.5f, -0.5f, -0.5f); vertices[29].tex = XMFLOAT2(1, 1);
	//right
	vertices[30].pos = XMFLOAT3(+0.5f, -0.5f, -0.5f); vertices[30].tex = XMFLOAT2(0, 1);
	vertices[31].pos = XMFLOAT3(+0.5f, +0.5f, -0.5f); vertices[31].tex = XMFLOAT2(0, 0);
	vertices[32].pos = XMFLOAT3(+0.5f, +0.5f, +0.5f); vertices[32].tex = XMFLOAT2(1, 0);
	vertices[33].pos = XMFLOAT3(+0.5f, -0.5f, -0.5f); vertices[33].tex = XMFLOAT2(0, 1);
	vertices[34].pos = XMFLOAT3(+0.5f, +0.5f, +0.5f); vertices[34].tex = XMFLOAT2(1, 0);
	vertices[35].pos = XMFLOAT3(+0.5f, -0.5f, +0.5f); vertices[35].tex = XMFLOAT2(1, 1);

	indices = new UINT[36];
	for (UINT i = 0; i < 36; ++i) {
		indices[i] = i;
	}

	hr = CalculateTangentAndBiTangent(vertices, indices, 36);
	if (FAILED(hr)) {
		return hr;
	}

	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(Vertex) * 36;
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;

	D3D11_SUBRESOURCE_DATA verticesSourceData;
	ZeroMemory(&verticesSourceData, sizeof(D3D11_SUBRESOURCE_DATA));
	verticesSourceData.pSysMem = vertices;

	pDevice->CreateBuffer(&vertexBufferDesc, &verticesSourceData, pVertexBufferObject);
	if (FAILED(hr)) {
		return hr;
	}

	D3D11_BUFFER_DESC indexDesc;
	ZeroMemory(&indexDesc, sizeof(D3D11_BUFFER_DESC));
	indexDesc.Usage = D3D11_USAGE_DEFAULT;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(UINT) * 36;

	D3D11_SUBRESOURCE_DATA indexData;
	ZeroMemory(&indexData, sizeof(D3D11_SUBRESOURCE_DATA));
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
	indexDesc.Usage = D3D11_USAGE_DYNAMIC;
	indexDesc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	indexDesc.ByteWidth = sizeof(UINT) * nNumVertex;
	indexDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;

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
	const INT nNumTex,
	ID3D11ShaderResourceView **pShaderResourceView,
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
	(*pImmediateContext)->PSSetShaderResources(0, nNumTex, pShaderResourceView);
	(*pImmediateContext)->PSSetSamplers(0, 1, &pSamplerState);
	(*pImmediateContext)->DrawIndexed(drawIndexCount, 0, 0);
	return S_OK;
}


inline HRESULT Update3DModelWorld(MatrixXD matrix, ID3D11Buffer *pMatrixDBuffer3D, ID3D11DeviceContext **pImmediateContext) {
	HRESULT hr;
	D3D11_MAPPED_SUBRESOURCE dms;
	hr = (*pImmediateContext)->Map(pMatrixDBuffer3D, 0, D3D11_MAP_WRITE_DISCARD, 0, &dms);
	if (FAILED(hr)) {
		return hr;
	}
	matrix.world = XMMatrixTranspose(matrix.world);
	matrix.view = XMMatrixTranspose(matrix.view);
	matrix.projection = XMMatrixTranspose(matrix.projection);
	auto* matrix3D = static_cast<MatrixXD*>(dms.pData);
	memcpy(matrix3D, &matrix, sizeof(matrix));
	(*pImmediateContext)->Unmap(pMatrixDBuffer3D, 0);
	return S_OK;
}


inline HRESULT InitFrustumPlane(const MatrixXD &matrix3D, XMVECTOR *&viewPlane) {
	const XMMATRIX vip = matrix3D.view * matrix3D.projection;
	if (viewPlane) {
		delete[] viewPlane;
		viewPlane = 0;
	}
	viewPlane = new XMVECTOR[6];
	//near
	viewPlane[0].m128_f32[0] = vip.r[0].m128_f32[3] + vip.r[0].m128_f32[2];
	viewPlane[0].m128_f32[1] = vip.r[1].m128_f32[3] + vip.r[1].m128_f32[2];
	viewPlane[0].m128_f32[2] = vip.r[2].m128_f32[3] + vip.r[2].m128_f32[2];
	viewPlane[0].m128_f32[3] = vip.r[3].m128_f32[3] + vip.r[3].m128_f32[2];
	viewPlane[0] = XMPlaneNormalize(viewPlane[0]);
	//far
	viewPlane[1].m128_f32[0] = vip.r[0].m128_f32[3] - vip.r[0].m128_f32[2];
	viewPlane[1].m128_f32[1] = vip.r[1].m128_f32[3] - vip.r[1].m128_f32[2];
	viewPlane[1].m128_f32[2] = vip.r[2].m128_f32[3] - vip.r[2].m128_f32[2];
	viewPlane[1].m128_f32[3] = vip.r[3].m128_f32[3] - vip.r[3].m128_f32[2];
	viewPlane[1] = XMPlaneNormalize(viewPlane[1]);
	//left
	viewPlane[2].m128_f32[0] = vip.r[0].m128_f32[3] + vip.r[0].m128_f32[0];
	viewPlane[2].m128_f32[1] = vip.r[1].m128_f32[3] + vip.r[1].m128_f32[0];
	viewPlane[2].m128_f32[2] = vip.r[2].m128_f32[3] + vip.r[2].m128_f32[0];
	viewPlane[2].m128_f32[3] = vip.r[3].m128_f32[3] + vip.r[3].m128_f32[0];
	viewPlane[2] = XMPlaneNormalize(viewPlane[2]);
	//right																   
	viewPlane[3].m128_f32[0] = vip.r[0].m128_f32[3] - vip.r[0].m128_f32[0];
	viewPlane[3].m128_f32[1] = vip.r[1].m128_f32[3] - vip.r[1].m128_f32[0];
	viewPlane[3].m128_f32[2] = vip.r[2].m128_f32[3] - vip.r[2].m128_f32[0];
	viewPlane[3].m128_f32[3] = vip.r[3].m128_f32[3] - vip.r[3].m128_f32[0];
	viewPlane[3] = XMPlaneNormalize(viewPlane[3]);
	//top
	viewPlane[4].m128_f32[0] = vip.r[0].m128_f32[3] - vip.r[0].m128_f32[1];
	viewPlane[4].m128_f32[1] = vip.r[1].m128_f32[3] - vip.r[1].m128_f32[1];
	viewPlane[4].m128_f32[2] = vip.r[2].m128_f32[3] - vip.r[2].m128_f32[1];
	viewPlane[4].m128_f32[3] = vip.r[3].m128_f32[3] - vip.r[3].m128_f32[1];
	viewPlane[4] = XMPlaneNormalize(viewPlane[4]);
	//bottom
	viewPlane[5].m128_f32[0] = vip.r[0].m128_f32[3] + vip.r[0].m128_f32[1];
	viewPlane[5].m128_f32[1] = vip.r[1].m128_f32[3] + vip.r[1].m128_f32[1];
	viewPlane[5].m128_f32[2] = vip.r[2].m128_f32[3] + vip.r[2].m128_f32[1];
	viewPlane[5].m128_f32[3] = vip.r[3].m128_f32[3] + vip.r[3].m128_f32[1];
	viewPlane[5] = XMPlaneNormalize(viewPlane[5]);

	return S_OK;
}


inline bool CheckCube(XMVECTOR *&viewPlane, const FLOAT centerX, const FLOAT centerY, const FLOAT centerZ, const FLOAT R) {
	for (int i = 0; i < 6; ++i) {
		float dot;
		XMStoreFloat(&dot, XMPlaneDotCoord(viewPlane[i], XMVectorSet(centerX + R, centerY + R, centerZ + R, 0.0f))); if (dot > 0.0f) continue;
		XMStoreFloat(&dot, XMPlaneDotCoord(viewPlane[i], XMVectorSet(centerX + R, centerY + R, centerZ - R, 0.0f))); if (dot > 0.0f) continue;
		XMStoreFloat(&dot, XMPlaneDotCoord(viewPlane[i], XMVectorSet(centerX + R, centerY - R, centerZ + R, 0.0f))); if (dot > 0.0f) continue;
		XMStoreFloat(&dot, XMPlaneDotCoord(viewPlane[i], XMVectorSet(centerX + R, centerY - R, centerZ - R, 0.0f))); if (dot > 0.0f) continue;
		XMStoreFloat(&dot, XMPlaneDotCoord(viewPlane[i], XMVectorSet(centerX - R, centerY + R, centerZ + R, 0.0f))); if (dot > 0.0f) continue;
		XMStoreFloat(&dot, XMPlaneDotCoord(viewPlane[i], XMVectorSet(centerX - R, centerY + R, centerZ - R, 0.0f))); if (dot > 0.0f) continue;
		XMStoreFloat(&dot, XMPlaneDotCoord(viewPlane[i], XMVectorSet(centerX - R, centerY - R, centerZ + R, 0.0f))); if (dot > 0.0f) continue;
		XMStoreFloat(&dot, XMPlaneDotCoord(viewPlane[i], XMVectorSet(centerX - R, centerY - R, centerZ - R, 0.0f))); if (dot > 0.0f) continue;
		return false;
	}
	return true;
}

inline HRESULT InitTextureResourceView(
	const INT width , 
	const INT height ,
	ID3D11Device *pDevice,
	ID3D11Texture2D **pTexure2D , 
	ID3D11RenderTargetView **pRenderTargetView , 
	ID3D11ShaderResourceView ** pShaderResourceView) {
	
	HRESULT hr;
	D3D11_TEXTURE2D_DESC td;
	ZeroMemory(&td, sizeof(td));
	td.Width = width;
	td.Height = height;
	td.MipLevels = 1;
	td.ArraySize = 1;
	td.Format = DXGI_FORMAT_R32G32B32A32_FLOAT;
	td.SampleDesc.Count = 1;
	td.Usage = D3D11_USAGE_DEFAULT;
	td.BindFlags = D3D11_BIND_RENDER_TARGET | D3D11_BIND_SHADER_RESOURCE;

	hr = pDevice->CreateTexture2D(&td, nullptr, pTexure2D);
	if (FAILED(hr)) {
		return hr;
	}

	D3D11_RENDER_TARGET_VIEW_DESC rd;
	ZeroMemory(&rd, sizeof(rd));
	rd.Format = td.Format;
	rd.ViewDimension = D3D11_RTV_DIMENSION_TEXTURE2D;
	rd.Texture2D.MipSlice = 0;

	hr = pDevice->CreateRenderTargetView(*pTexure2D, &rd, pRenderTargetView);
	if (FAILED(hr)) {
		return hr;
	}

	D3D11_SHADER_RESOURCE_VIEW_DESC sd;
	ZeroMemory(&sd, sizeof(sd));
	sd.Format = td.Format;
	sd.ViewDimension = D3D11_SRV_DIMENSION_TEXTURE2D;
	sd.Texture2D.MipLevels = 1;

	hr = pDevice->CreateShaderResourceView(*pTexure2D, &sd, &pShaderResourceView[0]);
	if (FAILED(hr)) {
		return hr;
	}

	return S_OK;
}