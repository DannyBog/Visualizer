#ifndef WINDOW_H
#define WINDOW_H

#include "bog\bog_types.h"
#include "bog\bog_gui.h"
#include "bog\bog_string.h"
#include "bog\bog_utils.h"
#include "sort.c"
#include "pathfind.c"
#include "maze.c"

#include "bog\controls\bog_trackbar.h"
#include "bog\controls\bog_combo_box.h"
#include "bog\controls\bog_button.h"

#define ID_TRACKBAR					(WM_USER + 0)
#define ID_SORT						(WM_USER + 1)
#define ID_SORTING_ALGORITHMS		(WM_USER + 2)

#define ID_RESET					(WM_USER + 3)
#define ID_MAZE						(WM_USER + 4)
#define ID_FIND						(WM_USER + 5)
#define ID_PATHFINDING_ALGORITHMS	(WM_USER + 6)

#define ID_TIMER					(WM_USER + 7)

#define CMD_SORT					(WM_USER + 7)
#define CMD_PATH					(WM_USER + 8)

#define MAX_DIGITS	3
#define MAX_SORTING_ALGORITHMS 6
#define MAX_PATHFINDING_ALGORITHMS 4

static bar bars[MAX_BARS];
static u32 barsNum;
static u32 mode;
static u64 seed;

static cell cells[MAX_CELLS];
static u32 cellsNum;

typedef enum {
	SORT = 1,
	PATHFIND = 2
} modes;

typedef struct {
	char *name;
	void (*algorithm)(bar *, u32);
} sorting_algorithms;

typedef struct {
	char *name;
	void (*algorithm)(cell *, u32);
} pathfinding_algorithms;

static sorting_algorithms sa[] = {
	"Bubble Sort", BubbleSort,
	"Insertion Sort", InsertionSort,
	"Selection Sort", SelectionSort,
	"Heap Sort", HeapSort,
	"Merge Sort", MergeSort,
	"Quick Sort", QuickSort
};

static pathfinding_algorithms pa[] = {
	"A* Search", AStar,
	"Dijkstra's Algorithm", Dijkstra,
	"BFS", BFS,
	"DFS", DFS
};

DWORD WINAPI ThreadProc(LPVOID lpParameter);
LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

#endif //WINDOW_H
