#pragma once
#include <graphics.h>
#include <windows.h>
#include <string>
#include <map>
#include <functional>

#pragma region ö��

/****����״ö��****/
enum SimpleShape
{
	RECTANGLE = 0,		// ����
	ROUNDRECT = 1,		// Բ�Ǿ���
	CIRCLE = 2,			// Բ��
	ELLIPSE = 3			// ��Բ
};

/****�ؼ�����ö��****/
enum UIElement
{
	UNKNOW = 0,			// δ֪
	STATIC = 1,			// ��̬����޷���Ӧ�¼���ֻ�ܻ���
	BUTTON = 2,			// ��ť
	DRAGBAR = 3,		// �϶���
	INPUTBOX = 4,		// �����
	PREBAR = 5,			// ������
	CHECKBOX = 6,		// ѡ���

	LASTOFENUM = 0xFFFF	// ö�ٸ�ֵ�����ֵ���벻Ҫ�������ֵ
};

/****�¼�����ö��****/
enum UIEvent
{
	MOUSE_MOVE = 0,		// ����ƶ�
	MOUSE_DOWN = 1,		// ��갴��
	MOUSE_UP = 2,		// ���̧��
	KEYBOARD_INPUT = 3	// ��������
};

#pragma endregion ö��


#pragma region ���Ƶ�Ԫ

/*******************************************************
* ���Ƶ�Ԫ
* ���пɻ��ƶ���Ļ���(������)
* �̳д�����������ʵ��Draw����
* ע�⣬ע�⣺�������չ�������ͣ����Ƶ�ģʽ��Ҫ���� m_DrawPos��
* ���������һ������Ϊ 20 �ľ��Σ�Ӧ�ô� m_DrawPos Ϊ���Ͻǿ�ʼ��
* ���Ƴ���Ϊ20�ľ���
*********************************************************/
class DrawCell
{
public:
	DrawCell() { mChildCell = nullptr; mDrawPos.x = mDrawPos.y = 0; }
	virtual ~DrawCell() 
	{
		if (mChildCell)
			delete mChildCell;
	}

	virtual void draw() = 0;	// ���ƺ���

	void setDrawPos(POINT pos) { mDrawPos = pos; }					// ���û���λ��
	POINT getDrawPos() const { return mDrawPos; }					// ��ȡ����λ��
	DrawCell * setChildCell(DrawCell* cell) { mChildCell = cell; }	// ���ú��ӻ�����
	DrawCell * getChildCell() const { return mChildCell; }			// ��ȡ���ӻ�����

protected:
	POINT mDrawPos;			// ����λ��
	DrawCell * mChildCell;	// ���ӻ�����
};

#pragma endregion ���Ƶ�Ԫ