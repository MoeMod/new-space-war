#include "MetaHook/metahook.h"
#include "DeviceListener.h"

#include "util/Encode.h"

const char *g_szWindowName = "New Space War";

void ProcessWindow(HWND hwnd)
{
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	// Create Surface Dial Interface.
	DeviceListener *listener = new DeviceListener(console);
	listener->Init(hwnd);
}

HWND(WINAPI *g_pfnCreateWindowExA)(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = NULL;
HWND WINAPI NewCreateWindowExA(DWORD dwExStyle, LPCSTR lpClassName, LPCSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	HWND hwnd = g_pfnCreateWindowExA(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	// unicode project, no need
	if (!strcmp(lpWindowName, g_szWindowName))
		ProcessWindow(hwnd);

	return hwnd;
}

HWND(WINAPI *g_pfnCreateWindowExW)(DWORD dwExStyle, LPWSTR lpClassName, LPWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam) = NULL;
HWND WINAPI NewCreateWindowExW(DWORD dwExStyle, LPWSTR lpClassName, LPWSTR lpWindowName, DWORD dwStyle, int X, int Y, int nWidth, int nHeight, HWND hWndParent, HMENU hMenu, HINSTANCE hInstance, LPVOID lpParam)
{
	HWND hwnd = g_pfnCreateWindowExW(dwExStyle, lpClassName, lpWindowName, dwStyle, X, Y, nWidth, nHeight, hWndParent, hMenu, hInstance, lpParam);

	if (!strcmp(UnicodeToUTF8(lpWindowName), g_szWindowName))
		ProcessWindow(hwnd);

	return hwnd;
}

void Window_Init()
{
	// since cocos doesnt provide hwnd, only this way to get it
	g_pMetaHookAPI->InlineHook(&CreateWindowExA, &NewCreateWindowExA, (void *&)g_pfnCreateWindowExA);
	g_pMetaHookAPI->InlineHook(&CreateWindowExW, &NewCreateWindowExW, (void *&)g_pfnCreateWindowExW);
}