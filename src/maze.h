#ifndef MAZE_H
#define MAZE_H

#define DELAYY 1

typedef enum {
	UP= 1,
	RIGHT = 2,
	LEFT = 3,
	DOWN = 4
} directions;

static bool maze;

static void GenerateMaze(cell *cells, u32 cellsNum);

#endif //MAZE_H
