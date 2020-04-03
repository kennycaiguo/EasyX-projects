#include "GraphicsLibrary.h"
#include "Camera.h"
#include "Timer.h"
#include "conio.h"	

unsigned int width = 640;
unsigned int height = 480;

GraphicsLibrary* gl;
Matrix4 model, view, projection;	// 模型矩阵，观察矩阵，投影矩阵

Camera camera;
bool isClick = false;
double xMouseClick = 0.0;
double yMouseClick = 0.0;
Vector3 rotateAxisYaw = {0.0, 1.0, 0.0};
Vector3 rotateAxisPitch = {1.0, 0.0, 0.0};

Timer timer;

// 顶点着色器
void vs(const double* vbo, Point4& position, double* attributes, size_t stride)
{
	position = { vbo[0], vbo[1], vbo[2], 1.0 };
	position = projection * view * model * position;

	memcpy(attributes, vbo + 3, stride * sizeof(double));
}
// 片元着色器
void fs(double* attributes, size_t stride, COLORREF& fragColor)
{
	double u = attributes[0];
	double v = attributes[1];

	fragColor = gl->Texture2D(u, v);
}

void DrawBox()
{
	double boxBuffer[] =
	{
		// 位置坐标		 纹理坐标
		-1.0, -1.0,  1.0, 0.0, 1.0,	// 正面
		 1.0,  1.0,  1.0, 1.0, 0.0,
		-1.0,  1.0,  1.0, 0.0, 0.0,

		-1.0, -1.0,  1.0, 0.0, 1.0,
		 1.0, -1.0,  1.0, 1.0, 1.0,
		 1.0,  1.0,  1.0, 1.0, 0.0,

		 1.0, -1.0,  1.0, 0.0, 1.0,	// 右面
		 1.0,  1.0, -1.0, 1.0, 0.0,
		 1.0,  1.0,  1.0, 0.0, 0.0,

		 1.0, -1.0,  1.0, 0.0, 1.0,
		 1.0, -1.0, -1.0, 1.0, 1.0,
		 1.0,  1.0, -1.0, 1.0, 0.0,

		 1.0, -1.0, -1.0, 0.0, 1.0,	// 背面
		-1.0,  1.0, -1.0, 1.0, 0.0,
		 1.0,  1.0, -1.0, 0.0, 0.0,

		 1.0, -1.0, -1.0, 0.0, 1.0,
		-1.0, -1.0, -1.0, 1.0, 1.0,
		-1.0,  1.0, -1.0, 1.0, 0.0,

		-1.0, -1.0, -1.0, 0.0, 1.0,	// 左面
		-1.0,  1.0,  1.0, 1.0, 0.0,
		-1.0,  1.0, -1.0, 0.0, 0.0,

		-1.0, -1.0, -1.0, 0.0, 1.0,
		-1.0, -1.0,  1.0, 1.0, 1.0,
		-1.0,  1.0,  1.0, 1.0, 0.0,

		-1.0,  1.0,  1.0, 0.0, 1.0,	// 顶面
		 1.0,  1.0, -1.0, 1.0, 0.0,
		-1.0,  1.0, -1.0, 0.0, 0.0,

		-1.0,  1.0,  1.0, 0.0, 1.0,
		 1.0,  1.0,  1.0, 1.0, 1.0,
		 1.0,  1.0, -1.0, 1.0, 0.0,

		-1.0, -1.0, -1.0, 0.0, 1.0,	// 底面
		 1.0, -1.0,  1.0, 1.0, 0.0,
		-1.0, -1.0,  1.0, 0.0, 0.0,

		-1.0, -1.0, -1.0, 0.0, 1.0,
		 1.0, -1.0, -1.0, 1.0, 1.0,
		 1.0, -1.0,  1.0, 1.0, 0.0
	};

	gl->SetVBO(boxBuffer, 36, 5);
	model = Matrix4::Translate(0.0, 0.0, -8.0);
	gl->cullFace = true;
	gl->cullClockwise = true;
	gl->VertexShader = vs;
	gl->FragmentShader = fs;
	gl->LoadPicture("container.png");
}

void processKeyBoardInput()
{
	if (GetAsyncKeyState('W') & 0x8000)
		camera.ProcessKeyboard(CameraMovement::FORWARD, timer.DeltaTime());
	if (GetAsyncKeyState('A') & 0x8000)
		camera.ProcessKeyboard(CameraMovement::LEFT, timer.DeltaTime());
	if (GetAsyncKeyState('S') & 0x8000)
		camera.ProcessKeyboard(CameraMovement::BACKWARD, timer.DeltaTime());
	if (GetAsyncKeyState('D') & 0x8000)
		camera.ProcessKeyboard(CameraMovement::RIGHT, timer.DeltaTime());
	if (GetAsyncKeyState('Q') & 0x8000)
		camera.ProcessKeyboard(CameraMovement::UP, timer.DeltaTime());
	if (GetAsyncKeyState('E') & 0x8000)
		camera.ProcessKeyboard(CameraMovement::DOWN, timer.DeltaTime());
}

int main()
{
	gl = new GraphicsLibrary(width, height);
	timer.Reset();

	DrawBox();

	while (true)
	{
		timer.Tick();
		if (timer.DeltaTime() < 10.0)
			Sleep(10 - timer.DeltaTime());

		processKeyBoardInput();

		if (MouseHit())
		{
			MOUSEMSG m = GetMouseMsg();
			switch (m.uMsg)
			{
			case WM_MOUSEMOVE:
			{
				if (isClick)
				{
					double xPos = m.x;
					double yPos = m.y;

					double xOffset = xPos - xMouseClick;
					double yOffset = yMouseClick - yPos;
					xMouseClick = xPos;
					yMouseClick = yPos;

					model = model * Matrix4::Rotate({ 0.0, 1.0, 0.0 }, xOffset) *
						Matrix4::Rotate({ 1.0, 0.0, 0.0 }, yOffset);
				}
				break;
			}

			case WM_LBUTTONDOWN:
				isClick = true;
				xMouseClick = m.x;
				yMouseClick = m.y;
				break;

			case WM_LBUTTONUP:
				isClick = false;
				break;

			case WM_MOUSEWHEEL:
				camera.ProcessMouseScroll(m.wheel / 80);
				break;
			}
		}
		else
		{
			view = camera.GetViewMatrix();
			projection = GraphicsLibrary::PerspectiveProjection(camera.zoom, (double)width / height, 1, 100.0);

			gl->Clear();
			gl->ClearDepth(1.0);
			gl->Draw();
			gl->Flush();
			gl->Swap();
		}

		if (GetAsyncKeyState(VK_ESCAPE) & 0x8000)
			break;
	}

	return 0;
}