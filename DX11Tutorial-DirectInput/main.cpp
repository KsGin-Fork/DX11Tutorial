
#include "Common.h"


#include <D3D11.h>
#include <minwinbase.h>
#include <Windows.h>
#include <D3DX10math.h>
#include <DirectXMath.h>
#include <dinput.h>
#include <iostream>


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
	int nNumVertex = 0;

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

	IDirectInputDevice8 *pKeyboard = nullptr;
	IDirectInputDevice8 *pMouse = nullptr;
	InitInputDevice(hInstance, hWnd, &pKeyboard, &pMouse);

	char pKeyBoardState[256];
	DIMOUSESTATE mouseState;
	int mouseX = 0, mouseY = 0;	

	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));
	while (msg.message != WM_QUIT) {
		if (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		hr = pKeyboard->GetDeviceState(sizeof(pKeyBoardState), static_cast<LPVOID>(&pKeyBoardState));
		if (FAILED(hr)) {
			if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
				pKeyboard->Acquire();
			}
			else {
				MessageBox(nullptr, "ERROR::GetKeyBoardState", "ERROR", MB_OK);
				return hr;
			}
		}

		hr = pMouse->GetDeviceState(sizeof(DIMOUSESTATE), static_cast<LPVOID>(&mouseState));
		if (FAILED(hr)) {
			if (hr == DIERR_INPUTLOST || hr == DIERR_NOTACQUIRED) {
				pMouse->Acquire();
			}
			else {
				MessageBox(nullptr, "ERROR::GetMouseState", "ERROR", MB_OK);
				return hr;
			}
		}

		if (pKeyBoardState[DIK_ESCAPE] & 0x80) {
			PostQuitMessage(0);
		}

		mouseX += mouseState.lX;
		mouseY += mouseState.lY;
		

		float color[] = { 0.0f , 0.0f , 0.0f , 1.0f };
		pImmediateContext->ClearRenderTargetView(pRenderTargetView, color);
		PrintMousePos(mouseX , mouseY , fonts , pVertexShader , pPixelShader , pInputLayout , pDevice , &pImmediateContext);
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

	pMouse->Unacquire();
	pMouse->Release();
	pMouse = 0;
	pKeyboard->Unacquire();
	pKeyboard->Release();
	pKeyboard = 0;

	return 0;
}


