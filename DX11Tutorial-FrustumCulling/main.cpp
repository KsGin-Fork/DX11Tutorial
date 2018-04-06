
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
float backgroundColor[4] = { 0.0f , 0.0f , 0.0f , 1.0f };
const string cubeText = "   CUBE : ";
int cube = 9999;
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
	ID3D11ShaderResourceView		*pCubeShaderResourceView = nullptr;
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
	ID3D11ShaderResourceView		*pFontShaderResourceView = nullptr;

	const FLOAT snear = 0.01f;
	const FLOAT sfar = 100.0f;
	MatrixXD matrix3d = {
	XMMatrixIdentity() ,
	XMMatrixLookAtLH(
		XMVectorSet(0.0f, 0.0f , -100.0f , 0.0f),
		XMVectorSet(0.0f, 0.0f ,  0.0f , 0.0f),
		XMVectorSet(0.0f, 1.0f ,  0.0f , 0.0f)
	),
	XMMatrixPerspectiveFovLH(90 ,static_cast<float>(width) / static_cast<float>(height) , snear , sfar)
	};

	XMVECTOR *viewPlanes = nullptr;
	InitFrustumPlane(sfar , matrix3d, viewPlanes);

	const UINT nNumWorldTranslateList = 5000;
	auto *worldTranslateList = new XMFLOAT3[nNumWorldTranslateList];
	auto *worldList = new XMMATRIX[nNumWorldTranslateList];
	for (int i = 0 ; i < nNumWorldTranslateList ; i++) {
		worldTranslateList[i] = XMFLOAT3(
			(static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX * nNumWorldTranslateList / 10, 
			(static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX * nNumWorldTranslateList / 10, 
			(static_cast<float>(rand()) - static_cast<float>(rand())) / RAND_MAX * nNumWorldTranslateList / 10);	
		worldList[i] = XMMatrixIdentity() * XMMatrixTranslation(worldTranslateList[i].x, worldTranslateList[i].y, worldTranslateList[i].z);
	}

	HRESULT hr;
	hr = InitWindowAndD3D(hInstance, "FrustumCulling", &hWnd, &pD3DSwapChain, &pD3DRenderTargetView, &pD3DDevice, &pD3DImmediateContext);
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
	hr = Init3DConstant(matrix3d,pD3DDevice, &pMatrixDBuffer3D);
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitShader(
		"./cubeVertexShader.hlsl",
		"./cubePixelShader.hlsl",
		"./texture.jpg",
		pD3DDevice,
		&pCubeVertexShader,
		&pCubePixelShader,
		&pCubeInputLayout,
		&pCubeSamplerState,
		&pCubeShaderResourceView);
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
		"./font.dds",
		pD3DDevice,
		&pFontVertexShader,
		&pFontPixelShader,
		&pFontInputLayout,
		&pFontSamplerState,
		&pFontShaderResourceView);
	if (FAILED(hr)) {
		return hr;
	}

	hr = InitSentenceVertexAndIndexBuffer(
		(fpsText + to_string(GetFPS(startTime, fps, tCount)) + cubeText + std::to_string(cube) + cpuText + to_string(GetCPUUsage(canSample, cpuUsage, lastSampleTime, hQuery, hCounter))).c_str(),
		-width / 2 + offsetX, height / 2 - offsetY, fonts, pD3DDevice,
		&pFontVertexBufferObject, &pFontIndexBufferObject,
		sentenceVertexNum, fontVertice, fontIndices);
	if (FAILED(hr)) {
		return hr;
	}

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		pD3DImmediateContext->ClearRenderTargetView(pD3DRenderTargetView, backgroundColor);
		pD3DImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);

		cube = 0;

		for (int i = 0; i < nNumWorldTranslateList; ++i) {
			worldTranslateList[i] = XMFLOAT3(worldTranslateList[i].x , worldTranslateList[i].y , worldTranslateList[i].z + 0.03f);
			worldList[i] *= XMMatrixTranslation(0.0f , 0.0f , 0.03f);
			if (CheckCube(viewPlanes, worldTranslateList[i].x, worldTranslateList[i].y, worldTranslateList[i].z, 0.5f)) {				
				matrix3d.world = worldList[i];
				Update3DModelWorld(matrix3d, pMatrixDBuffer3D, &pD3DImmediateContext);
				DrawModelIndex(
					cubeVertexNum,
					pCubeVertexBufferObject,
					pCubeIndexBufferObject,
					pMatrixDBuffer3D,
					pCubeSamplerState,
					pCubeShaderResourceView,
					pCubeVertexShader,
					pCubePixelShader,
					pCubeInputLayout,
					pDepthStencilView,
					pEnableDepthStencilState,
					&pD3DRenderTargetView,
					&pD3DImmediateContext);
				++cube;
			}
		}	

		UpdateSentenceVertexAndIndexBuffer(
			(fpsText + to_string(GetFPS(startTime, fps, tCount)) + cubeText + std::to_string(cube) + cpuText + to_string(GetCPUUsage(canSample, cpuUsage, lastSampleTime, hQuery, hCounter))).c_str(),
			-width / 2 + offsetX, height / 2 - offsetY, fonts,
			&pFontVertexBufferObject, &pFontIndexBufferObject, &pD3DImmediateContext,
			sentenceVertexNum, fontVertice, fontIndices);	

		//MessageBox(nullptr, std::to_string(cot).c_str(), "NOTE", MB_OK);
		DrawModelIndex(
			sentenceVertexNum,
			pFontVertexBufferObject,
			pFontIndexBufferObject,
			pMatrixDBuffer2D,
			pFontSamplerState,
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
	if (pCubeShaderResourceView) pCubeShaderResourceView->Release();
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
	if (pFontShaderResourceView) pFontShaderResourceView->Release();
	delete fonts;
	delete[] fontVertice;
	delete[] fontIndices;
	delete[] cubeVertice;
	delete[] cubeIndices;
	PdhCloseQuery(hQuery);

	_CrtDumpMemoryLeaks();
	return 0;
}


