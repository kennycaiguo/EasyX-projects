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
 �Թ�������
******************************/
class MazeGraph
{
public:
	MazeGraph() = default;
	~MazeGraph();

	void Init(int width, int height, int row, int col, bool isWall = true);
	void DrawWall(int row, int column);
	void DrawPath(int row, int column);
	void Draw(int row, int column, COLORREF fillColor, bool isFrame = true);

private:
	int cellLength = 0;		// �Թ����ӱ߳�
	int gridLeft = 0;		// �Թ�դ�����Ͻ�x����
	int gridUp = 0;			// �Թ�դ�����Ͻ�y����
};

/*****************************
 A*Ѱ·����
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
	Maze(int row, int column);

	void DFSGenerate(bool isIteration = false);

private:
	void DFSIterativeGenerator();			// ���� �ָ� ��ǽ����
	void DFSGenerator(int x, int y);		// �ݹ� �ָ� ��ǽ���� Ҫ�����Ϊ����
	void ClearMaze();
private:
	int delayMs = 40;						// ��ʱdelayMs����
	int row, column;						// �Թ���������
	glm::vec2 end, start;					// �Թ������յ�
	MazeGraph mazeGraph;					// �Թ���ͼ
	std::vector<std::vector<CellState>> map;// �Թ���ά����

public:
	int width = 640;						// ���ڿ��
	int height = 480;						// ���ڸ߶�
};