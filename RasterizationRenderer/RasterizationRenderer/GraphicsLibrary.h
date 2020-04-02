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
// 图形加载库
#include "stb_image.h"

/*坐标系是上面为Y正方向，右面为X正方向，屏幕向外为Z正方向
  即右手坐标系
*/
class GraphicsLibrary
{
public:
	GraphicsLibrary(unsigned int width, unsigned int height);
	~GraphicsLibrary();
	// 顶点着色器
	void (*VertexShader)(const double* vbo, Point4& position, double attributes[], size_t stride) = nullptr;
	// 片元着色器
	void (*FragmentShader)(double attributes[], size_t stride, COLORREF& fragColor) = nullptr;
	void SetVBO(double* buffer, size_t count, size_t stride);	// 设置顶点缓冲VBO，顶点数量，顶点数据间隔
	void FastPutPixel(int x, int y, COLORREF c);				// 绘制像素
	COLORREF FastGetPixel(int x, int y);						// 获得像素颜色
	bool LoadPicture(const char* filename);						// 加载图片
	void Flush();												// 刷新屏幕，使得对绘图窗口的显存操作生效
	void Draw();												// 绘制
	void Clear();												// 清屏
	void ClearDepth(double v = 1.0);							// 将深度缓冲区的值全部设置为v
	void Swap();												// 交换链
	COLORREF Texture2D(double u, double v);						// 读取纹理的颜色

	static Matrix4 PerspectiveProjection(double l, double  r, double b, double t, double n, double f);
	static Matrix4 LookAt(Vector3& eye, Vector3& up, Vector3& target);

	bool cullFace = true;										// 是否启用三角形剔除
	bool cullClockwise = true;									// 默认剔除顺时针，false时剔除逆时针
	
private:
	unsigned int screenWidth, screenHeight;						// 屏幕的大小


	std::unique_ptr<unsigned char[]> textureBuffer;				// 纹理数据
	int textureHeight = 0;										// 纹理大小
	int textureWidth = 0;							
	int pictureChannel = 0;										// 加载图片的通道数

	DWORD* imageBuffer;											// 显存指针
	std::unique_ptr<double[]> zBuffer;							// 深度缓冲区
	std::unique_ptr<double[]> vboBuffer;						// 顶点缓冲区
	size_t vboSize = 0;											// 顶点数量
	size_t vertexCount = 0;
	size_t vertexStride = 0;									// 顶点数据间隔
	size_t attributeStride = 0;									// 顶点除了位置坐标外其他属性间隔

	std::unique_ptr<std::list<Edge>[]> newEdgeTable;			// new edge table新边表数组，大小等于screenHeight
	std::unique_ptr<double[]> interpolationAttributes;			// 当前线程在绘制当前顶点插值之后的顶点属性，因为单线程，所以这里只需要一个

	void DrawTriangle(Point4 triangle[], double attributes[]);					// 使用扫描线算法填充三角形
	int clipEdge(int flag, Point4& A, Point4& B, Point4& result, double& t);	// 对边进行裁剪
	void clipTriangleAndDraw(Point4 triangle[], double attributes[]);			// 裁剪三角形并绘制
};