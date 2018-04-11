
#include "Common.h"

#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>

const string fpsText = "FPS : ";
int tCount = 0;
int fps = 9999;
unsigned long startTime = timeGetTime();
const int offsetX = 5;
const int offsetY = 5;
float bkgc[4] = { 0.5f , 0.5f , 0.5f , 1.0f };
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

	const int nNumScreenTex = 1;
	ID3D11VertexShader				*pScreenVertexShader = nullptr;
	ID3D11PixelShader				*pScreenPixelShader = nullptr;
	ID3D11InputLayout				*pScreenInputLayout = nullptr;
	ID3D11SamplerState				*pScreenSamplerState = nullptr;
	ID3D11ShaderResourceView *pScreenShaderResourceView[nNumScreenTex];

	ID3D11RenderTargetView *pRenderTargetViewT = nullptr;
	ID3D11Texture2D *pTexure2DT = nullptr;

	UINT nNumScreenVertex = 0;

	ID3D11Buffer *pScreenVertexBufferObject = nullptr;
	ID3D11Buffer *pScreenIndexBufferObject = nullptr;
	Vertex *pScreenVertices = nullptr;
	UINT *pScreenIndices = nullptr;	

	const FLOAT snear = 0.01f;
	const FLOAT sfar = 100.0f;
	MatrixXD matrix3d = {
	XMMatrixIdentity() * XMMatrixScaling(0.3f , 0.3f , 0.3f),
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
		(fpsText + to_string(GetFPS(startTime, fps, tCount))).c_str(),
		-width / 2 + offsetX, height / 2 - offsetY, fonts, pD3DDevice,
		&pFontVertexBufferObject, &pFontIndexBufferObject,
		sentenceVertexNum, fontVertice, fontIndices);
	if (FAILED(hr)) {
		return hr;
	}

	D3D11_VIEWPORT d3D11Viewport;
	d3D11Viewport.Width = width;
	d3D11Viewport.Height = height;
	d3D11Viewport.TopLeftX = 0;
	d3D11Viewport.TopLeftY = 0;
	d3D11Viewport.MinDepth = 0;
	d3D11Viewport.MaxDepth = 1;
	pD3DImmediateContext->RSSetViewports(1, &d3D11Viewport);


	InitScreenVertices(
		width,
		height,
		pD3DDevice,
		&pScreenVertexBufferObject,	
		&pScreenIndexBufferObject,
		nNumScreenVertex,
		pScreenVertices,
		pScreenIndices);

	hr = InitShader(
		"./screenVertexShader.hlsl",
		"./screenPixelShader.hlsl",
		fontTexs , 
		nNumFontTex,
		pD3DDevice,
		&pScreenVertexShader,
		&pScreenPixelShader,
		&pScreenInputLayout,
		&pScreenSamplerState,
		pScreenShaderResourceView);
	if (FAILED(hr)) {
		return hr;
	}

	hr = InitTextureResourceView(
		width,
		height,
		pD3DDevice,
		&pTexure2DT,
		&pRenderTargetViewT,
		pScreenShaderResourceView);
	if (FAILED(hr)) {
		return hr;
	}

	vector<XMMATRIX> worldMatrixList(5);
	vector<XMFLOAT3> rotateDir(5);
	for (int i = 0; i < 5; ++i) {
		worldMatrixList[i] = XMMatrixIdentity() * XMMatrixScaling(0.2f, 0.2f, 0.2f) * XMMatrixTranslation(
			(rand() - rand()) % 2, (rand() - rand()) % 2, (rand() - rand()) % 2);
		rotateDir[i] = XMFLOAT3((rand() - rand()) % 2 * 0.001f, (rand() - rand()) % 2 * 0.001f, (rand() - rand()) % 2 * 0.001f);
	}


	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		pD3DImmediateContext->ClearRenderTargetView(pD3DRenderTargetView, bkgc);
		pD3DImmediateContext->ClearRenderTargetView(pRenderTargetViewT, bkgc);
		pD3DImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		for (int i = 0; i < 5; ++i) {
			worldMatrixList[i] *= XMMatrixRotationX(rotateDir[i].x) * XMMatrixRotationY(rotateDir[i].y) * XMMatrixRotationZ(rotateDir[i].z);
			matrix3d.world = worldMatrixList[i];
			Update3DModelWorld(matrix3d, pMatrixDBuffer3D, &pD3DImmediateContext);
			DrawModelIndex(
				cubeVertexNum,
				pCubeVertexBufferObject,
				pCubeIndexBufferObject,
				pMatrixDBuffer3D,
				pCubeSamplerState,
				nNumCubeTex,
				pCubeShaderResourceView,
				pCubeVertexShader,
				pCubePixelShader,
				pCubeInputLayout,
				pDepthStencilView,
				pEnableDepthStencilState,
				&pRenderTargetViewT,
				&pD3DImmediateContext);
		}


		UpdateSentenceVertexAndIndexBuffer(
			(fpsText + to_string(GetFPS(startTime, fps, tCount))).c_str(),
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
			&pRenderTargetViewT,
			&pD3DImmediateContext);

		DrawModelIndex(
			nNumScreenVertex,
			pScreenVertexBufferObject,
			pScreenIndexBufferObject,
			pMatrixDBuffer2D,
			pScreenSamplerState,
			1 ,
			pScreenShaderResourceView,
			pScreenVertexShader,
			pScreenPixelShader,
			pScreenInputLayout,
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
	if (pScreenVertexBufferObject) { pScreenVertexBufferObject->Release(); }
	if (pScreenIndexBufferObject) { pScreenIndexBufferObject->Release(); }
	if (pTexure2DT) { pTexure2DT->Release(); }
	if (pRenderTargetViewT) { pRenderTargetViewT->Release(); }
	for (auto& i : pScreenShaderResourceView) if (i) i->Release();
	for (auto& i : pCubeShaderResourceView) if (i) i->Release();
	for (auto& i : pFontShaderResourceView) if (i) i->Release();
	delete fonts;
	delete[] fontVertice;
	delete[] fontIndices;
	delete[] cubeVertice;
	delete[] cubeIndices;
	delete[] pScreenIndices;
	delete[] pScreenVertices;

	_CrtDumpMemoryLeaks();
	return 0;
}


