
#include "Common.h"
#include <D3D11.h>
#include <minwinbase.h>
#include <Windows.h>
#include <D3DX10math.h>
#include <DirectXMath.h>
#include <Pdh.h>

#pragma comment(lib, "winmm.lib")
#pragma comment(lib, "pdh.lib")

////////////////////////////////////////////////////////////////////////////// Input ///////////////////////////////////////////////////////////////////////
#define DIRECTINPUT8_VERSION 0x0800
#define KEYDOWN(name,key) (name[key]&0x80)


////////////////////////////////////////////////////////////////////////////// Main //////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {

	HWND hWnd;

	ID3D11RenderTargetView *pRenderTargetView = nullptr;
	ID3D11Device *pDevice = nullptr;
	ID3D11DeviceContext *pImmediateContext = nullptr;
	IDXGISwapChain *pSwapChain = nullptr;
	Font *fonts = nullptr;
	Vertex *vertices = nullptr;
	ID3D11VertexShader *pVertexShader = nullptr;
	ID3D11PixelShader *pPixelShader = nullptr;
	ID3D11InputLayout *pInputLayout = nullptr;
	HRESULT hr;

	hr = InitWindowAndD3D(hInstance, "DirectInput", &hWnd, &pSwapChain, &pRenderTargetView, &pDevice, &pImmediateContext);
	if (FAILED(hr)) {
		return hr;
	}
	hr = SetDepth(pDevice, &pImmediateContext, &pRenderTargetView);
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitLetter(&fonts);
	if (FAILED(hr)) {
		return hr;
	}

	hr = InitConstant(pDevice, &pImmediateContext);
	if (FAILED(hr)) {
		return hr;
	}
	hr = InitShader(pDevice, &pImmediateContext, &pVertexShader, &pPixelShader, &pInputLayout);
	if (FAILED(hr)) {
		return hr;
	}

	const string fpsText = "FPS : ";
	int tCount = 0;
	int fps = 0;
	unsigned long startTime = timeGetTime();

	const string cpuText = "CUP : ";
	unsigned long lastSampleTime = timeGetTime();
	int cpuUsage = 0;
	bool canSample = true;
	HQUERY hQuery = nullptr;
	HCOUNTER hCounter = nullptr;
	PDH_STATUS status;

	status = PdhOpenQuery(nullptr, 0, &hQuery);
	if (status != ERROR_SUCCESS) {
		MessageBox(nullptr, "ERROR::OpenQuery", "ERROR", MB_OK);
		canSample = false;
	}
	status = PdhAddCounter(hQuery, TEXT("\\Processor(_Total)\\% processor time"), 0, &hCounter);
	if (status != ERROR_SUCCESS) {
		MessageBox(nullptr, "ERROR::AddCounter", "ERROR", MB_OK);
		canSample = false;
	}

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		float color[] = { 0.0f , 0.0f , 0.0f , 1.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, color);
		PrintText(fpsText + to_string(GetFPS(startTime , fps , tCount)) , -620, 300, fonts , pVertexShader, pPixelShader, pInputLayout, pDevice, &vertices, &pImmediateContext);
		PrintText(cpuText + to_string(GetCPUUsage(canSample , cpuUsage , lastSampleTime , hQuery , hCounter)) , -620, 240, fonts , pVertexShader, pPixelShader, pInputLayout, pDevice, &vertices, &pImmediateContext);
		pSwapChain->Present(0, 0);
	}

	///////////////////////////////////////////////////////////////////////////// Release ///////////////////////////////////////////////////////////////////////////
	pImmediateContext->Release();
	pRenderTargetView->Release();
	pDevice->Release();
	pVertexShader->Release();
	pPixelShader->Release();
	pInputLayout->Release();
	delete[] fonts;
	delete[] vertices;
	fonts = 0;
	vertices = 0;
	PdhCloseQuery(hQuery);

	return 0;
}


