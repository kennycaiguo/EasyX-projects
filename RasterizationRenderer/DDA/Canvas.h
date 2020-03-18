#pragma once

#include <graphics.h>

class Canvas
{
public:
	Canvas(int width, int height);
	~Canvas();

	void setPixel(int x, int y, COLORREF color);	// 绘制像素点

private:
	DWORD* pImageBuf;								// 显存指针
	int mScrWidth, mScrHeight;						// 画布的大小
};

Canvas::Canvas(int width, int height)
{
	initgraph(width, height);						// 初始化窗口
	pImageBuf = GetImageBuffer();					// 获取显存指针
	mScrWidth = width, mScrHeight = height;
}

Canvas::~Canvas()
{
	closegraph();									// 关闭窗口
}

void Canvas::setPixel(int x, int y, COLORREF color)
{
	if (x > 0 && x < mScrWidth && y > 0 && y < mScrHeight)
	{
		y = mScrHeight - y;							// 翻转y轴
		pImageBuf[x + y * mScrWidth] = BGR(color);
	}
}