#pragma once

#include <graphics.h>
#include <list>
#include "Point2.h"
#include "Point4.h"
#include "Edge.h"
#include "Matrix4.h"

/*坐标系是上面为Y正方向，右面为X正方向，屏幕向外为Z正方向
  即右手坐标系
*/
class GraphicsLibrary
{
public:
	GraphicsLibrary(unsigned int width, unsigned int height);
	~GraphicsLibrary();

	char errmas[1024] = { '0' };	// 错误信息，如果发生错误可以读取本消息
	/*
	顶点着色器
	Position对应了OpenGL的gl_Position，
	VBO来当前顶点的属性，
	varying表示需要通过顶点着色器传递给片元着色器的变量，会在顶点处插值被传递给片元着色器
	*/
	void (*VertexShader)(const double* vbo, double* varing, Point4& position);	
	/*
	FragColor对应于OpenGL的gl_FragColor，只是没有了透明度，只有RGB
	顶点着色器中的Varying经过1/w插值之后传递给片元着色器
	*/
	void (*FragmentShader)(double* varing, COLORREF& fragcolor);
	void SetVBO(double* buffer, int numOfvertex, int count);	// 设置顶点缓冲对象
	void FastPutPixel(int x, int y, COLORREF c);				// 绘制像素
	COLORREF FastGetPixel(int x, int y);						// 获得像素颜色
	bool LoadPicture(const char* filename);						// 加载图片
	void Flush();												// 刷新屏幕，使得对绘图窗口的显存操作生效
	bool Draw();												// 绘制
	void Clear();												// 清屏
	void ClearDepth(double v);									// 将深度缓冲区的值全部设置为v
	void Swap();												// 交换链
	void SetVaryingCount(int count);							// 设置Varying变量的数量
	COLORREF Texture2D(double x, double y);						// 读取纹理的颜色

	bool cullFace = true;										// 是否启用顺时针逆时针三角形剔除
	bool cullCWCCW = false;										// 默认逆时针，true为顺时针
	
private:
	unsigned int screenWidth, screenHeight;						// 屏幕的大小

	unsigned char* pictureData = nullptr;						// 图片的数据
	int pictureWidth = 0;										// 加载图片的大小
	int pictureHeight = 0;

	unsigned char* textureBuffer = nullptr;						// 纹理数据
	int textureHeight, textureWidth;							// 纹理大小

	DWORD* imageBuffer;											// 显存指针
	double* zBuffer = nullptr;									// 深度缓冲区
	double* vboBuffer = nullptr;								// 顶点缓冲区
	int vboCount;												// 顶点数量
	int numOfVertexVBO = 0;										// 顶点缓冲区对象中顶点的数量
	int countOfVarying = 0;										// varying变量的数量

	std::list<Edge>* NET = nullptr;								// new edge table新边表数组，大小等于screenHeight
	double* interpolationVarying = nullptr;						// 当前线程在绘制当前顶点插值之后的varying，因为单线程，所以这里只需要一个

	void DrawTriangle(Point4* pointArray, double* varying);		// 使用扫描线算法填充三角形
	int ClipEdge(Point4& A, Point4& B, Point4& tmpA, Point4& tmpB, 
		double& proportionA, double& proportionB);				// 对边进行裁剪(在四维空间中)
	int ClipEdgeByParallelFace(Point4& A, Point4& B, Point4& tmpA, Point4& tmpB, 
		double& proportionA, double& proportionB, int flag);	// 使用一组平行平面裁剪边,flag为1，2，3时分别使用left right、bottom top、near far平面裁剪
	void ClipFaceByParallelFace(Point4* ps, int pCount, Point4* resultPoint, int& resultCount, double* varying, 
		int countofvarying, double* resultvarying, int flag);	//使用CVV对平面裁剪,resultCount 为裁剪之后的顶点数量
};