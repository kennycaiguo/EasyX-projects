#pragma once

#include <graphics.h>
#include <list>
#include <vector>
#include <memory>
#include "Point2.h"
#include "Point4.h"
#include "Edge.h"
#include "Vector3.h"
#include "Matrix4.h"
// ͼ�μ��ؿ�
#include "stb_image.h"

/*����ϵ������ΪY����������ΪX��������Ļ����ΪZ������
  ����������ϵ
*/
class GraphicsLibrary
{
public:
	GraphicsLibrary(unsigned int width, unsigned int height);
	~GraphicsLibrary();
	// ������ɫ��
	void (*VertexShader)(const double* vbo, Point4& position, double attributes[], size_t stride) = nullptr;
	// ƬԪ��ɫ��
	void (*FragmentShader)(double attributes[], size_t stride, COLORREF& fragColor) = nullptr;
	void SetVBO(double* buffer, size_t count, size_t stride);	// ���ö��㻺��VBO�������������������ݼ��
	void FastPutPixel(int x, int y, COLORREF c);				// ��������
	COLORREF FastGetPixel(int x, int y);						// ���������ɫ
	bool LoadPicture(const char* filename);						// ����ͼƬ
	void Flush();												// ˢ����Ļ��ʹ�öԻ�ͼ���ڵ��Դ������Ч
	void Draw();												// ����
	void Clear();												// ����
	void ClearDepth(double v = 1.0);							// ����Ȼ�������ֵȫ������Ϊv
	void Swap();												// ������
	COLORREF Texture2D(double u, double v);						// ��ȡ�������ɫ

	static Matrix4 PerspectiveProjection(double l, double  r, double b, double t, double n, double f);
	static Matrix4 LookAt(Vector3& eye, Vector3& up, Vector3& target);

	bool cullFace = true;										// �Ƿ������������޳�
	bool cullClockwise = true;									// Ĭ���޳�˳ʱ�룬falseʱ�޳���ʱ��
	
private:
	unsigned int screenWidth, screenHeight;						// ��Ļ�Ĵ�С


	std::unique_ptr<unsigned char[]> textureBuffer;				// ��������
	int textureHeight = 0;										// �����С
	int textureWidth = 0;							
	int pictureChannel = 0;										// ����ͼƬ��ͨ����

	DWORD* imageBuffer;											// �Դ�ָ��
	std::unique_ptr<double[]> zBuffer;							// ��Ȼ�����
	std::unique_ptr<double[]> vboBuffer;						// ���㻺����
	size_t vboSize = 0;											// ��������
	size_t vertexCount = 0;
	size_t vertexStride = 0;									// �������ݼ��
	size_t attributeStride = 0;									// �������λ���������������Լ��

	std::unique_ptr<std::list<Edge>[]> newEdgeTable;			// new edge table�±߱����飬��С����screenHeight
	std::unique_ptr<double[]> interpolationAttributes;			// ��ǰ�߳��ڻ��Ƶ�ǰ�����ֵ֮��Ķ������ԣ���Ϊ���̣߳���������ֻ��Ҫһ��

	void DrawTriangle(Point4 triangle[], double attributes[]);					// ʹ��ɨ�����㷨���������
	int clipEdge(int flag, Point4& A, Point4& B, Point4& result, double& t);	// �Ա߽��вü�
	void clipTriangleAndDraw(Point4 triangle[], double attributes[]);			// �ü������β�����
};