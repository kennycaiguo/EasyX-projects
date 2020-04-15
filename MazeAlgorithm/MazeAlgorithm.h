#pragma once

#include <set>
#include <vector>
#include <memory>
#include <stack>
#include <algorithm>
#include <iostream>
#include <easyx.h>
#include <glm/glm.hpp>

enum class CellState : int { PATH = 0, WALL, FLAG };

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
	A_Container(const glm::vec2& destination);
	~A_Container();

	void PushOpenList(const glm::vec2 position);
	glm::vec2 GetMinNode();
	Node* GetDestNode() const;
	
private:
	glm::vec2 destPosition;
	Node* destNode = nullptr;
	std::vector<Node*> closeList;
	std::multiset<Node*, Compare> openList;
};

class Maze
{
public:
	Maze(int _width, int _height, int _row, int _column);
	~Maze();

	void DFSGenerate(bool isIteration = false);

private:
	void DFSIterativeGenerator();			// DFS迭代版本 挖墙生成
	void DFSGenerator(int x, int y);		// DFS递归版本 挖墙生成
	void ClearMaze();

private:
	int width;								// 窗口宽度
	int height;								// 窗口高度
	int delayMs = 40;						// 延时delayMs毫秒
	int row, column;						// 迷宫长宽方格数
	glm::vec2 end, start;					// 迷宫起点和终点
	MazeGraph mazeGraph;					// 迷宫绘图
	std::vector<std::vector<CellState>> map;// 迷宫二维容器
};