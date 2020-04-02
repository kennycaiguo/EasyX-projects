#include "GraphicsLibrary.h"


GraphicsLibrary* gl;				
Matrix4 model, view, projection;	// 模型矩阵，观察矩阵，投影矩阵

// 顶点着色器
void vs(const double* vbo, Point4& position, double attributes[], size_t stride)
{
	position = { vbo[0], vbo[1], vbo[2], 1.0 };
	position = projection * view * model * position;

	memcpy(attributes, vbo + 3, stride);
}
// 片元着色器
void fs(double attributes[], size_t stride, COLORREF& fragColor)
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
	model = Matrix4::Translate(0.0, 0.0, -5.0);
}

int main()
{
	unsigned int width = 640;
	unsigned int height = 480;

	gl = new GraphicsLibrary(width, height);
	gl->cullFace = true;
	gl->cullClockwise = true;
	gl->VertexShader = vs;
	gl->FragmentShader = fs;

	while (true)
	{
		gl->ClearDepth(1.0);

		DrawBox();
		Vector3 eye(0.0, 0.0, 0.0);
		Vector3 up(0.0, 1.0, 0.0);
		Vector3 target(0.0, 0.0, -1.0);
		view = GraphicsLibrary::LookAt(eye, up, target);
		projection = GraphicsLibrary::PerspectiveProjection(-(double)width / height, (double)width / height, -1.0, 1.0, 2.0, 100.0);
		gl->LoadPicture("container.png");

		gl->Draw();
		gl->Flush();
		gl->Swap();
	}
	

	return 0;
}