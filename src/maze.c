#include "maze.h"

static void CravePassage(cell * cells, u32 row, u32 col, u32 width, u32 height, u64 seed) {
	u32 directions[4] = {UP, LEFT, RIGHT, DOWN};
	
	for (u32 i = 0; i < BOG_ARR_LEN(directions); ++i) {
		u32 j = (u32) (BOGRandom(&seed) % (BOG_ARR_LEN(directions) - i)) + i;
		u32 temp = directions[j];
		directions[j] = directions[i];
		directions[i] = temp;
	}
	
	for (u32 i = 0; i < BOG_ARR_LEN(directions); ++i) {
		switch (directions[i]) {
			case UP: {
				if ((row > 1) && !cells[col + (row - 2) * width].color) {
					for (u32 j = 0; j < 3; ++j) {
						u32 color = cells[col + (row - j) * width].color;
						if (!color) cells[col + (row - j) * width].color = WALL;
						if (maze) Sleep(DELAYY);
					}
					CravePassage(cells, row - 2, col, width, height, seed);
				}
			} break;
			
			case RIGHT: {
				if ((col < width - 2) && !cells[(col + 2) + row * width].color) {
					for (u32 j = 0; j < 3; ++j) {
						u32 color = cells[(col + j) + row * width].color;
						if (!color) cells[(col + j) + row * width].color = WALL;
						if (maze) Sleep(DELAYY);
					}
					CravePassage(cells, row, col + 2, width, height, seed);
				}
			} break;
				
			
			case LEFT: {
				if ((col > 1) && !cells[(col - 2) + row * width].color) {
					for (u32 j = 0; j < 3; ++j) {
						u32 color = cells[(col - j) + row * width].color;
						if (!color) cells[(col - j) + row * width].color = WALL;
						if (maze) Sleep(DELAYY);
					}
					CravePassage(cells, row, col - 2, width, height, seed);
				}
			} break;
				
			case DOWN: {
				if ((row < height - 2) && !cells[col + (row + 2) * width].color) {
					for (u32 j = 0; j < 3; ++j) {
						u32 color = cells[col + (row + j) * width].color;
						if (!color) cells[col + (row + j) * width].color = WALL;
						if (maze) Sleep(DELAYY);
					}
					CravePassage(cells, row + 2, col, width, height, seed);
				}
			} break;
		}
	}
}

static void GenerateMaze(cell *cells, u32 cellsNum) {
	u64 seed = BOGGetSystemTime();
	u32 index = 0;
	cell start = {0};
	
	do {
		index = (BOGRandom(&seed) % (cellsNum - 1));
		start = cells[index];
	} while ((start.color == START_NODE) || (start.color == TARGET_NODE));
	
	u32 width = cells[cellsNum - 1].col + 1;
	u32 height = cells[cellsNum - 1].row + 1;
	
	CravePassage(cells, start.row / 2 * 2, start.col / 2 * 2, width, height, seed);
}
