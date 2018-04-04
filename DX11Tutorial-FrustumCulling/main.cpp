
#include "Common.h"

bool canSample = true;
HQUERY hQuery = nullptr;
HCOUNTER hCounter = nullptr;
const string fpsText = "FPS : ";
int tCount = 0;
int fps = 0;
unsigned long startTime = timeGetTime();
const string cpuText = "  CPU : ";
unsigned long lastSampleTime = timeGetTime();
int cpuUsage = 0;
const int offsetX = 5;
const int offsetY = 5;
float backgroundColor[4] = { 0.1f , 0.3f , 0.1f , 1.0f };

////////////////////////////////////////////////////////////////////////////// Main //////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(const HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {

	HWND hWnd;

	ID3D11RenderTargetView *pD3DRenderTargetView = nullptr;
	ID3D11Device *pD3DDevice = nullptr;
	ID3D11DeviceContext *pD3DImmediateContext = nullptr;
	IDXGISwapChain *pD3DSwapChain = nullptr;
	ID3D11DepthStencilView *pDepthStencilView = nullptr;
	ID3D11DepthStencilState *pDisableDepthStencilState = nullptr;
	ID3D11DepthStencilState *pEnableDepthStencilState = nullptr;
	ID3D11SamplerState *pCubeSamplerState = nullptr;
	ID3D11ShaderResourceView *pCubeShaderResourceView = nullptr;
	ID3D11Buffer *pCubeVertexBufferObject = nullptr;
	ID3D11Buffer *pCubeIndexBufferObject = nullptr;
	ID3D11Buffer *pMatrixDBuffer3D = nullptr;
	ID3D11Buffer *pMatrixDBuffer2D = nullptr;
	Vertex *cubeVertice = nullptr;
	UINT *cubeIndices = nullptr;
	ID3D11VertexShader *pCubeVertexShader = nullptr;
	ID3D11PixelShader  *pCubePixelShader = nullptr;
	ID3D11InputLayout  *pCubeInputLayout = nullptr;

	UINT sentenceVertexNum = 0;
	Font *fonts = nullptr;
	Vertex *fontVertice = nullptr;
	UINT *fontIndices = nullptr;
	ID3D11Buffer *pFontVertexBufferObject = nullptr;
	ID3D11Buffer *pFontIndexBufferObject = nullptr;
	ID3D11VertexShader *pFontVertexShader = nullptr;
	ID3D11PixelShader  *pFontPixelShader = nullptr;
	ID3D11InputLayout  *pFontInputLayout = nullptr;
	ID3D11SamplerState *pFontSamplerState = nullptr;
	ID3D11ShaderResourceView *pFontShaderResourceView = nullptr;

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
	hr = Init3DConstant(pD3DDevice, &pMatrixDBuffer3D);
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

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		pD3DImmediateContext->ClearRenderTargetView(pD3DRenderTargetView, backgroundColor);
		pD3DImmediateContext->ClearDepthStencilView(pDepthStencilView, D3D11_CLEAR_DEPTH, 1.0f, 0);
		DrawModelIndex(
			36,
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
		
		hr = InitSentenceVertexAndIndexBuffer(
			(fpsText + to_string(GetFPS(startTime, fps, tCount)) + cpuText + to_string(GetCPUUsage(canSample, cpuUsage, lastSampleTime, hQuery, hCounter))).c_str(),
			- width / 2 + offsetX , height / 2 - offsetY , fonts , pD3DDevice , 
			&pFontVertexBufferObject , &pFontIndexBufferObject, 
			sentenceVertexNum , fontVertice , fontIndices);
		if (FAILED(hr)) {
			return hr;
		}
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
	pCubeSamplerState->Release();
	pCubeShaderResourceView->Release();
	pCubeVertexBufferObject->Release();
	pCubeIndexBufferObject->Release();
	pMatrixDBuffer3D->Release();
	pDisableDepthStencilState->Release();
	pEnableDepthStencilState->Release();
	pD3DImmediateContext->Release();
	pD3DRenderTargetView->Release();
	pD3DDevice->Release();
	pCubeVertexShader->Release();
	pCubePixelShader->Release();
	pCubeInputLayout->Release();
	delete[] cubeVertice;
	cubeVertice = 0;
	PdhCloseQuery(hQuery);

	_CrtDumpMemoryLeaks();

	return 0;
}


