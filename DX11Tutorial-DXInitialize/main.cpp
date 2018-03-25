#include <D3D11.h>
#include <minwinbase.h>
#define _CRTDBG_MAP_ALLOC 
#include <stdlib.h> 
#include <crtdbg.h>  

LRESULT CALLBACK WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam);
const int width = 1920, height = 1080;

/**
 * \brief 启动函数
 * \param hInstance 实例
 * \param hPrevInstance 上一个实例
 * \param lpCmdLine 命令行参数
 * \param nCmdShow 显示方式
 * \return 退出码
 */
int WINAPI WinMain(const HINSTANCE hInstance, const HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {

	// 初始化窗口类
	WNDCLASS wc;
	wc.hInstance = hInstance;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
	wc.hIcon = LoadIcon(nullptr, IDC_ICON);
	wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH));
	wc.lpfnWndProc = static_cast<WNDPROC>(WndProc);
	wc.lpszClassName = "mWndClass";
	wc.lpszMenuName = nullptr;


	// 注册窗口类
	if (!RegisterClass(&wc)) {
		MessageBox(nullptr, "ERROR::REGISTER_CLASS_ERROR", "Error", 0);
		return E_FAIL;
	}

	// 创建窗口
	const auto window = CreateWindow("mWndClass", "DirectX Application", WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, nullptr, nullptr, hInstance, nullptr);
	if (!window) {
		MessageBox(nullptr, "ERROR::CREATE_WINDOW_ERROR", "ERROR", 0);
		return E_FAIL;
	}

	// 显示窗口
	ShowWindow(window, SW_SHOWNORMAL);
	UpdateWindow(window);

	// 配置交换链
	DXGI_SWAP_CHAIN_DESC ds;
	ZeroMemory(&ds, sizeof(DXGI_SWAP_CHAIN_DESC));
	ds.BufferCount = 1;
	ds.BufferDesc.Width = width;
	ds.BufferDesc.Height = height;
	ds.BufferDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
	ds.BufferDesc.RefreshRate.Numerator = 60;
	ds.BufferDesc.RefreshRate.Denominator = 1;
	ds.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	ds.OutputWindow = window;
	ds.SampleDesc.Count = 1;
	ds.SampleDesc.Quality = 0;
	ds.Windowed = TRUE;


	UINT createDeviceFlags = 0;
#ifdef _DEBUG
	createDeviceFlags |= D3D11_CREATE_DEVICE_DEBUG;
#endif

	IDXGISwapChain* pIdxgiSwapChain = nullptr;
	ID3D11RenderTargetView* pD3D11RenderTargetView = nullptr;
	ID3D11Device* pD3D11Device = nullptr;
	ID3D11DeviceContext*    pImmediateContext = nullptr;
	D3D_DRIVER_TYPE         gD3DDriverType = D3D_DRIVER_TYPE_NULL;
	D3D_FEATURE_LEVEL       pD3DFeatureLevel = D3D_FEATURE_LEVEL_11_0;


	D3D_DRIVER_TYPE driverTypes[] =
	{
		D3D_DRIVER_TYPE_HARDWARE,
		D3D_DRIVER_TYPE_WARP,
		D3D_DRIVER_TYPE_REFERENCE,
	};
	const auto numDriverTypes = ARRAYSIZE(driverTypes);

	D3D_FEATURE_LEVEL featureLevels[] =
	{
		D3D_FEATURE_LEVEL_11_0,
		D3D_FEATURE_LEVEL_10_1,
		D3D_FEATURE_LEVEL_10_0,
	};
	const auto numFeatureLevels = ARRAYSIZE(featureLevels);

	HRESULT hr = S_OK;
	for (auto driverType : driverTypes) {
		hr = D3D11CreateDeviceAndSwapChain(
			nullptr,
			driverType,
			nullptr,
			createDeviceFlags,
			featureLevels,
			numFeatureLevels,
			D3D11_SDK_VERSION,
			&ds,
			&pIdxgiSwapChain,
			&pD3D11Device,
			&pD3DFeatureLevel,
			&pImmediateContext);
		if (SUCCEEDED(hr)) {
			break;
		}
	}
	if (FAILED(hr)) {
		return hr;
	}



	// 创建渲染目标视图
	ID3D11Texture2D *pId3D11Texture2D = nullptr;
	hr = pIdxgiSwapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<LPVOID*>(&pId3D11Texture2D));

	if (FAILED(hr)) {
		return hr;
	}

	hr = pD3D11Device->CreateRenderTargetView(pId3D11Texture2D, nullptr, &pD3D11RenderTargetView);
	if (FAILED(hr)) {
		return hr;
	}
	pImmediateContext->OMSetRenderTargets(1, &pD3D11RenderTargetView, nullptr);

	D3D11_VIEWPORT dv;
	dv.Width = width;
	dv.Height = height;
	dv.MaxDepth = 1;
	dv.MinDepth = 0;
	dv.TopLeftX = 0;
	dv.TopLeftY = 0;
	pImmediateContext->RSSetViewports(1, &dv);

	MSG uMsg;
	ZeroMemory(&uMsg, sizeof(MSG));
	while (uMsg.message != WM_QUIT) {
		float clearColor[4] = { 0.1f , 0.2f , 0.5f , 0.3f };
		pImmediateContext->ClearRenderTargetView(pD3D11RenderTargetView, clearColor);
		pIdxgiSwapChain->Present(0, 0);
		if (PeekMessage(&uMsg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}		
	}
	// free
	pId3D11Texture2D->Release();
	pImmediateContext->Release();
	pD3D11Device->Release();
	pD3D11RenderTargetView->Release();
	pIdxgiSwapChain->Release();
	_CrtDumpMemoryLeaks();  
	return 0;

}


LRESULT CALLBACK WndProc(const HWND hWnd, const UINT uMsg, const WPARAM wParam, const LPARAM lParam) {
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