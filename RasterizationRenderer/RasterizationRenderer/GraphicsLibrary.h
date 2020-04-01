#pragma once

#include <graphics.h>
#include <list>
#include "Point2.h"
#include "Point4.h"
#include "Edge.h"
#include "Matrix4.h"

/*����ϵ������ΪY����������ΪX��������Ļ����ΪZ������
  ����������ϵ
*/
class GraphicsLibrary
{
public:
	GraphicsLibrary(unsigned int width, unsigned int height);
	~GraphicsLibrary();

	char errmas[1024] = { '0' };	// ������Ϣ���������������Զ�ȡ����Ϣ
	/*
	������ɫ��
	Position��Ӧ��OpenGL��gl_Position��
	VBO����ǰ��������ԣ�
	varying��ʾ��Ҫͨ��������ɫ�����ݸ�ƬԪ��ɫ���ı��������ڶ��㴦��ֵ�����ݸ�ƬԪ��ɫ��
	*/
	void (*VertexShader)(const double* vbo, double* varing, Point4& position);	
	/*
	FragColor��Ӧ��OpenGL��gl_FragColor��ֻ��û����͸���ȣ�ֻ��RGB
	������ɫ���е�Varying����1/w��ֵ֮�󴫵ݸ�ƬԪ��ɫ��
	*/
	void (*FragmentShader)(double* varing, COLORREF& fragcolor);
	void SetVBO(double* buffer, int numOfvertex, int count);	// ���ö��㻺�����
	void FastPutPixel(int x, int y, COLORREF c);				// ��������
	COLORREF FastGetPixel(int x, int y);						// ���������ɫ
	bool LoadPicture(const char* filename);						// ����ͼƬ
	void Flush();												// ˢ����Ļ��ʹ�öԻ�ͼ���ڵ��Դ������Ч
	bool Draw();												// ����
	void Clear();												// ����
	void ClearDepth(double v);									// ����Ȼ�������ֵȫ������Ϊv
	void Swap();												// ������
	void SetVaryingCount(int count);							// ����Varying����������
	COLORREF Texture2D(double x, double y);						// ��ȡ�������ɫ

	bool cullFace = true;										// �Ƿ�����˳ʱ����ʱ���������޳�
	bool cullCWCCW = false;										// Ĭ����ʱ�룬trueΪ˳ʱ��
	
private:
	unsigned int screenWidth, screenHeight;						// ��Ļ�Ĵ�С

	unsigned char* pictureData = nullptr;						// ͼƬ������
	int pictureWidth = 0;										// ����ͼƬ�Ĵ�С
	int pictureHeight = 0;

	unsigned char* textureBuffer = nullptr;						// ��������
	int textureHeight, textureWidth;							// �����С

	DWORD* imageBuffer;											// �Դ�ָ��
	double* zBuffer = nullptr;									// ��Ȼ�����
	double* vboBuffer = nullptr;								// ���㻺����
	int vboCount;												// ��������
	int numOfVertexVBO = 0;										// ���㻺���������ж��������
	int countOfVarying = 0;										// varying����������

	std::list<Edge>* NET = nullptr;								// new edge table�±߱����飬��С����screenHeight
	double* interpolationVarying = nullptr;						// ��ǰ�߳��ڻ��Ƶ�ǰ�����ֵ֮���varying����Ϊ���̣߳���������ֻ��Ҫһ��

	void DrawTriangle(Point4* pointArray, double* varying);		// ʹ��ɨ�����㷨���������
	int ClipEdge(Point4& A, Point4& B, Point4& tmpA, Point4& tmpB, 
		double& proportionA, double& proportionB);				// �Ա߽��вü�(����ά�ռ���)
	int ClipEdgeByParallelFace(Point4& A, Point4& B, Point4& tmpA, Point4& tmpB, 
		double& proportionA, double& proportionB, int flag);	// ʹ��һ��ƽ��ƽ��ü���,flagΪ1��2��3ʱ�ֱ�ʹ��left right��bottom top��near farƽ��ü�
	void ClipFaceByParallelFace(Point4* ps, int pCount, Point4* resultPoint, int& resultCount, double* varying, 
		int countofvarying, double* resultvarying, int flag);	//ʹ��CVV��ƽ��ü�,resultCount Ϊ�ü�֮��Ķ�������
};