#ifndef BOG_BUTTON_H
#define BOG_BUTTON_H

static HWND BOGCreateButton(HWND window, char *title, int32_t x, int32_t y,
							int32_t width, int32_t height, UINT_PTR id) {
	DWORD styles = WS_CHILD | WS_VISIBLE;
	HWND hButton = CreateWindow(WC_BUTTON, title, styles, x, y, width, height, window, (HMENU) id,
								0, 0);
	
	return hButton;
}

#endif //BOG_BUTTON_H
