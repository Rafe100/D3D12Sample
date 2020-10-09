#include "WinApplication.h"

HWND WinApplication::m_hwnd = nullptr;
WinApplication::WinApplication(UINT width, UINT height) {
}

WinApplication::WinApplication() {
}

int WinApplication::Run(HINSTANCE hInstance, D3DInstance* d3d, int width, int height) {
	LPCSTR wName = "direct";
	LPCSTR wTitle = "我的应用程序";
	WNDCLASS wc = { sizeof(WNDCLASS) };
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WinApplication::WNDProc;
	wc.lpszClassName = L"direct";
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDC_HELP);
	wc.hCursor = LoadCursor(hInstance, IDC_IBEAM);
	if (!RegisterClass(&wc)) {
		//字符串前面加L表示转换成宽字符,就是每个字符占用两个字节。  
		//例：strlen("asd") = 3;
		//strlen(L"asd") = 6;
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return 0;
	}
	m_hwnd = CreateWindow(
		L"direct",           //类名，要和刚才注册的一致  
		L"我的应用程序",  //窗口标题文字  
		WS_OVERLAPPEDWINDOW, //窗口外观样式  
		38,                 //窗口相对于父级的X坐标  
		20,                 //窗口相对于父级的Y坐标  
		width,                //窗口的宽度  
		height,                //窗口的高度  
		NULL,               //没有父窗口，为NULL  
		NULL,               //没有菜单，为NULL  
		hInstance,          //当前应用程序的实例句柄  
		NULL);              //没有附加数据，为NULL  
	if (m_hwnd == NULL) {
		printf("hwnd is null");
		return 0;
	}
	d3d->OnInit(width, height);
	ShowWindow(m_hwnd, SW_SHOW);
	UpdateWindow(m_hwnd);

	MSG msg;
	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
		d3d->Draw();
	}

	return 0;
}

LRESULT CALLBACK WinApplication::WNDProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}
	}
	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}