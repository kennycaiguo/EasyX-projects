#include "MazeAlgorithm.h"

void MazeGraph::Init(int width, int height, int row, int column, bool isWall)
{
	setlinecolor(DARKGRAY);
	cellLength = height / row;
	gridLeft = (width - cellLength * column) / 2;
	gridUp = (height - cellLength * row) / 2;

	// 当isWall为true时，迷宫全为墙；当isWall为false时，迷宫外围为墙，内部为路
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

A_Container::A_Container(const Vector2i destination)
	: destPosition(destination) {}

A_Container::~A_Container()
{
	for (auto node : closeList)
		delete node;
	for (auto node : openList)
		delete node;
}

void A_Container::PushOpenList(const Vector2i& position)
{
	int G = 0;
	Node* parent = nullptr;
	if (!closeList.empty())
	{
		parent = closeList.back();
		G = parent->G + 1;
	}
	int x = position.x();
	int y = position.y();
	int H = abs(x - destPosition.x()) + abs(y - destPosition.y());
	Node* temp(new Node(x, y, G, H, parent));
	if (position == destPosition)
		destNode = temp;
	openList.insert(temp);
}

Vector2i A_Container::GetMinNode()
{
	auto it = openList.begin();
	Vector2i minNode((*it)->x, (*it)->y);
	closeList.push_back(*it);
	openList.erase(it);
	return minNode;
}

A_Container::Node* A_Container::GetDestNode() const
{
	return destNode;
}

Maze::Maze(int _width, int _height, int _row, int _column)
{
	srand((unsigned)time(0));
	width = _width;
	height = _height;
	initgraph(width, height);
	setbkmode(TRANSPARENT);
	setbkcolor(LIGHTGRAY);
	cleardevice();
	// 构造迷宫大小及起点终点，迷宫大小必须为奇数
	if ((_row & 1) == 0) ++_row;
	if ((_column & 1) == 0) ++_column;
	if (_row <= 5 || _column <= 5) _row = _column = 31;
	if (_row != _column) _row = _column;	// 保证迷宫格子为正方形
	row = _row;
	column = _column;
	start = Vector2i(1, 0);
	end = Vector2i(row - 2, column - 1);
}

Maze::~Maze()
{
	closegraph();
}

void Maze::DFSGenerate(bool isIteration)
{
	ClearMaze();
	for (int i = 0; i < row; i++)
	{
		map.push_back(std::vector<CellState>());
		for (int j = 0; j < column; j++)
			map[i].push_back(CellState::WALL);
	}

	mazeGraph.Init(width, height, row, column);
	mazeGraph.Draw(start.x(), start.y(), RGB(255, 0, 0));
	mazeGraph.Draw(end.x(), end.y(), RGB(0, 255, 0));
	map[start.x()][start.y()] = map[end.x()][end.y()] = CellState::PATH;
	if (isIteration)
		DFSGenerator();
	else
		DFSGenerator(
			Vector2i((rand() % (row - 2) + 1) | 1, 
				(rand() % (column - 2) + 1) | 1)
		);
}

void Maze::DivisionGenerate(bool isIteration)
{
	ClearMaze();
	// 迷宫外围一圈是墙，内部均为路
	for (int i = 0; i < row; i++)
	{
		map.push_back(std::vector<CellState>());
		for (int j = 0; j < column; j++)
		{
			if (i == 0 || i == row - 1 || j == 0 || j == column - 1)
				map[i].push_back(CellState::WALL);
			else 
				map[i].push_back(CellState::PATH);
		}
	}

	mazeGraph.Init(width, height, row, column, false);
	mazeGraph.Draw(start.x(), start.y(), RGB(0, 255, 0));
	mazeGraph.Draw(end.x(), end.y(), RGB(255, 0, 0));
	map[start.x()][start.y()] = map[end.x()][end.y()] = CellState::PATH;
	if (isIteration) DivisionGenerator();
	else DivisionGenerator(Vector2i(1, 1), Vector2i(row - 2, column - 2));
}

void Maze::PrimGenerate()
{
	ClearMaze();
	for (int i = 0; i < row; i++)
	{
		map.push_back(std::vector<CellState>());
		for (int j = 0; j < column; j++)
			map[i].push_back(CellState::WALL);
	}

	mazeGraph.Init(width, height, row, column);
	mazeGraph.Draw(start.x(), start.y(), RGB(255, 0, 0));
	mazeGraph.Draw(end.x(), end.y(), RGB(0, 255, 0));
	map[start.x()][start.y()] = map[end.x()][end.y()] = CellState::PATH;
	
	// 构建墙栅格，奇数点为通路，
	for (int i = 1; i <= row - 2; i += 2)
	{
		for (int j = 1; j <= column - 2; j += 2)
		{
			mazeGraph.DrawPath(i, j);
			map[i][j] = CellState::PATH;
		}
	}
	Sleep(delayMs);
	std::vector<Vector2i> wallList;
	// 选择一个通路作为起点
	Vector2i root((rand() % (row - 2) + 1) | 1, (rand() % (row - 2) + 1) | 1);
	// 将该通路周围的墙加入墙列表
	if (root.x() - 1 >= 2) wallList.push_back(Vector2i(root.x() - 1, root.y()));
	if (root.x() + 1 <= row - 3) wallList.push_back(Vector2i(root.x() + 1, root.y()));
	if (root.y() - 1 >= 2) wallList.push_back(Vector2i(root.x(), root.y() - 1));
	if (root.y() + 1 <= column - 3) wallList.push_back(Vector2i(root.x(), root.y() + 1));
	// 标记该通路
	map[root.x()][root.y()] = CellState::VISITED;
	int index = 0;
	while (!wallList.empty())
	{
		index = rand() % wallList.size();	// 在墙列表中随机选择一面墙
		Vector2i wall = wallList[index];

		// 墙列表中的墙分为两种，一种是左右是通路，一种上下是通路，所以需要进行判断
		bool isDig = false;			// 墙是否打通
		int up = wall.x() - 1;
		int down = wall.x() + 1;
		int left = wall.y() - 1;
		int right = wall.y() + 1;
		// 如果墙的上方也是墙，说明该墙的左右是通路
		if (map[up][wall.y()] == CellState::WALL)
		{
			// 如果墙的左右有一个没访问过，即一个是VISITED，一个是PATH
			if (map[wall.x()][left] != map[wall.x()][right])
			{
				mazeGraph.DrawPath(wall.x(), wall.y());
				map[wall.x()][wall.y()] = CellState::PATH;
				// 把访问过的通路设置为VISITED
				if (map[wall.x()][left] == CellState::VISITED)
				{
					map[wall.x()][right] = CellState::VISITED;
					++wall.y();
				}
				else
				{
					map[wall.x()][left] = CellState::VISITED;
					--wall.y();
				}
				isDig = true;
			}
		}
		// 墙的上下是通路
		else
		{
			if (map[up][wall.y()] != map[down][wall.y()])
			{
				mazeGraph.DrawPath(wall.x(), wall.y());
				map[wall.x()][wall.y()] = CellState::PATH;
				// 把访问过的通路设置为VISITED
				if (map[up][wall.y()] == CellState::VISITED)
				{
					map[down][wall.y()] = CellState::VISITED;
					++wall.x();
				}
				else
				{
					map[up][wall.y()] = CellState::VISITED;
					--wall.x();
				}
				isDig = true;
			}
		}
		// 如果打通了墙，则把设置为VISITED的通路周围的墙加入墙列表中
		if (isDig)
		{
			up = wall.x() - 1;
			down = wall.x() + 1;
			left = wall.y() - 1;
			right = wall.y() + 1;
			if (up >= 2 && map[up][wall.y()] == CellState::WALL)
				wallList.push_back(Vector2i(up, wall.y()));
			if (down <= row - 3 && map[down][wall.y()] == CellState::WALL)
				wallList.push_back(Vector2i(down, wall.y()));
			if (left >= 2 && map[wall.x()][left] == CellState::WALL)
				wallList.push_back(Vector2i(wall.x(), left));
			if (right <= column - 3 && map[wall.x()][right] == CellState::WALL)
				wallList.push_back(Vector2i(wall.x(), right));
			Sleep(delayMs);
		}
		wallList.erase(wallList.begin() + index);
	}
	for (auto& v1 : map)
		for (auto& v2 : v1)
			if (v2 == CellState::VISITED) v2 = CellState::PATH;
}

void Maze::KruskalGenerate()
{
	ClearMaze();
	for (int i = 0; i < row; i++)
	{
		map.push_back(std::vector<CellState>());
		for (int j = 0; j < column; j++)
			map[i].push_back(CellState::WALL);
	}

	mazeGraph.Init(width, height, row, column);
	mazeGraph.Draw(start.x(), start.y(), RGB(255, 0, 0));
	mazeGraph.Draw(end.x(), end.y(), RGB(0, 255, 0));
	map[start.x()][start.y()] = map[end.x()][end.y()] = CellState::PATH;
	// 构建墙栅格，奇数点为通路，同时构建通路的并查集
	for (int i = 1; i <= row - 2; i += 2)
	{
		for (int j = 1; j <= column - 2; j += 2)
		{
			mazeGraph.DrawPath(i, j);
			map[i][j] = CellState::PATH;
		}
	}
	pathSet = new DisjointSet(row, column);
	Sleep(delayMs);
	// 构建墙列表，每一堵墙分割两个通路
	std::vector<Vector2i> wallList;
	for (int i = 1; i <= row - 2; i++)
	{
		int start, end;
		// 奇数行和偶数行的墙列表索引不同
		if (i % 2 == 0){ start = 1; end = column - 2; }
		else { start = 2; end = column - 3; }
		for (int j = start; j <= end; j += 2)
			wallList.push_back(Vector2i(i, j));
	}
	// 随机化墙列表
	std::random_shuffle(wallList.begin(), wallList.end());
	for (auto& wall : wallList)
	{
		// 墙有两种类型，一种是分割上下通路，一种是分割左右通路
		// 奇数行分割左右通路，偶数行分割上下通路
		if (wall.x() % 2 == 0)
		{
			Vector2i up(wall.x() - 1, wall.y());
			Vector2i down(wall.x() + 1, wall.y());
			// 如果两个通路不属于同一集合，即没有连通，则打通墙
			if (!pathSet->IsSame(up, down))
			{
				mazeGraph.DrawPath(wall.x(), wall.y());
				map[wall.x()][wall.y()] = CellState::PATH;
				pathSet->Union(up, down);
				Sleep(delayMs);
			}
		}
		else
		{
			Vector2i left(wall.x(), wall.y() - 1);
			Vector2i right(wall.x(), wall.y() + 1);
			if (!pathSet->IsSame(left, right))
			{
				mazeGraph.DrawPath(wall.x(), wall.y());
				map[wall.x()][wall.y()] = CellState::PATH;
				pathSet->Union(left, right);
				Sleep(delayMs);
			}
		}
	}
}

void Maze::DFSGenerator()
{
	std::stack<Vector2i> st;
	std::vector<Vector2i> direction = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
	// DFS挖墙起点的索引必须为奇数，否则挖不到起点和终点，可以简单举例尝试
	Vector2i node((rand() % (row - 2) + 1) | 1, (rand() % (column - 2) + 1) | 1);
	st.push(node);
	while (!st.empty())
	{
		if (map[node.x()][node.y()] != CellState::PATH)
		{
			mazeGraph.DrawPath(node.x(), node.y());
			map[node.x()][node.y()] = CellState::PATH;
			Sleep(delayMs);
		}

		std::random_shuffle(direction.begin(), direction.end());
		int k;
		for (k = 0; k < 4; k++)
		{
			Vector2i next = node + direction[k];
			Vector2i next2 = node + 2 * direction[k];
			if (next2.x() >= 1 && next2.x() <= row - 2 && next2.y() >= 1 && next2.y() <= column - 2
				&& map[next2.x()][next2.y()] == CellState::WALL)
			{
				mazeGraph.DrawPath(next.x(), next.y());
				map[next.x()][next.y()] = CellState::PATH;
				node = next2;
				st.push(node);
				Sleep(delayMs);
				break;
			}
		}
		if (k == 4) 
			st.pop();
		if (!st.empty())
			node = st.top();
	}
}

void Maze::DFSGenerator(Vector2i node)
{
	std::vector<Vector2i> direction = { {1, 0}, {-1, 0}, {0, 1}, {0, -1} };
	std::random_shuffle(direction.begin(), direction.end());

	mazeGraph.DrawPath(node.x(), node.y());
	map[node.x()][node.y()] = CellState::PATH;
	Sleep(delayMs);

	for (int i = 0; i < 4; i++)
	{
		Vector2i next = node + direction[i];
		Vector2i next2 = node + 2 * direction[i];
		
		if (next2.x() >= 1 && next2.x() <= row - 2 && next2.y() >= 1 && next2.y() <= column - 2
			&& map[next2.x()][next2.y()] == CellState::WALL)
		{
			mazeGraph.DrawPath(next.x(), next.y());
			map[next.x()][next.y()] = CellState::PATH;
			Sleep(delayMs);
			DFSGenerator(next2);
		}
	}
}

void Maze::DivisionGenerator()
{
	std::stack<Vector2i> st;
	std::vector<int> wallNumber{ 0, 1, 2, 3 }; // 十字墙的编号
	Vector2i leftTop(1, 1);
	Vector2i rightBottom(row - 2, column - 2);
	st.push(leftTop);
	st.push(rightBottom);
	while (!st.empty())
	{
		if (rightBottom.x() - leftTop.x() > 1 && rightBottom.y() - leftTop.y() > 1)
		{
			// 十字墙中心坐标必须为偶数
			Vector2i center(
				((rand() % (rightBottom.x() - leftTop.x()) + leftTop.x() + 1) | 1) - 1,
				((rand() % (rightBottom.y() - leftTop.y()) + leftTop.y() + 1) | 1) - 1
			);
			int i = 0;
			while (center.x() + i <= rightBottom.x() || center.x() - i >= leftTop.x()
				|| center.y() + i <= rightBottom.y() || center.y() - i >= leftTop.y())
			{
				int x, y;
				if (center.x() + i <= rightBottom.x())
				{ 
					x = center.x() + i; y = center.y();
					mazeGraph.DrawWall(x, y);
					map[x][y] = CellState::WALL;
				}
				if (center.x() - i >= leftTop.x())
				{
					x = center.x() - i; y = center.y();
					mazeGraph.DrawWall(x, y);
					map[x][y] = CellState::WALL; 
				}
				if (center.y() + i <= rightBottom.y())
				{
					x = center.x(); y = center.y() + i;
					mazeGraph.DrawWall(x, y);
					map[x][y] = CellState::WALL; 
				}
				if (center.y() - i >= leftTop.y())
				{ 
					x = center.x(); y = center.y() - i;
					mazeGraph.DrawWall(x, y);
					map[x][y] = CellState::WALL;
				}
				Sleep(delayMs);
				i++;
			}
			// 随机选择三面墙打洞，使得四个矩形连通。打洞的位置必须为奇数，防止与下一次生成的十字墙重叠
			std::random_shuffle(wallNumber.begin(), wallNumber.end());
			for (i = 0; i < 3; i++)
			{
				int holeRow, holeColumn;
				switch (wallNumber[i])
				{
				case 0:
					holeRow = (rand() % (center.x() - leftTop.x()) + leftTop.x()) | 1;
					mazeGraph.DrawPath(holeRow, center.y());
					map[holeRow][center.y()] = CellState::PATH;
					break;
				case 1:
					holeColumn = (rand() % (center.y() - leftTop.y()) + leftTop.y()) | 1;
					mazeGraph.DrawPath(center.x(), holeColumn);
					map[center.x()][holeColumn] = CellState::PATH;
				case 2:
					holeRow = (rand() % (rightBottom.x() - center.x()) + center.x()) | 1;
					mazeGraph.DrawPath(holeRow, center.y());
					map[holeRow][center.y()] = CellState::PATH;
					break;
				case 3:
					holeColumn = (rand() % (rightBottom.y() - center.y()) + center.y()) | 1;
					mazeGraph.DrawPath(center.x(), holeColumn);
					map[center.x()][holeColumn] = CellState::PATH;
					break;
				}
			}
			Sleep(delayMs);
			// 除了左上角的方块，其他三个方块压入栈中
			// 左下角方块
			st.push(Vector2i(center.x() + 1, leftTop.y()));
			st.push(Vector2i(rightBottom.x(), center.y() - 1));
			// 右下角方块
			st.push(Vector2i(center.x() + 1, center.y() + 1));
			st.push(Vector2i(rightBottom.x(), rightBottom.y()));
			// 右上角方块
			st.push(Vector2i(leftTop.x(), center.y() + 1));
			st.push(Vector2i(center.x() - 1, rightBottom.y()));
			
			rightBottom.x() = center.x() - 1;
			rightBottom.y() = center.y() - 1;
		}
		else if (!st.empty())
		{
			rightBottom = st.top();
			st.pop();
			leftTop = st.top();
			st.pop();
		}
	}
}

void Maze::DivisionGenerator(Vector2i leftTop, Vector2i rightBottom)
{
	std::vector<int> wallNumber{ 0, 1, 2, 3 };	// 十字墙的编号

	if (rightBottom.x() - leftTop.x() > 1 && rightBottom.y() - leftTop.y() > 1)
	{
		// 十字墙中心坐标必须为偶数
		Vector2i center(
			((rand() % (rightBottom.x() - leftTop.x()) + leftTop.x() + 1) | 1) - 1,
			((rand() % (rightBottom.y() - leftTop.y()) + leftTop.y() + 1) | 1) - 1
		);
		int i = 0;
		while (center.x() + i <= rightBottom.x() || center.x() - i >= leftTop.x()
			|| center.y() + i <= rightBottom.y() || center.y() - i >= leftTop.y())
		{
			int x, y;
			if (center.x() + i <= rightBottom.x())
			{
				x = center.x() + i; y = center.y();
				mazeGraph.DrawWall(x, y);
				map[x][y] = CellState::WALL;
			}
			if (center.x() - i >= leftTop.x())
			{
				x = center.x() - i; y = center.y();
				mazeGraph.DrawWall(x, y);
				map[x][y] = CellState::WALL;
			}
			if (center.y() + i <= rightBottom.y())
			{
				x = center.x(); y = center.y() + i;
				mazeGraph.DrawWall(x, y);
				map[x][y] = CellState::WALL;
			}
			if (center.y() - i >= leftTop.y())
			{
				x = center.x(); y = center.y() - i;
				mazeGraph.DrawWall(x, y);
				map[x][y] = CellState::WALL;
			}
			Sleep(delayMs);
			i++;
		}
		// 随机选择三面墙打洞，使得四个矩形连通。打洞的位置必须为奇数，防止与下一次生成的十字墙重叠
		std::random_shuffle(wallNumber.begin(), wallNumber.end());
		for (i = 0; i < 3; i++)
		{
			int holeRow, holeColumn;
			switch (wallNumber[i])
			{
			case 0:
				holeRow = (rand() % (center.x() - leftTop.x()) + leftTop.x()) | 1;
				mazeGraph.DrawPath(holeRow, center.y());
				map[holeRow][center.y()] = CellState::PATH;
				break;
			case 1:
				holeColumn = (rand() % (center.y() - leftTop.y()) + leftTop.y()) | 1;
				mazeGraph.DrawPath(center.x(), holeColumn);
				map[center.x()][holeColumn] = CellState::PATH;
			case 2:
				holeRow = (rand() % (rightBottom.x() - center.x()) + center.x()) | 1;
				mazeGraph.DrawPath(holeRow, center.y());
				map[holeRow][center.y()] = CellState::PATH;
				break;
			case 3:
				holeColumn = (rand() % (rightBottom.y() - center.y()) + center.y()) | 1;
				mazeGraph.DrawPath(center.x(), holeColumn);
				map[center.x()][holeColumn] = CellState::PATH;
				break;
			}
		}
		Sleep(delayMs);
		// 对四个方块分别进行分割
		DivisionGenerator(Vector2i(leftTop.x(), leftTop.y()), Vector2i(center.x() - 1, center.y() - 1));
		DivisionGenerator(Vector2i(center.x() + 1, leftTop.y()), Vector2i(rightBottom.x(), center.y() - 1));
		DivisionGenerator(Vector2i(center.x() + 1, center.y() + 1), Vector2i(rightBottom.x(), rightBottom.y()));
		DivisionGenerator(Vector2i(leftTop.x(), center.y() + 1), Vector2i(center.x() - 1, rightBottom.y()));
	}
}

int Maze::DisjointSetIndex(int _row, int _column)
{
	int numberOfRow = (column - 2) / 2 + 1;
	int pathRow = _row / 2;
	int pathColumn = _column / 2;
	return pathRow * numberOfRow + pathColumn;
}

void Maze::ClearMaze()
{
	cleardevice();
	if (!map.empty()) 
		map.clear();
}
