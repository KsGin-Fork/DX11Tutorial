#include <Windows.h>
#include <iostream>
#include <winuser.h>
#include <minwinbase.h>

LRESULT CALLBACK WndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPreInstince, LPSTR lpCmdLine, int nCmdShow) {
	WNDCLASS winClass;
	winClass.style = CS_VREDRAW | CS_HREDRAW;
	winClass.hInstance = hInstance;
	winClass.cbClsExtra = 0;
	winClass.cbWndExtra = 0;
	winClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	winClass.hIcon = LoadIcon(nullptr, IDC_ICON);
	winClass.hbrBackground = static_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH));
	winClass.lpfnWndProc = static_cast<WNDPROC>(WndProc);
	winClass.lpszClassName = "windowClass";
	winClass.lpszMenuName = nullptr;
	if (!RegisterClass(&winClass)) {
		MessageBox(nullptr, "ERROR::WINDOWS_CLASS::REGISTER_ERROR", "REGISTER REEOR", 0);
		return 0;
	}
	const auto windowHandle = CreateWindow("windowClass", "Empty Window", WS_EX_TOPMOST | WS_OVERLAPPEDWINDOW, 0, 0, 1920, 1080, nullptr, nullptr, hInstance, nullptr);
	if (!windowHandle) {
		MessageBox(nullptr, "ERROR::WINDOWS_CREATE::CREATE_ERROR", "CREATE REEOR", 0);
		return 0;
	}

	ShowWindow(windowHandle, SW_SHOWNORMAL);
	UpdateWindow(windowHandle);

	MSG uMsg;
	ZeroMemory(&uMsg, sizeof(MSG));
	uMsg.message = WM_ACTIVATE;
	while (uMsg.message != WM_QUIT) {
		if (PeekMessage(&uMsg, nullptr, 0, 0, PM_REMOVE)) {
			TranslateMessage(&uMsg);
			DispatchMessage(&uMsg);
		}
	}
	return 0;
}


LRESULT CALLBACK WndProc(const HWND hwnd, const UINT uMsg, const WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
	case WM_DESTROY:
		PostQuitMessage(0);
		break;
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			DestroyWindow(hwnd);
		}
		break;
	case WM_LBUTTONDOWN:
		MessageBox(nullptr, "Mouse Left Button Down", "Information", 0);
		break;
	default: break;
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);

}