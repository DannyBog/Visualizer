#ifndef BOG_TRACKBAR_H
#define BOG_TRACKBAR_H

LRESULT CALLBACK TrackbarSubclassProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam,
									  UINT_PTR uIdSubclass, DWORD_PTR dwRefData) {
	LRESULT result = 0;
	
	switch (uMsg) {
		case WM_LBUTTONDOWN:
		case WM_MOUSEMOVE: {
			if (wParam & MK_LBUTTON) {
				POINT pt;
				GetCursorPos(&pt);
				ScreenToClient(hwnd, &pt);
				
				LRESULT min = SendMessage(hwnd, TBM_GETRANGEMIN, 0, 0);
				LRESULT max = SendMessage(hwnd, TBM_GETRANGEMAX, 0, 0);
				
				RECT rc;
				GetClientRect(hwnd, &rc);
				uint32_t width = rc.right - rc.left;
				
				uint32_t currentPos = (uint32_t) SendMessage(hwnd, TBM_GETPOS, 0, 0);
				LRESULT newPos = min + (pt.x * ((max + 1) - min)) / width;
				
				if ((currentPos != newPos) && ((newPos >= min) && (newPos <= max))) {
					SendMessage(hwnd, TBM_SETPOS, 1, (LPARAM) newPos);
					
					WPARAM wp = MAKEWPARAM(TB_THUMBPOSITION, newPos);
					LPARAM lp = (LPARAM) hwnd;
					SendMessage(GetParent(hwnd), WM_HSCROLL, wp, lp);
				}
				
				SetCapture(hwnd);
			}
		} break;
		
		case WM_LBUTTONUP: {
			ReleaseCapture();
		} break;
		
		case WM_NCDESTROY: {
			result = RemoveWindowSubclass(hwnd, TrackbarSubclassProc, uIdSubclass);
		} break;
		
		default: result = DefSubclassProc(hwnd, uMsg, wParam, lParam);
	}
	
	return result;
}

static HWND BOGCreateTrackbar(HWND window, int32_t x, int32_t y, int32_t width, int32_t height,
							  size_t min, size_t max, UINT_PTR id) {
	DWORD styles = WS_CHILD | TBS_AUTOTICKS | WS_VISIBLE;
	HWND hTrackbar = CreateWindow(TRACKBAR_CLASS, 0, styles, x, y, width, height, window, (HMENU) id,
								  0, 0);
	SendMessage(hTrackbar, TBM_SETRANGE, 1, (LPARAM) MAKELONG(min, max));
	
	SetWindowSubclass(hTrackbar, TrackbarSubclassProc, 1, 0);
	
	return hTrackbar;
}

#endif //BOG_TRACKBAR_H
