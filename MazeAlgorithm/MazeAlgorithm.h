#pragma once

#include <set>
#include <vector>
#include <memory>
#include <stack>
#include <algorithm>
#include <ctime>
#include <iostream>
#include <easyx.h>
#include <Eigen/Dense>
using namespace Eigen;

enum class CellState : int { PATH = 0, WALL, VISITED };

/*****************************
 迷宫绘制类
******************************/
class MazeGraph
{
public:
	MazeGraph() = default;
	~MazeGraph() = default;

	void Init(int width, int height, int row, int col, bool isWall = true);
	void DrawWall(int row, int column);
	void DrawPath(int row, int column);
	void Draw(int row, int column, COLORREF fillColor, bool isFrame = true);

private:
	int cellLength = 0;		// 迷宫格子边长
	int gridLeft = 0;		// 迷宫栅格左上角x坐标
	int gridUp = 0;			// 迷宫栅格左上角y坐标
};

/*****************************
 A*寻路容器
******************************/
class A_Container
{
	struct Node
	{
		int x, y;
		int G, H, F;
		Node* parent;
		Node(int _x, int _y, int _G, int _H, Node* p) 
			:x(_x), y(_y), G(_G), H(_H), F(_G + _H), parent(p) {}
	};
	struct Compare
	{
		bool operator ()(const Node* n1, const Node* n2) const { return n1->F < n2->F; }
	};

public:
	A_Container(const Vector2i destination);
	~A_Container();

	void PushOpenList(const Vector2i& position);
	Vector2i GetMinNode();
	Node* GetDestNode() const;
	
private:
	Vector2i destPosition;
	Node* destNode = nullptr;
	std::vector<Node*> closeList;
	std::multiset<Node*, Compare> openList;
};

class Maze
{
public:
	Maze(int _width, int _height, int _row, int _column);
	~Maze();

	void DFSGenerate(bool isIteration = false);						// DFS生成迷宫
	void DivisionGenerate(bool isIteration = false);				// 分割生成
	void PrimGenerate();											// 随机Prim生成迷宫

private:
	void DFSGenerator();											// DFS迭代版本 挖墙生成
	void DFSGenerator(Vector2i node);								// DFS递归版本 挖墙生成
	void DivisionGenerator();										// 分割生成迭代版本 补墙生成
	void DivisionGenerator(Vector2i leftTop, Vector2i rightBottom);	// 分割生成递归版本 补墙生成
	void PrimGenerator();											// 随机Prim生成
	void ClearMaze();

private:
	int width;								// 窗口宽度
	int height;								// 窗口高度
	int delayMs = 40;						// 延时delayMs毫秒
	int row, column;						// 迷宫长宽方格数
	Vector2i end, start;					// 迷宫起点和终点
	MazeGraph mazeGraph;					// 迷宫绘图
	std::vector<std::vector<CellState>> map;// 迷宫二维容器
};