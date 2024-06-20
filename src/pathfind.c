#include "pathfind.h"

static void ReversePath(s32 *path, u32 index) {
	if (path[index] == -1) return;
	ReversePath(path, path[index]);
	path[path[index]] = index;
}

static void MinHeapify(cell *cells, u32 nodes, u32 parent) {
	u32 smallest = parent;
	u32 lChild = 2 * parent + 1;
	u32 rChild = 2 * parent + 2;
	
	if ((lChild < nodes) && (cells[parent].distance > cells[lChild].distance)) smallest = lChild;
	if ((rChild < nodes) && (cells[smallest].distance > cells[rChild].distance)) smallest = rChild;
	
	if (smallest != parent) {
		cell temp = cells[smallest];
		cells[smallest] = cells[parent];
		cells[parent] = temp;
		
		MinHeapify(cells, nodes, smallest);
	}
}

static u32 ManhattanDistance(s32 src, s32 dst) {
	u32 abs1 = ((src >> 31) ^ src) - (src >> 31);
	u32 abs2 = ((dst >> 31) ^ dst) - (dst >> 31);
	
	return abs1 + abs2;
}

static void AStar(cell *cells, u32 cellsNum) {
	udm unvisited = cellsNum - 1;
	cell unvisitedCells[MAX_CELLS];
	s32 visitedCells[MAX_CELLS] = {-1};
	
	cell target = {0};
	
	for (udm i = 0; i < cellsNum; ++i) {
		unvisitedCells[i] = cells[i];
		if (!cells[i].distance) {
			cell temp = unvisitedCells[0];
			unvisitedCells[0] = cells[i];
			unvisitedCells[i] = temp;
		}
		
		if (cells[i].color == TARGET_NODE) target = cells[i];
		visitedCells[i] = -1;
	}
	
	cell start = unvisitedCells[0];
	cell closest = start;
	
	u32 width = cells[cellsNum - 1].col + 1;
	u32 height = cells[cellsNum - 1].row + 1;
	
	while (unvisited) {
		if (closest.row > 0) {
			u32 index = closest.col + (closest.row - 1) * width;
			cell *up = &cells[index];
			
			for (u32 i = 0; i <= unvisited && !up->visited; ++i) {
				u32 row = unvisitedCells[i].row;
				u32 col = unvisitedCells[i].col;
				if (row == up->row && col == up->col) {
					up->distance = ManhattanDistance(up->col - target.col, up->row - target.row);
					unvisitedCells[i].distance = up->distance;
					visitedCells[index] = closest.col + closest.row * width;
					break;
				}
			}
		}
		
		if (closest.col > 0) {
			u32 index = (closest.col - 1) + closest.row * width;
			cell *left = &cells[index];
			
			for (u32 i = 0; i <= unvisited && !left->visited; ++i) {
				u32 row = unvisitedCells[i].row;
				u32 col = unvisitedCells[i].col;
				if (row == left->row && col == left->col) {
					left->distance = ManhattanDistance(left->col - target.col, left->row - target.row);
					unvisitedCells[i].distance = left->distance;
					visitedCells[index] = closest.col + closest.row * width;
					break;
				}
			}
		}
		
		if (closest.col < width - 1) {
			u32 index = (closest.col + 1) + closest.row * width;
			cell *right = &cells[index];
			
			for (u32 i = 0; i <= unvisited && !right->visited; ++i) {
				u32 row = unvisitedCells[i].row;
				u32 col = unvisitedCells[i].col;
				if (row == right->row && col == right->col) {
					right->distance = ManhattanDistance(right->col - target.col,
														right->row - target.row);
					unvisitedCells[i].distance = right->distance;
					visitedCells[index] = closest.col + closest.row * width;
					break;
				}
			}
		}
		
		if (closest.row < height - 1) {
			u32 index = closest.col + (closest.row + 1) * width;
			cell *down = &cells[index];
			
			for (u32 i = 0; i <= unvisited && !down->visited; ++i) {
				u32 row = unvisitedCells[i].row;
				u32 col = unvisitedCells[i].col;
				if (row == down->row && col == down->col) {
					down->distance = ManhattanDistance(down->col - target.col, down->row - target.row);
					unvisitedCells[i].distance = down->distance;
					visitedCells[index] = closest.col + closest.row * width;
					break;
				}
			}
		}
		
		cell previousClosest = unvisitedCells[0];
		
		do {
			cell temp = unvisitedCells[0];
			unvisitedCells[0] = unvisitedCells[unvisited];
			unvisitedCells[unvisited] = temp;
			
			for (s32 i = (s32) unvisited / 2 - 1; i >= 0; --i) {
				MinHeapify(unvisitedCells, (s32) unvisited, i);
			}
			
			closest = unvisitedCells[0];
		} while (unvisited-- && (closest.color == WALL));
		
		u32 previousIndex = previousClosest.col + previousClosest.row * width;
		cells[previousIndex].visited = true;
		
		if ((closest.color == TARGET_NODE) || (closest.distance == UINT_MAX)) break;
		
		u32 currentIndex = closest.col + closest.row * width;
		cells[currentIndex].color = SEARCHING;
		if (pathfinding) Sleep(1);
	}
	
	u32 index = closest.col + closest.row * width;
	ReversePath(visitedCells, index);
	visitedCells[index] = -1;
	
	index = start.col + start.row * width;
	while (visitedCells[index] != -1) {
		index = visitedCells[index];
		if (cells[index].color == SEARCHING) {
			cells[index].color = FOUND;
			if (pathfinding) Sleep(1);
		}
	}
}

static void Dijkstra(cell *cells, u32 cellsNum) {
	udm unvisited = cellsNum - 1;
	cell unvisitedCells[MAX_CELLS];
	s32 visitedCells[MAX_CELLS] = {-1};
	
	for (udm i = 0; i < cellsNum; ++i) {
		unvisitedCells[i] = cells[i];
		if (!cells[i].distance) {
			cell temp = unvisitedCells[0];
			unvisitedCells[0] = cells[i];
			unvisitedCells[i] = temp;
		}
		
		visitedCells[i] = -1;
	}
	
	cell start = unvisitedCells[0];
	cell closest = start;
	
	u32 width = cells[cellsNum - 1].col + 1;
	u32 height = cells[cellsNum - 1].row + 1;
	
	while (unvisited) {
		if (closest.row > 0) {
			u32 index = closest.col + (closest.row - 1) * width;
			cell *up = &cells[index];
			
			for (u32 i = 0; i <= unvisited && !up->visited; ++i) {
				u32 row = unvisitedCells[i].row;
				u32 col = unvisitedCells[i].col;
				if (row == up->row && col == up->col) {
					up->distance = closest.distance + 1;
					unvisitedCells[i].distance = up->distance;
					visitedCells[index] = closest.col + closest.row * width;
					break;
				}
			}
		}
		
		if (closest.col > 0) {
			u32 index = (closest.col - 1) + closest.row * width;
			cell *left = &cells[index];
			
			for (u32 i = 0; i <= unvisited && !left->visited; ++i) {
				u32 row = unvisitedCells[i].row;
				u32 col = unvisitedCells[i].col;
				if (row == left->row && col == left->col) {
					left->distance = closest.distance + 1;
					unvisitedCells[i].distance = left->distance;
					visitedCells[index] = closest.col + closest.row * width;
					break;
				}
			}
		}
		
		if (closest.col < width - 1) {
			u32 index = (closest.col + 1) + closest.row * width;
			cell *right = &cells[index];
			
			for (u32 i = 0; i <= unvisited && !right->visited; ++i) {
				u32 row = unvisitedCells[i].row;
				u32 col = unvisitedCells[i].col;
				if (row == right->row && col == right->col) {
					right->distance = closest.distance + 1;
					unvisitedCells[i].distance = right->distance;
					visitedCells[index] = closest.col + closest.row * width;
					break;
				}
			}
		}
		
		if (closest.row < height - 1) {
			u32 index = closest.col + (closest.row + 1) * width;
			cell *down = &cells[index];
			
			for (u32 i = 0; i <= unvisited && !down->visited; ++i) {
				u32 row = unvisitedCells[i].row;
				u32 col = unvisitedCells[i].col;
				if (row == down->row && col == down->col) {
					down->distance = closest.distance + 1;
					unvisitedCells[i].distance = down->distance;
					visitedCells[index] = closest.col + closest.row * width;
					break;
				}
			}
		}
		
		cell previousClosest = unvisitedCells[0];
		
		do {
			cell temp = unvisitedCells[0];
			unvisitedCells[0] = unvisitedCells[unvisited];
			unvisitedCells[unvisited] = temp;
			
			for (s32 i = (s32) unvisited / 2 - 1; i >= 0; --i) {
				MinHeapify(unvisitedCells, (s32) unvisited, i);
			}
			
			closest = unvisitedCells[0];
		} while (unvisited-- && (closest.color == WALL));
		
		u32 previousIndex = previousClosest.col + previousClosest.row * width;
		cells[previousIndex].visited = true;
		
		if ((closest.color == TARGET_NODE) || (closest.distance == UINT_MAX)) break;
		
		u32 currentIndex = closest.col + closest.row * width;
		cells[currentIndex].color = SEARCHING;
		if (pathfinding) Sleep(1);
	}
	
	u32 index = closest.col + closest.row * width;
	ReversePath(visitedCells, index);
	visitedCells[index] = -1;
	
	index = start.col + start.row * width;
	while (visitedCells[index] != -1) {
		index = visitedCells[index];
		if (cells[index].color == SEARCHING) {
			cells[index].color = FOUND;
			if (pathfinding) Sleep(1);
		}
	}
}

static void BFS(cell *cells, u32 cellsNum) {
	udm unvisited = cellsNum - 1;
	cell unvisitedCells[MAX_CELLS];
	s32 visitedCells[MAX_CELLS] = {-1};
	
	for (udm i = 0; i < cellsNum; ++i) {
		if (cells[i].color == START_NODE) unvisitedCells[unvisited] = cells[i];
		visitedCells[i] = -1;
	}
	
	cell start = unvisitedCells[unvisited];
	cell closest = start;
	
	u32 width = cells[cellsNum - 1].col + 1;
	u32 height = cells[cellsNum - 1].row + 1;
	
	while (unvisited) {
		if (closest.row > 0) {
			u32 index = closest.col + (closest.row - 1) * width;
			cell up = cells[index];
		
			if (!up.visited) {
				for (s32 i = cellsNum - 1; i >= 0; --i) {
					if (!unvisitedCells[i].size) {
						unvisitedCells[i] = up;
						break;
					}
					
					u32 row = unvisitedCells[i].row;
					u32 col = unvisitedCells[i].col;
					if (row == up.row && col == up.col) break;
				}
				
				visitedCells[index] = closest.col + closest.row * width;
			}
		}
		
		if (closest.col > 0) {
			u32 index = (closest.col - 1) + closest.row * width;
			cell left = cells[index];
			
			if (!left.visited) {
				for (s32 i = cellsNum - 1; i >= 0; --i) {
					if (!unvisitedCells[i].size) {
						unvisitedCells[i] = left;
						break;
					}
					
					u32 row = unvisitedCells[i].row;
					u32 col = unvisitedCells[i].col;
					if (row == left.row && col == left.col) break;
				}
				
				visitedCells[index] = closest.col + closest.row * width;
			}
		}
		
		if (closest.col < width - 1) {
			u32 index = (closest.col + 1) + closest.row * width;
			cell right = cells[index];
			
			if (!right.visited) {
				for (s32 i = cellsNum - 1; i >= 0; --i) {
					if (!unvisitedCells[i].size) {
						unvisitedCells[i] = right;
						break;
					}
					
					u32 row = unvisitedCells[i].row;
					u32 col = unvisitedCells[i].col;
					if (row == right.row && col == right.col) break;
				}
				
				visitedCells[index] = closest.col + closest.row * width;
			}
		}
		
		if (closest.row < height - 1) {
			u32 index = closest.col + (closest.row + 1) * width;
			cell down = cells[index];
			
			if (!down.visited) {
				for (s32 i = cellsNum - 1; i >= 0; --i) {
					if (!unvisitedCells[i].size) {
						unvisitedCells[i] = down;
						break;
					}
					
					u32 row = unvisitedCells[i].row;
					u32 col = unvisitedCells[i].col;
					if (row == down.row && col == down.col) break;
				}
				
				visitedCells[index] = closest.col + closest.row * width;
			}
		}
		
		cell previousClosest = unvisitedCells[unvisited];
		
		do {
			closest = unvisitedCells[unvisited - 1];
		} while (--unvisited && (closest.color == WALL));
		
		u32 previousIndex = previousClosest.col + previousClosest.row * width;
		cells[previousIndex].visited = true;
		
		if (!closest.size || (closest.color == TARGET_NODE)) break;
		
		u32 currentIndex = closest.col + closest.row * width;
		cells[currentIndex].color = SEARCHING;
		if (pathfinding) Sleep(1);
	}
	
	if (closest.color == TARGET_NODE) {
		u32 index = closest.col + closest.row * width;
		ReversePath(visitedCells, index);
		visitedCells[index] = -1;
	
		index = start.col + start.row * width;
		while (visitedCells[index] != -1) {
			index = visitedCells[index];
			if (cells[index].color == SEARCHING) {
				cells[index].color = FOUND;
				if (pathfinding) Sleep(1);
			}
		}
	}
}

static void DFS(cell *cells, u32 cellsNum) {
	udm unvisited = cellsNum - 1;
	cell unvisitedCells[MAX_CELLS] = {{0}};
	s32 visitedCells[MAX_CELLS] = {-1};
	
	for (udm i = 0; i < cellsNum; ++i) {
		if (cells[i].color == START_NODE) unvisitedCells[unvisited] = cells[i];
		visitedCells[i] = -1;
	}
	
	cell start = unvisitedCells[unvisited];
	cell closest = start;
	
	u32 width = cells[cellsNum - 1].col + 1;
	u32 height = cells[cellsNum - 1].row + 1;
	
	while (unvisited) {
		if (closest.row < height - 1) {
			u32 index = closest.col + (closest.row + 1) * width;
			cell down = cells[index];
			
			if (!down.visited) {
				for (u32 i = 0; i < unvisited; ++i) {
					u32 row = unvisitedCells[i].row;
					u32 col = unvisitedCells[i].col;
					if (row == down.row && col == down.col) {
						for (s32 j = i; j > 0; --j) {
							unvisitedCells[j] = unvisitedCells[j - 1];
						}
					}
					
					unvisitedCells[i] = unvisitedCells[i + 1];
				}
				
				unvisitedCells[unvisited - 1] = down;
				visitedCells[index] = closest.col + closest.row * width;
			}
		}
		
		if (closest.col < width - 1) {
			u32 index = (closest.col + 1) + closest.row * width;
			cell right = cells[index];
			
			if (!right.visited) {
				for (u32 i = 0; i < unvisited; ++i) {
					u32 row = unvisitedCells[i].row;
					u32 col = unvisitedCells[i].col;
					if (row == right.row && col == right.col) {
						for (s32 j = i; j > 0; --j) {
							unvisitedCells[j] = unvisitedCells[j - 1];
						}
					}
					
					unvisitedCells[i] = unvisitedCells[i + 1];
				}
				
				unvisitedCells[unvisited - 1] = right;
				visitedCells[index] = closest.col + closest.row * width;
			}
		}
		
		if (closest.col > 0) {
			u32 index = (closest.col - 1) + closest.row * width;
			cell left = cells[index];
			
			if (!left.visited) {
				for (u32 i = 0; i < unvisited; ++i) {
					u32 row = unvisitedCells[i].row;
					u32 col = unvisitedCells[i].col;
					if (row == left.row && col == left.col) {
						for (s32 j = i; j > 0; --j) {
							unvisitedCells[j] = unvisitedCells[j - 1];
						}
					}
					
					unvisitedCells[i] = unvisitedCells[i + 1];
				}
				
				unvisitedCells[unvisited - 1] = left;
				visitedCells[index] = closest.col + closest.row * width;
			}
		}
		
		if (closest.row > 0) {
			u32 index = closest.col + (closest.row - 1) * width;
			cell up = cells[index];
		
			if (!up.visited) {
				for (u32 i = 0; i < unvisited; ++i) {
					u32 row = unvisitedCells[i].row;
					u32 col = unvisitedCells[i].col;
					if (row == up.row && col == up.col) {
						for (u32 j = i; j > 0; --j) {
							unvisitedCells[j] = unvisitedCells[j - 1];
						}
					}
					
					unvisitedCells[i] = unvisitedCells[i + 1];
				}
				
				unvisitedCells[unvisited - 1] = up;
				visitedCells[index] = closest.col + closest.row * width;
			}
		}
		
		cell previousClosest = unvisitedCells[unvisited];
		
		do {
			closest = unvisitedCells[unvisited - 1];
		} while (--unvisited && ((closest.color == WALL) ||
								 cells[closest.col + closest.row * width].visited));
		
		u32 previousIndex = previousClosest.col + previousClosest.row * width;
		cells[previousIndex].visited = true;
		
		if (!closest.size || (closest.color == TARGET_NODE)) break;
		
		u32 currentIndex = closest.col + closest.row * width;
		cells[currentIndex].color = SEARCHING;
		if (pathfinding) Sleep(1);
	}
	
	if (closest.color == TARGET_NODE) {
		u32 index = closest.col + closest.row * width;
		ReversePath(visitedCells, index);
		visitedCells[index] = -1;
	
		index = start.col + start.row * width;
		while (visitedCells[index] != -1) {
			index = visitedCells[index];
			if (cells[index].color == SEARCHING) {
				cells[index].color = FOUND;
				if (pathfinding) Sleep(1);
			}
		}
	}
}
