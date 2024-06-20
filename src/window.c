#include "window.h"

DWORD WINAPI MainThreadProc(LPVOID lpParameter) {
	u64 index = (u64) lpParameter;
	
	switch (mode) {
		case SORT: {
			(sa[index].algorithm)(bars, barsNum);
			sorting = false;
		} break;
			
		case PATHFIND: {
			(pa[index].algorithm)(cells, cellsNum);
			pathfinding = false;
			recalculate = true;
		} break;
	}
	
	return 0;
}

DWORD WINAPI MazeThreadProc(LPVOID lpParameter) {
	GenerateMaze(cells, cellsNum);
	maze = false;
	
	return 0;
}

BOOL CALLBACK EnumChildProc(HWND hwnd, LPARAM lParam) {
	DestroyWindow(hwnd);
	return TRUE;
}

static void SetMode(HWND hWindow, int32_t ww, int32_t wh) {
	switch (mode) {
		case SORT: {
			barsNum = 4;
			
			BOGCreateTrackbar(hWindow, 0, 0, ww, 30, 1, 100, ID_TRACKBAR);
			HWND button = BOGCreateButton(hWindow, "SORT", (ww - 50) / 2, (wh - 50) / 2, 50, 50,
										  ID_SORT);
			EnableWindow(button, 0);
			
			seed = BOGGetSystemTime();
			for (size_t i = 0; i < barsNum; ++i) {
				u32 num = (u32) (BOGRandom(&seed) % 151) + 50;
				u32 color = INITIAL;
				
				bar b = {num, color};
				bars[i] = b;
			}
			
			char *title = "Choose a sorting algorithm...";
			char *list[MAX_SORTING_ALGORITHMS + 1];
			for (size_t i = 0; i < MAX_SORTING_ALGORITHMS; ++i) {
				list[i] = sa[i].name;
			}
			list[MAX_SORTING_ALGORITHMS] = 0;
			
			HDC hdc = GetDC(hWindow);
			TEXTMETRIC text;
			GetTextMetrics(hdc, &text);
			u32 comboBoxHeight = MulDiv(text.tmHeight, 9, 8) * MAX_SORTING_ALGORITHMS;
			BOGCreateComboBox(hWindow, (ww - 220) / 2, (wh - comboBoxHeight) / 3, 220, 0,
							  ID_SORTING_ALGORITHMS, title, list);
		} break;
			
		case PATHFIND: {
			u32 cellSize = 20;
			u32 yOffset = cellSize * 3;
			u32 sw = (u32) (ww / cellSize);
			u32 sh = (u32) ((wh - yOffset) / cellSize);
			cellsNum = (u32) (sw * sh);
			
			for (u32 i = 0; i < cellsNum; ++i) {
				cell c = {
					.row = i / sw,
					.col = i % sw,
					.size = cellSize,
					.distance = UINT_MAX,
					.color = 0
				};
				
				cells[i] = c;
			}
			
			cells[(sw / 3) + (sh / 2) * sw].color = START_NODE;
			cells[(sw / 3) + (sh / 2) * sw].distance = 0;
			cells[(2 * sw / 3) + (sh / 2) * sw].color = TARGET_NODE;
			
			char *title = "Choose a pathfinding algorithm...";
			char *list[MAX_PATHFINDING_ALGORITHMS + 1];
			for (size_t i = 0; i < MAX_PATHFINDING_ALGORITHMS; ++i) {
				list[i] = pa[i].name;
			}
			list[MAX_PATHFINDING_ALGORITHMS] = 0;
			
			HDC hdc = GetDC(hWindow);
			TEXTMETRIC text;
			GetTextMetrics(hdc, &text);
			BOGCreateComboBox(hWindow, (ww - 250) / 2, 0, 250, 0,
							  ID_PATHFINDING_ALGORITHMS, title, list);
			
			u32 comboBoxHeight = MulDiv(text.tmHeight, 9, 6);
			BOGCreateButton(hWindow, "RESET",
							(ww - 250) / 2, comboBoxHeight, 83, 34, ID_RESET);
			BOGCreateButton(hWindow, "MAZE", (ww - 250) / 2 + 83, comboBoxHeight, 83, 34, ID_MAZE);
			HWND button = BOGCreateButton(hWindow, "FIND", (ww - 250) / 2 + 166, comboBoxHeight,
										  83, 34, ID_FIND);
			EnableWindow(button, 0);
		} break;
	}
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	LRESULT result = 0;
	
	static bog_gui *bg;
	static HMENU hPopupMenu;
	static u32 ww, wh;
	static cell *startCell, previousCell;
	static HANDLE hThread;
	static HBITMAP bitmap;
	
	switch (msg) {
		case WM_CREATE: {
			CREATESTRUCT *cs = (CREATESTRUCT *) lParam;
			bg = (bog_gui *) cs->lpCreateParams;
			
			BufferedPaintUnInit();
			bitmap = LoadBitmap(GetModuleHandle(0), MAKEINTRESOURCE(2));
			
			hPopupMenu = CreatePopupMenu();
			AppendMenu(hPopupMenu, MF_STRING, CMD_SORT, "Sorting Visualizer");
			AppendMenu(hPopupMenu, MF_STRING, CMD_PATH, "Pathfinding Visualizer");
			
			RECT rect;
			GetClientRect(hwnd, &rect);
			ww = rect.right - rect.left;
			wh = rect.bottom - rect.top;
			
			SetTimer(hwnd, ID_TIMER, 0, 0);
		} break;
		
		case WM_COMMAND: {
			switch (mode) {
				case SORT: {
					switch (HIWORD(wParam)) {
						case CBN_SELCHANGE: {
							if (!sorting) {
								HWND button = GetDlgItem(hwnd, ID_SORT);
								EnableWindow(button, 1);
								
								HWND trackbar = GetDlgItem(hwnd, ID_TRACKBAR);
								u32 pos = (u32) SendMessage(trackbar, TBM_GETPOS, 0, 0);
								
								barsNum = 2 + (pos * 2);
								
								for (size_t i = 0; i < barsNum; ++i) {
									u32 num = (u32) (BOGRandom(&seed) % 151) + 50;
									u32 color = INITIAL;
									
									bar b = {num, color};
									bars[i] = b;
								}
							}
						} break;
						
						case BN_CLICKED: {
							switch (LOWORD(wParam)) {
								case ID_SORT: {
									HWND button = GetDlgItem(hwnd, ID_SORT);
									EnableWindow(button, 0);
								
									sorting = true;
								
									HWND comboBox = GetDlgItem(hwnd, ID_SORTING_ALGORITHMS);
									LRESULT index = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
									hThread = CreateThread(0, 0, MainThreadProc, (void *) index, 0, 0);
								} break;
							}
						} break;
					}
				} break;
					
				case PATHFIND: {
					switch (HIWORD(wParam)) {
						case CBN_SELCHANGE: {
							if (!pathfinding && !maze) {
								HWND findButton = GetDlgItem(hwnd, ID_FIND);
								HWND mazeButton = GetDlgItem(hwnd, ID_MAZE);
								EnableWindow(findButton, 1);
								EnableWindow(mazeButton, 1);
								
								recalculate = false;
								
								for (u32 i = 0; i < cellsNum; ++i) {
									u32 color = cells[i].color;
									cells[i].distance = UINT_MAX;
									if (color == START_NODE) cells[i].distance = 0;
									
									cells[i].visited = false;
									
									if (color != WALL &&
										color != START_NODE && color != TARGET_NODE) cells[i].color = 0;
								}
							}
						} break;
						
						case BN_CLICKED: {
							switch (LOWORD(wParam)) {
								case ID_RESET: {
									if (pathfinding || recalculate || maze) {
										pathfinding = false;
										maze = false;
										
										WaitForSingleObject(hThread, INFINITE);
										CloseHandle(hThread);
										recalculate = false;
									}
									
									HWND comboBox = GetDlgItem(hwnd, ID_PATHFINDING_ALGORITHMS);
									LRESULT index = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
									
									if (index >= 0) {
										HWND findButton = GetDlgItem(hwnd, ID_FIND);
										EnableWindow(findButton, 1);
									}
									
									HWND mazeButton = GetDlgItem(hwnd, ID_MAZE);
									EnableWindow(mazeButton, 1);
									
									for (u32 i = 0; i < cellsNum; ++i) {
										u32 color = cells[i].color;
										cells[i].distance = UINT_MAX;
										if (color == START_NODE) cells[i].distance = 0;
										
										cells[i].visited = false;
										
										if (color != START_NODE &&
											color != TARGET_NODE) cells[i].color = 0;
									}
								} break;
									
								case ID_MAZE: {
									HWND findButton = GetDlgItem(hwnd, ID_FIND);
									HWND mazeButton = GetDlgItem(hwnd, ID_MAZE);
									EnableWindow(findButton, 0);
									EnableWindow(mazeButton, 0);
									
									for (u32 i = 0; i < cellsNum; ++i) {
										u32 color = cells[i].color;
										if (color == WALL) cells[i].color = 0;
									}
									
									maze = true;
									
									hThread = CreateThread(0, 0, MazeThreadProc, 0, 0, 0);
								} break;
								
								case ID_FIND: {
									HWND findButton = GetDlgItem(hwnd, ID_FIND);
									HWND mazeButton = GetDlgItem(hwnd, ID_MAZE);
									EnableWindow(findButton, 0);
									EnableWindow(mazeButton, 0);
									
									pathfinding = true;
									recalculate = false;
								
									HWND comboBox = GetDlgItem(hwnd, ID_PATHFINDING_ALGORITHMS);
									LRESULT index = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
									hThread = CreateThread(0, 0, MainThreadProc, (void *) index, 0, 0);
								} break;
							}
						} break;
					}
				} break;
			}
			
			switch (LOWORD(wParam)) {
				case CMD_SORT: {
					u32 state = GetMenuState(hPopupMenu, CMD_SORT, MF_BYCOMMAND);
					if (state == MF_UNCHECKED) {
						CheckMenuItem(hPopupMenu, CMD_SORT, MF_CHECKED);
						CheckMenuItem(hPopupMenu, CMD_PATH, MF_UNCHECKED);
						
						mode = SORT;
						if (pathfinding || maze) {
							pathfinding = false;
							maze = false;
							WaitForSingleObject(hThread, INFINITE);
							CloseHandle(hThread);
							recalculate = false;
						}
						EnumChildWindows(hwnd, EnumChildProc, 0);
						SetMode(hwnd, ww, wh);
					}
				} break;
					
				case CMD_PATH: {
					u32 state = GetMenuState(hPopupMenu, CMD_PATH, MF_BYCOMMAND);
					if (state == MF_UNCHECKED) {
						CheckMenuItem(hPopupMenu, CMD_PATH, MF_CHECKED);
						CheckMenuItem(hPopupMenu, CMD_SORT, MF_UNCHECKED);
						
						mode = PATHFIND;
						if (sorting) {
							sorting = false;
							WaitForSingleObject(hThread, INFINITE);
							CloseHandle(hThread);
						}
						EnumChildWindows(hwnd, EnumChildProc, 0);
						SetMode(hwnd, ww, wh);
					}
				} break;
			}
		}
			
		case WM_MOUSEWHEEL: {
			HWND trackbar = GetDlgItem(hwnd, ID_TRACKBAR);
			SendMessage(trackbar, msg, wParam, lParam);
		} break;
			
		case WM_RBUTTONUP: {
			POINT pt;
			GetCursorPos(&pt);
			TrackPopupMenu(hPopupMenu, TPM_LEFTALIGN | TPM_LEFTBUTTON, pt.x, pt.y, 0, hwnd, 0);
		} break;
		
		case WM_LBUTTONDOWN: {
			if (mode == PATHFIND && !pathfinding && !maze) {
				u32 x = LOWORD(lParam);
				u32 y = HIWORD(lParam);
				POINT pos = {x, y};
				
				for (u32 i = 0; i < cellsNum; ++i) {
					u32 yOffset = cells[i].size * 3;
					u32 row = cells[i].row * cells[i].size + yOffset - 1;
					u32 col = cells[i].col * cells[i].size + 2;
					
					RECT rect = {
						.left = col,
						.top = row,
						.right = col + cells[i].size,
						.bottom = row + cells[i].size
					};
					
					if (PtInRect(&rect, pos)) {
						u32 color = cells[i].color;
						
						if ((color != START_NODE) && (color != TARGET_NODE)) {
							if (color != WALL) {
								cells[i].color = WALL;
							} else {
								cells[i].color = 0;
							}
						}
						
						if (recalculate) {
							for (u32 j = 0; j < cellsNum; ++j) {
								color = cells[j].color;
								cells[j].distance = UINT_MAX;
								cells[j].visited = false;
								if (color == INITIAL ||
									color == SORTING) cells[j].color = 0;
								if (color == START_NODE) cells[j].distance = 0;
							}
							
							HWND comboBox = GetDlgItem(hwnd, ID_PATHFINDING_ALGORITHMS);
							LRESULT index = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
							hThread = CreateThread(0, 0, MainThreadProc, (void *) index, 0, 0);
							WaitForSingleObject(hThread, INFINITE);
							CloseHandle(hThread);
						}
						
						startCell = &cells[i];
						previousCell = cells[i];
						break;
					}
				}
			}
		} break;
			
		case WM_LBUTTONUP: {
			ClipCursor(0);
		} break;
		
		case WM_MOUSEMOVE: {
			if ((mode == PATHFIND) && (wParam & MK_LBUTTON) && !pathfinding && !maze) {
				u32 x = LOWORD(lParam);
				u32 y = HIWORD(lParam);
				POINT pos = {x, y};
				
				u32 yOffset = previousCell.size * 3;
				u32 row = previousCell.row * previousCell.size + yOffset - 1;
				u32 col = previousCell.col * previousCell.size + 2;
				
				RECT rect;
				GetClientRect(hwnd, &rect);
				MapWindowPoints(hwnd, 0, (LPPOINT) &rect, 2);
				rect.left += 2;
				rect.top += yOffset - 1;
				rect.right -= 2;
				rect.bottom -= 2;
				ClipCursor(&rect);
				
				RECT previousRect = {
					.left = col,
					.top = row,
					.right = col + previousCell.size,
					.bottom = row + previousCell.size
				};
				
				for (u32 i = 0; i < cellsNum; ++i) {
					yOffset = cells[i].size * 3;
					row = cells[i].row * cells[i].size + yOffset - 1;
					col = cells[i].col * cells[i].size + 2;
					
					RECT currentRect = {
						.left = col,
						.top = row,
						.right = col + cells[i].size,
						.bottom = row + cells[i].size
					};
					
					if (!EqualRect(&currentRect, &previousRect) && PtInRect(&currentRect, pos)) {
						u32 currentColor = cells[i].color;
						u32 startColor = (startCell) ? startCell->color : currentColor;
						
						if ((currentColor != START_NODE) && (currentColor != TARGET_NODE)) {
							switch (startColor) {
								case START_NODE:
								case TARGET_NODE: {
									if (currentColor != WALL) {
										BOG_SWAP(&cells[i].color, &startCell->color);
										BOG_SWAP(&cells[i].distance, &startCell->distance);
										startCell = &cells[i];
									}
								} break;
								
								case WALL: {
									cells[i].color = WALL;
								} break;
								
								default: {
									if (currentColor == WALL) cells[i].color = 0;
								} break;
							}
						}
						
						if (recalculate) {
							for (u32 j = 0; j < cellsNum; ++j) {
								u32 color = cells[j].color;
								cells[j].distance = UINT_MAX;
								cells[j].visited = false;
								if (color == INITIAL ||
									color == SORTING) cells[j].color = 0;
								if (color == START_NODE) cells[j].distance = 0;
							}
							
							HWND comboBox = GetDlgItem(hwnd, ID_PATHFINDING_ALGORITHMS);
							LRESULT index = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
							hThread = CreateThread(0, 0, MainThreadProc, (void *) index, 0, 0);
							WaitForSingleObject(hThread, INFINITE);
							CloseHandle(hThread);
						}
						
						previousCell = cells[i];
					}
				}
			}
		} break;
			
		case WM_HSCROLL: {
			switch (LOWORD(wParam)) {
				case TB_BOTTOM:
				case TB_LINEDOWN:
				case TB_LINEUP:
				case TB_PAGEDOWN:
				case TB_PAGEUP:
				case TB_TOP:
				case TB_THUMBPOSITION: {
					if (sorting) {
						sorting = false;
						WaitForSingleObject(hThread, INFINITE);
						CloseHandle(hThread);
					}
					
					HWND comboBox = GetDlgItem(hwnd, ID_SORTING_ALGORITHMS);
					LRESULT index = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
					
					if (index >= 0) {
						HWND button = GetDlgItem(hwnd, ID_SORT);
						EnableWindow(button, 1);
					}
					
					HWND trackbar = GetDlgItem(hwnd, ID_TRACKBAR);
					u32 pos = (u32) SendMessage(trackbar, TBM_GETPOS, 0, 0);
					
					barsNum = 2 + (pos * 2);
					
					for (size_t i = 0; i < barsNum; ++i) {
						u32 num = (u32) (BOGRandom(&seed) % 151) + 50;
						u32 color = INITIAL;
						
						bar b = {num, color};
						bars[i] = b;
					}
				}  break;
			}
		} break;
		
		case WM_CTLCOLORSTATIC: {
			result = (LRESULT) GetStockObject(BLACK_BRUSH);
		} break;
			
		case WM_ERASEBKGND: {
			result = (LRESULT) 1;
		} break;
			
		case WM_PAINT: {
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hwnd, &ps);
			HPAINTBUFFER hpb = BeginBufferedPaint(ps.hdc, &ps.rcPaint, BPBF_COMPATIBLEBITMAP, 0, &hdc);
			
			if (hpb) {
				SetBkMode(hdc, TRANSPARENT);
				FillRect(hdc, &ps.rcPaint, GetStockObject(BLACK_BRUSH));
				
				switch (mode) {
					case SORT: {
						f32 rw = (f32) ww / (f32) barsNum;
						bool renderNums = true;
						for (udm i = 0; i < barsNum; ++i) {
							u32 num = bars[i].num;
				
							BOGDrawRectangle(hdc, (s32) (rw * i), wh - num, (s32) rw, wh,
											 bars[i].color, 0);
				
							s32 length = 0;
							if (num < 0) length++;
							while (num) {
								num /= 10;
								length++;
							}
				
							char text[MAX_DIGITS + 1];
							BOGIntToString(bars[i].num, text, 10);
				
							SIZE textSize;
							GetTextExtentPoint32(hdc, text, length, &textSize);
				
							if (textSize.cx >= rw - 2) renderNums = false;
						}
			
						if (renderNums) {
							for (udm i = 0; i < barsNum; ++i) {
								u32 num = bars[i].num;
					
								s32 length = 0;
								if (num < 0) length++;
								while (num) {
									num /= 10;
									length++;
								}
						
								char text[MAX_DIGITS + 1];
								BOGIntToString(bars[i].num, text, 10);
					
								SIZE textSize;
								GetTextExtentPoint32(hdc, text, length, &textSize);
					
								TextOut(hdc, (s32) ((rw * i) + (rw - textSize.cx) / 2),
										wh - textSize.cy, text, length);
							}
						}
					} break;
					
					case PATHFIND: {
						for (u32 i = 0; i < cellsNum; ++i) {
							u32 yOffset = cells[i].size * 3;
							u32 row = cells[i].row * cells[i].size + yOffset - 1;
							u32 col = cells[i].col * cells[i].size + 2;
							
							BOGDrawRectangle(hdc, col, row, cells[i].size, cells[i].size,
											 cells[i].color, GRID);
						}
					} break;
						
					default: {
						HDC hdcMem = CreateCompatibleDC(hdc);
						SelectObject(hdcMem, bitmap);
						
						BITMAP bm;
						GetObject(bitmap, sizeof(BITMAP), &bm);
						BitBlt(hdc, (ww - bm.bmWidth) / 2, (wh - bm.bmHeight) / 2,
							   bm.bmWidth, bm.bmHeight, hdcMem, 0, 0, SRCCOPY);
						
						DeleteDC(hdcMem);
					}
				}
				
				EndBufferedPaint(hpb, 1);
			}

			EndPaint(hwnd, &ps);
		} break;
			
		case WM_TIMER: {
			if (mode == PATHFIND) {
				HWND comboBox = GetDlgItem(hwnd, ID_PATHFINDING_ALGORITHMS);
				LRESULT index = SendMessage(comboBox, CB_GETCURSEL, 0, 0);
				
				if ((index >= 0) && !pathfinding && !recalculate && !maze) {
					HWND button = GetDlgItem(hwnd, ID_FIND);
					EnableWindow(button, 1);
				}
			}
			
			InvalidateRect(hwnd, 0, 1);
			UpdateWindow(hwnd);
		} break;
			
		case WM_DESTROY: {
			PostQuitMessage(0);
			bg->running = false;
		} break;
		
		default: result = DefWindowProc(hwnd, msg, wParam, lParam);
	}
	
	return result;
}
