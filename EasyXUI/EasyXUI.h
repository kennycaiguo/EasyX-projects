#pragma once
#include <graphics.h>
#include <windows.h>
#include <string>
#include <map>
#include <functional>

#pragma region 枚举

/****简单形状枚举****/
enum SimpleShape
{
	RECTANGLE = 0,		// 矩形
	ROUNDRECT = 1,		// 圆角矩形
	CIRCLE = 2,			// 圆形
	ELLIPSE = 3			// 椭圆
};

/****控件类型枚举****/
enum UIElement
{
	UNKNOW = 0,			// 未知
	STATIC = 1,			// 静态风格，无法响应事件，只能绘制
	BUTTON = 2,			// 按钮
	DRAGBAR = 3,		// 拖动条
	INPUTBOX = 4,		// 输入框
	PREBAR = 5,			// 进度条
	CHECKBOX = 6,		// 选项框

	LASTOFENUM = 0xFFFF	// 枚举赋值的最大值，请不要超过这个值
};

/****事件类型枚举****/
enum UIEvent
{
	MOUSE_MOVE = 0,		// 鼠标移动
	MOUSE_DOWN = 1,		// 鼠标按下
	MOUSE_UP = 2,		// 鼠标抬起
	KEYBOARD_INPUT = 3	// 键盘输入
};

#pragma endregion 枚举


#pragma region 绘制单元

/*******************************************************
* 绘制单元
* 所有可绘制对象的基类(抽象类)
* 继承此类的子类必须实现Draw函数
* 注意，注意：如果想扩展绘制类型，绘制的模式需要基于 m_DrawPos，
* 比如想绘制一个长宽为 20 的矩形，应该从 m_DrawPos 为左上角开始，
* 绘制长宽为20的矩形
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

	virtual void draw() = 0;	// 绘制函数

	void setDrawPos(POINT pos) { mDrawPos = pos; }					// 设置绘制位置
	POINT getDrawPos() const { return mDrawPos; }					// 获取绘制位置
	DrawCell * setChildCell(DrawCell* cell) { mChildCell = cell; }	// 设置孩子绘制器
	DrawCell * getChildCell() const { return mChildCell; }			// 获取孩子绘制器

protected:
	POINT mDrawPos;			// 绘制位置
	DrawCell * mChildCell;	// 孩子绘制器
};

#pragma endregion 绘制单元