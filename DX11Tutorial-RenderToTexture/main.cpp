
#include "Common.h"

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

bool canSample = true;
HQUERY hQuery = nullptr;
HCOUNTER hCounter = nullptr;
const string fpsText = "FPS : ";
int tCount = 0;
int fps = 9999;
unsigned long startTime = timeGetTime();
const string cpuText = "   CPU : ";
unsigned long lastSampleTime = timeGetTime();
int cpuUsage = 9999;
const int offsetX = 5;
const int offsetY = 5;
float backgroundColor1[4] = { 0.0f , 0.0f , 0.0f , 1.0f };
float backgroundColor2[4] = { 0.0f , 0.5f , 0.0f , 1.0f };
////////////////////////////////////////////////////////////////////////////// Main //////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(const HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {
	srand(timeGetTime());
	//_CrtSetBreakAlloc(168);
	_CrtSetDbgFlag(_CRTDBG_ALLOC_MEM_DF | _CRTDBG_LEAK_CHECK_DF);
	HWND hWnd;

	ID3D11RenderTargetView			*pD3DRenderTargetView = nullptr;
	ID3D11Device					*pD3DDevice = nullptr;
	ID3D11DeviceContext				*pD3DImmediateContext = nullptr;
	IDXGISwapChain					*pD3DSwapChain = nullptr;
	ID3D11DepthStencilView			*pDepthStencilView = nullptr;
	ID3D11DepthStencilState			*pDisableDepthStencilState = nullptr;
	ID3D11DepthStencilState			*pEnableDepthStencilState = nullptr;

	const UINT cubeVertexNum = 36;
	ID3D11SamplerState				*pCubeSamplerState = nullptr;
	ID3D11Buffer					*pCubeVertexBufferObject = nullptr;
	ID3D11Buffer					*pCubeIndexBufferObject = nullptr;
	ID3D11Buffer					*pMatrixDBuffer3D = nullptr;
	ID3D11Buffer					*pMatrixDBuffer2D = nullptr;
	Vertex							*cubeVertice = nullptr;
	UINT							*cubeIndices = nullptr;
	ID3D11VertexShader				*pCubeVertexShader = nullptr;
	ID3D11PixelShader				*pCubePixelShader = nullptr;
	ID3D11InputLayout				*pCubeInputLayout = nullptr;

	UINT sentenceVertexNum = 0;
	Font							*fonts = nullptr;
	Vertex							*fontVertice = nullptr;
	UINT							*fontIndices = nullptr;
	ID3D11Buffer					*pFontVertexBufferObject = nullptr;
	ID3D11Buffer					*pFontIndexBufferObject = nullptr;
	ID3D11VertexShader				*pFontVertexShader = nullptr;
	ID3D11PixelShader				*pFontPixelShader = nullptr;
	ID3D11InputLayout				*pFontInputLayout = nullptr;
	ID3D11SamplerState				*pFontSamplerState = nullptr;
	

	const char * cubeTexs[] = {
		"1.png"
	};
	const int nNumCubeTex = 1;
	ID3D11ShaderResourceView		*pCubeShaderResourceView[nNumCubeTex];
	const char * fontTexs[] = {
		"font.dds"
	};
	const int nNumFontTex = 1;
	ID3D11ShaderResourceView		*pFontShaderResourceView[nNumFontTex];

	const FLOAT snear = 0.01f;
	const FLOAT sfar = 100.0f;
	MatrixXD matrix3d = {
	XMMatrixIdentity() ,
	XMMatrixLookAtLH(
		XMVectorSet(1.0f, 1.0f , -1.0f , 0.0f),
		XMVectorSet(0.0f, 0.0f ,  0.0f , 0.0f),
		XMVectorSet(0.0f, 1.0f ,  0.0f , 0.0f)
	),
	XMMatrixPerspectiveFovLH(90 ,static_cast<float>(width) / static_cast<float>(height) , snear , sfar)
	};

	HRESULT hr;
	hr = InitWindowAndD3D(hInstance, "BumpMapping", &hWnd, &pD3DSwapChain, &pD3DRenderTargetView, &pD3DDevice, &pD3DImmediateContext);
	if (FAILED(hr)) {
		return hr;
	}
	hr = SetDepthStencil(pD3DDevice, &pDepthStencilView);
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitEnableDepth(pD3DDevice, &pEnableDepthStencilState);
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitDisableDepth(pD3DDevice, &pDisableDepthStencilState);
	if (FAILED(hr)) {
		return hr;
	}
	hr = Init3DConstant(matrix3d, pD3DDevice, &pMatrixDBuffer3D);
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitShader(
		"./cubeVertexShader.hlsl",
		"./cubePixelShader.hlsl",
		cubeTexs , 
		nNumCubeTex ,
		pD3DDevice,
		&pCubeVertexShader,
		&pCubePixelShader,
		&pCubeInputLayout,
		&pCubeSamplerState,
		pCubeShaderResourceView);
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitCubeVertexAndIndexBuffer(pD3DDevice, &pCubeVertexBufferObject, &pCubeIndexBufferObject, cubeVertice, cubeIndices);
	if (FAILED(hr)) {
		return hr;
	}

	InitLetter(&fonts);
	Init2DConstant(pD3DDevice, &pMatrixDBuffer2D);
	InitQueryAndCounter(&hQuery, &hCounter, &canSample);


	hr = InitShader(
		"./fontVertexShader.hlsl",
		"./fontPixelShader.hlsl",
		fontTexs , 
		nNumFontTex,
		pD3DDevice,
		&pFontVertexShader,
		&pFontPixelShader,
		&pFontInputLayout,
		&pFontSamplerState,
		pFontShaderResourceView);
	if (FAILED(hr)) {
		return hr;
	}

	hr = InitSentenceVertexAndIndexBuffer(
		(fpsText + to_string(GetFPS(startTime, fps, tCount)) + cpuText + to_string(GetCPUUsage(canSample, cpuUsage, lastSampleTime, hQuery, hCounter))).c_str(),
		-width / 2 + offsetX, height / 2 - offsetY, fonts, pD3DDevice,
		&pFontVertexBufferObject, &pFontIndexBufferObject,
		sentenceVertexNum, fontVertice, fontIndices);
	if (FAILED(hr)) {
		return hr;
	}


	ID3D11ShaderResourceView *pShaderResourceViewT[nNumCubeTex];
	ID3D11RenderTargetView *pRenderTargetViewT = nullptr;
	ID3D11Texture2D *pTexure2DT = nullptr;

	hr = InitTextureResourceView(
		200,
		200,
		pD3DDevice,
		&pTexure2DT,
		&pRenderTargetViewT,
		pShaderResourceViewT);
	if (FAILED(hr)) {
		return hr;
	}

	D3D11_VIEWPORT dv1;
	dv1.Width = width;
	dv1.Height = height;
	dv1.TopLeftX = 0;
	dv1.TopLeftY = 0;
	dv1.MinDepth = 0;
	dv1.MaxDepth = 1;

	D3D11_VIEWPORT dv2;
	dv2.Width = 200;
	dv2.Height = 200;
	dv2.TopLeftX = 0;
	dv2.TopLeftY = 0;
	dv2.MinDepth = 0;
	dv2.MaxDepth = 1;

	const MatrixXD matrixTex = {
	XMMatrixIdentity() * XMMatrixScaling(3.0f , 3.0f , 3.0f) ,
	XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f , -4.0f , 0.0f),
		XMVectorSet(0.0f, 0.0f ,  0.0f , 0.0f),
		XMVectorSet(0.0f, 1.0f ,  0.0f , 0.0f)
	),
	XMMatrixPerspectiveFovLH(90 , 1.0f , snear , sfar)
	};

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		pD3DImmediateContext->ClearRenderTargetView(pD3DRenderTargetView, backgroundColor1);
		pD3DImmediateContext->ClearRenderTargetView(pRenderTargetViewT, backgroundColor2);
		pD3DImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		pD3DImmediateContext->RSSetViewports(1, &dv2);
		Update3DModelWorld(matrixTex, pMatrixDBuffer3D, &pD3DImmediateContext);
		DrawModelIndex(
			cubeVertexNum,
			pCubeVertexBufferObject,
			pCubeIndexBufferObject,
			pMatrixDBuffer3D,
			pCubeSamplerState,
			nNumCubeTex ,
			pCubeShaderResourceView,
			pCubeVertexShader,
			pCubePixelShader,
			pCubeInputLayout,
			pDepthStencilView,
			pEnableDepthStencilState,
			&pRenderTargetViewT,
			&pD3DImmediateContext);

		pD3DImmediateContext->RSSetViewports(1, &dv1);
		matrix3d.world *= XMMatrixRotationY(0.0001);
		Update3DModelWorld(matrix3d, pMatrixDBuffer3D, &pD3DImmediateContext);
		DrawModelIndex(
			cubeVertexNum,
			pCubeVertexBufferObject,
			pCubeIndexBufferObject,
			pMatrixDBuffer3D,
			pCubeSamplerState,
			nNumCubeTex ,
			pShaderResourceViewT,
			pCubeVertexShader,
			pCubePixelShader,
			pCubeInputLayout,
			pDepthStencilView,
			pEnableDepthStencilState,
			&pD3DRenderTargetView,
			&pD3DImmediateContext);

		UpdateSentenceVertexAndIndexBuffer(
			(fpsText + to_string(GetFPS(startTime, fps, tCount)) + cpuText + to_string(GetCPUUsage(canSample, cpuUsage, lastSampleTime, hQuery, hCounter))).c_str(),
			-width / 2 + offsetX, height / 2 - offsetY, fonts,
			&pFontVertexBufferObject, &pFontIndexBufferObject, &pD3DImmediateContext,
			sentenceVertexNum, fontVertice, fontIndices);

		DrawModelIndex(
			sentenceVertexNum,
			pFontVertexBufferObject,
			pFontIndexBufferObject,
			pMatrixDBuffer2D,
			pFontSamplerState,
			nNumFontTex ,
			pFontShaderResourceView,
			pFontVertexShader,
			pFontPixelShader,
			pFontInputLayout,
			pDepthStencilView,
			pDisableDepthStencilState,
			&pD3DRenderTargetView,
			&pD3DImmediateContext);

		pD3DSwapChain->Present(0, 0);
	}

	///////////////////////////////////////////////////////////////////////////// Release ///////////////////////////////////////////////////////////////////////////
	if (pD3DRenderTargetView) pD3DRenderTargetView->Release();
	if (pD3DDevice) pD3DDevice->Release();
	if (pD3DImmediateContext) pD3DImmediateContext->Release();
	if (pD3DSwapChain) pD3DSwapChain->Release();
	if (pDepthStencilView) pDepthStencilView->Release();
	if (pDisableDepthStencilState) pDisableDepthStencilState->Release();
	if (pEnableDepthStencilState) pEnableDepthStencilState->Release();
	if (pCubeSamplerState) pCubeSamplerState->Release();	
	if (pCubeVertexBufferObject) pCubeVertexBufferObject->Release();
	if (pCubeIndexBufferObject) pCubeIndexBufferObject->Release();
	if (pMatrixDBuffer3D) pMatrixDBuffer3D->Release();
	if (pMatrixDBuffer2D) pMatrixDBuffer2D->Release();
	if (pCubeVertexShader) pCubeVertexShader->Release();
	if (pCubePixelShader) pCubePixelShader->Release();
	if (pCubeInputLayout) pCubeInputLayout->Release();
	if (pFontVertexBufferObject) pFontVertexBufferObject->Release();
	if (pFontIndexBufferObject) pFontIndexBufferObject->Release();
	if (pFontVertexShader) pFontVertexShader->Release();
	if (pFontPixelShader) pFontPixelShader->Release();
	if (pFontInputLayout) pFontInputLayout->Release();
	if (pFontSamplerState) pFontSamplerState->Release();
	for (auto& i : pCubeShaderResourceView) if (i) i->Release();
	for (auto& i : pFontShaderResourceView) if (i) i->Release();
	delete fonts;
	delete[] fontVertice;
	delete[] fontIndices;
	delete[] cubeVertice;
	delete[] cubeIndices;
	PdhCloseQuery(hQuery);

	_CrtDumpMemoryLeaks();
	return 0;
}


