#pragma once

#include <graphics.h>

class Canvas
{
public:
	Canvas(int width, int height);
	~Canvas();

	void setPixel(int x, int y, COLORREF color);	// �������ص�

private:
	DWORD* pImageBuf;								// �Դ�ָ��
	int mScrWidth, mScrHeight;						// �����Ĵ�С
};

Canvas::Canvas(int width, int height)
{
	initgraph(width, height);						// ��ʼ������
	pImageBuf = GetImageBuffer();					// ��ȡ�Դ�ָ��
	mScrWidth = width, mScrHeight = height;
}

Canvas::~Canvas()
{
	closegraph();									// �رմ���
}

void Canvas::setPixel(int x, int y, COLORREF color)
{
	if (x > 0 && x < mScrWidth && y > 0 && y < mScrHeight)
	{
		y = mScrHeight - y;							// ��תy��
		pImageBuf[x + y * mScrWidth] = BGR(color);
	}
}