#pragma once

#include <wrl.h>
#include <d3d12.h>
#include <dxgi1_6.h>
#include <D3Dcompiler.h>
#include <DirectXMath.h>
#include <string>
#include <shellapi.h>
#include <Windows.h>
#include <d3d12.h>
#include "D3DInstance.h"


class WinApplication
{
public:
	WinApplication();
	WinApplication(UINT width, UINT height);
	static int Run(HINSTANCE hInstance, D3DInstance* d3d, int width, int height);
	static LRESULT CALLBACK WNDProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
	static HWND GetHwnd() { return m_hwnd; }
private:
	static HWND m_hwnd;
};