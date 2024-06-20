#ifndef BOG_GUI_H
#define BOG_GUI_H

#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <uxtheme.h>
#include <commctrl.h>
#include <stdint.h>
#include <stdbool.h>

typedef struct {
	HINSTANCE hInstance;
	WNDPROC proc;
	HWND window;
	bool running;
} bog_gui;

static void BOGToggleFullScreen(HWND window, RECT *rect) {
	int32_t x, y, ww, wh;
	
	if (rect) {
		x = rect->left;
		y = rect->top;
		ww = rect->right - rect->left;
		wh = rect->bottom - rect->top;
		
		SetWindowLongPtr(window, GWL_STYLE, WS_OVERLAPPEDWINDOW | WS_VISIBLE);
		SetWindowPos(window, HWND_NOTOPMOST, x, y, ww, wh, SWP_SHOWWINDOW);
	} else {
		HMONITOR hmon = MonitorFromWindow(window, MONITOR_DEFAULTTONEAREST);
		MONITORINFO mi = {sizeof(mi)};
		GetMonitorInfo(hmon, &mi);
		
		x = mi.rcMonitor.left;
		y = mi.rcMonitor.top;
		ww = mi.rcMonitor.right - mi.rcMonitor.left;
		wh = mi.rcMonitor.bottom - mi.rcMonitor.top;
		
		SetWindowLongPtr(window, GWL_STYLE, WS_POPUP | WS_VISIBLE);
		SetWindowPos(window, HWND_TOP, x, y, ww, wh, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
	}
}

static void BOGScaleControls(HWND child, POINT *offset) {
	RECT rect;
	GetWindowRect(child, &rect);
	MapWindowPoints(HWND_DESKTOP, GetParent(child), (LPPOINT) &rect, 2);
	
	int32_t x = rect.left;
	int32_t y = rect.top;
	int32_t ww = (rect.right - rect.left) + offset->x;
	int32_t wh = (rect.bottom - rect.top) + offset->y;
	
	SetWindowPos(child, 0, x, y, ww, wh, SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
}

static void BOGDrawRectangle(HDC hdc, int32_t x, int32_t y, int32_t width, int32_t height,
							 COLORREF fill, COLORREF outline) {
	HBRUSH brush = CreateSolidBrush(fill);
	HPEN pen = CreatePen(PS_SOLID, 1, outline);
	
	SelectObject(hdc, brush);
	SelectObject(hdc, pen);
	
	Rectangle(hdc, x, y, x + width, y + height);
	
	DeleteObject(brush);
	DeleteObject(pen);
}

static void BOGCenterWindow(HWND window) {
	RECT rect;
	GetWindowRect(window, &rect);
	
	int32_t ww = rect.right - rect.left;
	int32_t wh = rect.bottom - rect.top;
	int32_t sw = GetSystemMetrics(SM_CXSCREEN);
	int32_t sh = GetSystemMetrics(SM_CYSCREEN);
	
	SetWindowPos(window, HWND_TOP, (sw - ww) / 2, (sh - wh) / 2, 0, 0, SWP_NOSIZE);
}

static void BOGCreateWindow(bog_gui *bg, char *title, uint32_t width, uint32_t height,
							HICON icon, bool resizeable) {
	WNDCLASS wc = {
		.style = CS_OWNDC | CS_HREDRAW | CS_VREDRAW,
		.lpfnWndProc = bg->proc,
		.hInstance = bg->hInstance,
		.hIcon = icon,
		.hCursor = LoadCursor(0, IDC_ARROW),
		.hbrBackground = GetStockObject(GRAY_BRUSH),
		.lpszClassName = title
	};
	
	RegisterClass(&wc);
	
	DWORD styles = 0;
	if (resizeable) {
		styles = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_VISIBLE;
	} else {
		styles = (WS_OVERLAPPEDWINDOW ^ WS_THICKFRAME ^ WS_MAXIMIZEBOX) | WS_CLIPCHILDREN | WS_VISIBLE;
	}
	
	bg->window = CreateWindow(wc.lpszClassName, title, styles, CW_USEDEFAULT, CW_USEDEFAULT,
							  width, height, 0, 0, bg->hInstance, bg);
	bg->running = true;
}

static void BOGUpdateWindow(HWND window) {
	MSG msg;
	if (GetMessage(&msg, 0, 0, 0)) {
		bool isDialog = IsDialogMessage(window, &msg);
		if (!isDialog) {
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}
}

#endif //BOG_GUI_H
