#include <D3D11.h>
#include <minwinbase.h>
#include <iostream>
#include <Windows.h>
#include <D3DX10math.h>
#include <D3DX11async.h>
#include <DirectXMath.h>
#include <D3Dcompiler.h>
#include <fstream>
#include <XInput.h>

#include "Common.h"









////////////////////////////////////////////////////////////////////////////// Main //////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, INT nCmdShow) {

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
	int nNumVertex = 0;



	hr = InitWindowAndD3D(hInstance, "DirectInput", &pSwapChain, &pRenderTargetView, &pDevice, &pImmediateContext);
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
	
	hr = InitVertex(pDevice, "Hello ksgin", 0, 0, fonts , &pImmediateContext , &nNumVertex , &vertices);
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
	
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		float color[] = { 0.0f , 0.0f , 0.0f , 1.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, color);
		pImmediateContext->VSSetShader(pVertexShader, 0, 0);
		pImmediateContext->PSSetShader(pPixelShader, 0, 0);
		pImmediateContext->IASetInputLayout(pInputLayout);
		pImmediateContext->Draw(6 * nNumVertex , 0);
		pSwapChain->Present(0, 0);
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
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

	return 0;
}


