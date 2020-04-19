#include "MazeAlgorithm.h"

void MazeGraph::Init(int width, int height, int row, int column, bool isWall)
{
	setlinecolor(DARKGRAY);
	cellLength = height / row;
	gridLeft = (width - cellLength * column) / 2;
	gridUp = (height - cellLength * row) / 2;

	// ��isWallΪtrueʱ���Թ�ȫΪǽ����isWallΪfalseʱ���Թ���ΧΪǽ���ڲ�Ϊ·
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
	auto DrawRectangle = solidrectangle;	// ����·��·û�б߿����ɫΪBLACK
	if (isFrame)							// ����ǽ��ǽ�б߿����ɫΪLIGHTGRAY��
		DrawRectangle = fillrectangle;
	// ��1��Ϊ�����Ա߸��ӵ���㲻�ص�
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
	// �����Թ���С������յ㣬�Թ���С����Ϊ����
	if ((_row & 1) == 0) ++_row;
	if ((_column & 1) == 0) ++_column;
	if (_row <= 5 || _column <= 5) _row = _column = 31;
	if (_row != _column) _row = _column;	// ��֤�Թ�����Ϊ������
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
	// �Թ���ΧһȦ��ǽ���ڲ���Ϊ·
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
	
	// ����ǽդ��������Ϊͨ·��
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
	// ѡ��һ��ͨ·��Ϊ���
	Vector2i root((rand() % (row - 2) + 1) | 1, (rand() % (row - 2) + 1) | 1);
	// ����ͨ·��Χ��ǽ����ǽ�б�
	if (root.x() - 1 >= 2) wallList.push_back(Vector2i(root.x() - 1, root.y()));
	if (root.x() + 1 <= row - 3) wallList.push_back(Vector2i(root.x() + 1, root.y()));
	if (root.y() - 1 >= 2) wallList.push_back(Vector2i(root.x(), root.y() - 1));
	if (root.y() + 1 <= column - 3) wallList.push_back(Vector2i(root.x(), root.y() + 1));
	// ��Ǹ�ͨ·
	map[root.x()][root.y()] = CellState::VISITED;
	int index = 0;
	while (!wallList.empty())
	{
		index = rand() % wallList.size();	// ��ǽ�б������ѡ��һ��ǽ
		Vector2i wall = wallList[index];

		// ǽ�б��е�ǽ��Ϊ���֣�һ����������ͨ·��һ��������ͨ·��������Ҫ�����ж�
		bool isDig = false;			// ǽ�Ƿ��ͨ
		int up = wall.x() - 1;
		int down = wall.x() + 1;
		int left = wall.y() - 1;
		int right = wall.y() + 1;
		// ���ǽ���Ϸ�Ҳ��ǽ��˵����ǽ��������ͨ·
		if (map[up][wall.y()] == CellState::WALL)
		{
			// ���ǽ��������һ��û���ʹ�����һ����VISITED��һ����PATH
			if (map[wall.x()][left] != map[wall.x()][right])
			{
				mazeGraph.DrawPath(wall.x(), wall.y());
				map[wall.x()][wall.y()] = CellState::PATH;
				// �ѷ��ʹ���ͨ·����ΪVISITED
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
		// ǽ��������ͨ·
		else
		{
			if (map[up][wall.y()] != map[down][wall.y()])
			{
				mazeGraph.DrawPath(wall.x(), wall.y());
				map[wall.x()][wall.y()] = CellState::PATH;
				// �ѷ��ʹ���ͨ·����ΪVISITED
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
		// �����ͨ��ǽ���������ΪVISITED��ͨ·��Χ��ǽ����ǽ�б���
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
	// ����ǽդ��������Ϊͨ·��ͬʱ����ͨ·�Ĳ��鼯
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
	// ����ǽ�б�ÿһ��ǽ�ָ�����ͨ·
	std::vector<Vector2i> wallList;
	for (int i = 1; i <= row - 2; i++)
	{
		int start, end;
		// �����к�ż���е�ǽ�б�������ͬ
		if (i % 2 == 0){ start = 1; end = column - 2; }
		else { start = 2; end = column - 3; }
		for (int j = start; j <= end; j += 2)
			wallList.push_back(Vector2i(i, j));
	}
	// �����ǽ�б�
	std::random_shuffle(wallList.begin(), wallList.end());
	for (auto& wall : wallList)
	{
		// ǽ���������ͣ�һ���Ƿָ�����ͨ·��һ���Ƿָ�����ͨ·
		// �����зָ�����ͨ·��ż���зָ�����ͨ·
		if (wall.x() % 2 == 0)
		{
			Vector2i up(wall.x() - 1, wall.y());
			Vector2i down(wall.x() + 1, wall.y());
			// �������ͨ·������ͬһ���ϣ���û����ͨ�����ͨǽ
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
	// DFS��ǽ������������Ϊ�����������ڲ��������յ㣬���Լ򵥾�������
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
	std::vector<int> wallNumber{ 0, 1, 2, 3 }; // ʮ��ǽ�ı��
	Vector2i leftTop(1, 1);
	Vector2i rightBottom(row - 2, column - 2);
	st.push(leftTop);
	st.push(rightBottom);
	while (!st.empty())
	{
		if (rightBottom.x() - leftTop.x() > 1 && rightBottom.y() - leftTop.y() > 1)
		{
			// ʮ��ǽ�����������Ϊż��
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
			// ���ѡ������ǽ�򶴣�ʹ���ĸ�������ͨ���򶴵�λ�ñ���Ϊ��������ֹ����һ�����ɵ�ʮ��ǽ�ص�
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
			// �������Ͻǵķ��飬������������ѹ��ջ��
			// ���½Ƿ���
			st.push(Vector2i(center.x() + 1, leftTop.y()));
			st.push(Vector2i(rightBottom.x(), center.y() - 1));
			// ���½Ƿ���
			st.push(Vector2i(center.x() + 1, center.y() + 1));
			st.push(Vector2i(rightBottom.x(), rightBottom.y()));
			// ���ϽǷ���
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
	std::vector<int> wallNumber{ 0, 1, 2, 3 };	// ʮ��ǽ�ı��

	if (rightBottom.x() - leftTop.x() > 1 && rightBottom.y() - leftTop.y() > 1)
	{
		// ʮ��ǽ�����������Ϊż��
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
		// ���ѡ������ǽ�򶴣�ʹ���ĸ�������ͨ���򶴵�λ�ñ���Ϊ��������ֹ����һ�����ɵ�ʮ��ǽ�ص�
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
		// ���ĸ�����ֱ���зָ�
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
