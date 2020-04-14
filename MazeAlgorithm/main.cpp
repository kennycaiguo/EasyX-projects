#include <easyx.h>
#include <iostream>

int main()
{
	initgraph(640, 480);

	setfillcolor(BLUE);
	setlinecolor(RED);
	circle(50, 50, 50);
	solidrectangle(100, 200, 400, 300);
	fillrectangle(200, 200, 300, 100);

	std::cin.get();
	closegraph();

	return 0;
}