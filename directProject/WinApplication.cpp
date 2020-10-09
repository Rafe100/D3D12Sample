#include "WinApplication.h"

HWND WinApplication::m_hwnd = nullptr;
WinApplication::WinApplication(UINT width, UINT height) {
}

WinApplication::WinApplication() {
}

int WinApplication::Run(HINSTANCE hInstance, D3DInstance* d3d, int width, int height) {
	LPCSTR wName = "direct";
	LPCSTR wTitle = "�ҵ�Ӧ�ó���";
	WNDCLASS wc = { sizeof(WNDCLASS) };
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;
	wc.lpfnWndProc = WinApplication::WNDProc;
	wc.lpszClassName = L"direct";
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(hInstance, IDC_HELP);
	wc.hCursor = LoadCursor(hInstance, IDC_IBEAM);
	if (!RegisterClass(&wc)) {
		//�ַ���ǰ���L��ʾת���ɿ��ַ�,����ÿ���ַ�ռ�������ֽڡ�  
		//����strlen("asd") = 3;
		//strlen(L"asd") = 6;
		MessageBox(0, L"RegisterClass Failed", 0, 0);
		return 0;
	}
	m_hwnd = CreateWindow(
		L"direct",           //������Ҫ�͸ղ�ע���һ��  
		L"�ҵ�Ӧ�ó���",  //���ڱ�������  
		WS_OVERLAPPEDWINDOW, //���������ʽ  
		38,                 //��������ڸ�����X����  
		20,                 //��������ڸ�����Y����  
		width,                //���ڵĿ��  
		height,                //���ڵĸ߶�  
		NULL,               //û�и����ڣ�ΪNULL  
		NULL,               //û�в˵���ΪNULL  
		hInstance,          //��ǰӦ�ó����ʵ�����  
		NULL);              //û�и������ݣ�ΪNULL  
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