#ifndef BOG_COMBO_BOX_H
#define BOG_COMBO_BOX_H

LRESULT CALLBACK EditSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
								  UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	LRESULT result = 0;
	
	switch (uMsg) {
		case WM_GETDLGCODE: {
			result = DefSubclassProc(hwnd, uMsg, wParam, lParam) & ~DLGC_WANTCHARS;
		} break;
		
		case WM_SETFOCUS: {
			result = DefSubclassProc(hwnd, uMsg, wParam, lParam);
			HideCaret(hwnd);
		} break;
		
		case WM_NCDESTROY: {
			result = RemoveWindowSubclass(hwnd, EditSubclassProc, uIdSubclass);
		} break;
		
		default: result = DefSubclassProc(hwnd, uMsg, wParam, lParam);
	}
	
	return result;
}

static HWND BOGCreateComboBox(HWND window, int32_t x, int32_t y, int32_t width, int32_t height,
							  UINT_PTR id, char *title, char **list) {
	DWORD styles = WS_CHILD | CBS_DROPDOWN | WS_VSCROLL | WS_VISIBLE;
	HWND hComboBox = CreateWindow(WC_COMBOBOX, 0, styles, x, y, width, height, window, (HMENU) id,
								  0, 0);
	
	HWND edit = GetWindow(hComboBox, GW_CHILD);
	SetWindowText(edit, title);
	
	char **items = list;
	while (*items) {
		SendMessage(hComboBox, CB_ADDSTRING, 0, (LPARAM) *items++); 
	}
	
	COMBOBOXINFO info = {0};
	info.cbSize = sizeof(COMBOBOXINFO);
	GetComboBoxInfo(hComboBox, &info);
	HWND hEdit = info.hwndItem;
	
	SetWindowSubclass(hEdit, EditSubclassProc, 1, 0);
	
	return hComboBox;
}

#endif //BOG_COMBO_BOX_H
