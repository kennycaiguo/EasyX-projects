#include "MazeAlgorithm.h"

int main()
{
	Maze maze(640, 480, 31, 31);
	//maze.DFSGenerate(true);
	/*maze.DivisionGenerate();
	maze.DivisionGenerate(true);*/

	maze.PrimGenerate();

	std::cin.get();
	return 0;
}