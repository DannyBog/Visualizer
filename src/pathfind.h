#ifndef PATHFIND_H
#define PATHFIND_H

typedef struct {
	u32 row, col;
	u32 size;
	u32 distance;
	bool visited;
	COLORREF color;
} cell;

#define SEARCHING	0xfff000
#define FOUND		0x00ffff

#define WALL		0x808080
#define GRID		0x808080
#define START_NODE	0x0000ff
#define TARGET_NODE 0x00ff00

#define MAX_CELLS	4096

static bool pathfinding;
static bool recalculate;

static void ReversePath(s32 *path, u32 index);
static void MinHeapify(cell *cells, u32 nodes, u32 parent);

static void AStar(cell *cells, u32 cellsNum);
static void Dijkstra(cell *cells, u32 cellsNum);
static void BFS(cell *cells, u32 cellsNum);
static void DFS(cell *cells, u32 cellsNum);

#endif //PATHFIND_H
