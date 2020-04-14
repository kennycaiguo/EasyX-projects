#include "MazeAlgorithm.h"

MazeGraph::~MazeGraph()
{
	closegraph();
}

void MazeGraph::Init(int width, int height, int row, int column, bool isWall)
{
	setlinecolor(DARKGRAY);
	cellLength = height / row;
	gridLeft = (width - cellLength * column) / 2;
	gridUp = (height - cellLength * row) / 2;

	for (int i = 0; i < row; i++)
	{
		for (int j = 0; j < column; j++)
		{
			if (isWall || i == 0 || i == row - 1 || j == 0 || j == column - 1)
				DrawWall(i, j);
			else
				DrawPath(i, j);
		}
	}
}

void MazeGraph::DrawWall(int row, int column)
{
	Draw(row, column, LIGHTGRAY, true);
}

void MazeGraph::DrawPath(int row, int column)
{
	Draw(row, column, BLACK, false);
}

void MazeGraph::Draw(int row, int column, COLORREF fillColor, bool isFrame)
{
	setfillcolor(fillColor);
	auto DrawRectangle = solidrectangle;	// 绘制路，路没有边框，填充色为BLACK
	if (isFrame)							// 绘制墙，墙有边框，填充色为LIGHTGRAY；
		DrawRectangle = fillrectangle;
	// 减1是为了与旁边格子的起点不重叠
	DrawRectangle(gridLeft + column * cellLength, gridUp + row * cellLength,
		gridLeft + (column + 1) * cellLength - 1, gridUp + (row + 1) * cellLength - 1);	
}

A_Container::A_Container(const glm::vec2& destination)
	: destPosition(destination) {}

A_Container::~A_Container()
{
	for (auto node : closeList)
		delete node;
	for (auto node : openList)
		delete node;
}

void A_Container::PushOpenList(const glm::vec2 position)
{
	int G = 0;
	Node* parent = nullptr;
	if (!closeList.empty())
	{
		parent = closeList.back();
		G = parent->G + 1;
	}
	int x = static_cast<int>(position.x);
	int y = static_cast<int>(position.y);
	int H = static_cast<int>(abs(position.x - destPosition.x) + abs(position.y - destPosition.y));
	Node* temp(new Node(x, y, G, H, parent));
	if (position == destPosition)
		destNode = temp;
	openList.insert(temp);
}

glm::vec2 A_Container::GetMinNode()
{
	auto it = openList.begin();
	glm::vec2 minNode((*it)->x, (*it)->y);
	closeList.push_back(*it);
	openList.erase(it);
	return minNode;
}

A_Container::Node* A_Container::GetDestNode() const
{
	return destNode;
}

Maze::Maze(int _row, int _column)
{
	initgraph(width, height);
	setbkmode(TRANSPARENT);
	setbkcolor(LIGHTGRAY);
	cleardevice();
	// 构造迷宫大小及起点终点，迷宫大小必须均为奇数
	if ((_row & 1) == 0) ++_row;
	if ((_column & 1) == 0) ++_column;
	if (_row <= 5 || _column <= 5) _row = _column = 31;
	if (_row != _column) _row = _column;	// 保证迷宫格子为正方形
	row = _row;
	column = _column;
	start = glm::vec2(1, 0);
	end = glm::vec2(row - 2, column - 1);
}

void Maze::DFSGenerate(bool isIteration)
{
	ClearMaze();
	for (int i = 0; i < row; i++)
		for (int j = 0; j < column; j++)
			map[i][j] = CellState::WALL;

	mazeGraph.Init(width, height, row, column);
	int startX = static_cast<int>(start.x);
	int startY = static_cast<int>(start.y);
	int endX = static_cast<int>(end.x);
	int endY = static_cast<int>(end.y);
	mazeGraph.Draw(startX, startY, RGB(255, 0, 0));
	mazeGraph.Draw(endX, endY, RGB(0, 255, 0));
	map[startX][startX] = map[endX][endY] = CellState::PATH;
	if (isIteration)
		DFSIterativeGenerator();
	else
		DFSGenerator((rand() % (row - 2) + 1) | 1, (rand() % (column - 2) + 1) | 1);
}

void Maze::DFSIterativeGenerator()
{
	std::stack<glm::vec2> st;
	std::vector<glm::vec2> direction = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
	// 因为迷宫的行列均为奇数，用DFS挖墙起点的索引也要为奇数，否则挖不到起点和终点，可以简单举例尝试
	glm::vec2 node((rand() % (row - 2) + 1) | 1, (rand() % (column - 2) + 1) | 1);
	st.push(node);
	while (!st.empty())
	{
		int i = static_cast<int>(node.x);
		int j = static_cast<int>(node.y);
		if (map[i][j] != CellState::PATH)
		{
			mazeGraph.DrawPath(i, j);
			map[i][j] = CellState::PATH;
			Sleep(delayMs);
		}

		std::random_shuffle(direction.begin(), direction.end());
		int k;
		for (k = 0; k < 4; k++)
		{
			int nextX = static_cast<int>(node.x + 2 * direction[k].x);
			int nextY = static_cast<int>(node.y + 2 * direction[k].y);
			if (nextX >= 1 && nextX <= row - 2 && nextY >= 1 && nextY <= column - 2 
				&& map[nextX][nextY] == CellState::WALL)
			{

			}
		}
	}
}

void Maze::DFSGenerator(int x, int y)
{

}

void Maze::ClearMaze()
{
	cleardevice();
	if (!map.empty()) 
		map.clear();
}
