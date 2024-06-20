#include "window.c"

#pragma comment(lib, "kernel32.lib")
#pragma comment(lib, "user32.lib")
#pragma comment(lib, "gdi32.lib")
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "uxtheme.lib")

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#ifdef _DEBUG
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR pCmdLine, int nCmdShow) {
#else
void __stdcall WinMainCRTStartup() {
#endif
	bog_gui bg = {0};
	bg.hInstance = GetModuleHandleW(0);
	bg.proc = WindowProc;
	
	HICON icon = LoadIcon(GetModuleHandle(0), MAKEINTRESOURCE(1));
	BOGCreateWindow(&bg, "Visualizer", 1280, 720, icon, false);
	BOGCenterWindow(bg.window);
	
	while (bg.running) {
		BOGUpdateWindow(bg.window);
	}
	
	ExitProcess(0);
}
