#include <easyx.h>
#include <iostream>
#include <ctime>
#include "MazeAlgorithm.h"

int main()
{
	srand((unsigned)time(nullptr));
	Maze maze(640, 480, 31, 31);
	maze.DFSGenerate(true);

	std::cin.get();
	return 0;
}