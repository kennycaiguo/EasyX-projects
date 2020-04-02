#include "GraphicsLibrary.h"

GraphicsLibrary::GraphicsLibrary(unsigned int width, unsigned int height)
	: screenWidth(width), screenHeight(height)
{
	initgraph(width, height);
	BeginBatchDraw();
	imageBuffer = GetImageBuffer();
	zBuffer.reset(new double[(size_t)width * height]);
	newEdgeTable.reset(new std::list<Edge>[height]);
}

GraphicsLibrary::~GraphicsLibrary()
{
	EndBatchDraw();
	closegraph();         
}

void GraphicsLibrary::SetVBO(double* buffer, size_t count, size_t stride)
{
	vertexCount = count;
	vertexStride = stride;
	attributeStride = vertexStride - 3;
	interpolationAttributes.reset(new double[attributeStride]);

	vboSize = vertexStride * vertexCount;
	vboBuffer.reset(new double[vboSize]);
	memcpy(vboBuffer.get(), buffer, vboSize);
}

void GraphicsLibrary::FastPutPixel(int x, int y, COLORREF c)
{
	imageBuffer[y * screenWidth + x] = BGR(c);
}

COLORREF GraphicsLibrary::FastGetPixel(int x, int y)
{
	COLORREF c = imageBuffer[y * screenWidth + x];
	return BGR(c);
}

bool GraphicsLibrary::LoadPicture(const char* filename)
{
	unsigned char* pictureData = stbi_load(filename, &textureWidth, &textureHeight, &pictureChannel, 0);
	if (pictureData)
	{
		textureBuffer.reset(pictureData);
		return true;
	}
	else
		return false;
}

void GraphicsLibrary::Flush()
{
	FlushBatchDraw();
}

void GraphicsLibrary::Draw()
{
	std::unique_ptr<Point4[]> triangle(new Point4[3]);
	std::unique_ptr<double[]> attributes(new double[attributeStride * 3]);
	Point4 point;

	double* vbo = vboBuffer.get();
	for (int i = 0; i < vertexCount / 3; i++)
	{
		for (int j = 0; j < 3; j++)
		{
			VertexShader(vbo + (3 * i + j) * vertexStride, point, 
				attributes.get() + j * attributeStride, attributeStride);
			triangle[j] = point;
		}
		
		clipTriangleAndDraw(triangle.get(), attributes.get());
	}

	return;
}

void GraphicsLibrary::Clear()
{
	cleardevice();
}

void GraphicsLibrary::ClearDepth(double v)
{
	std::fill(zBuffer.get(), zBuffer.get() + (size_t)screenWidth * screenHeight, v);
}

void GraphicsLibrary::Swap()
{
	EndBatchDraw();
	BeginBatchDraw();
}

COLORREF GraphicsLibrary::Texture2D(double u, double v)
{
	// Repeat模式采样
	u -= floor(u);
	v -= floor(v);

	size_t pixelSize = sizeof(char) * pictureChannel;
	size_t rowSize = textureWidth * pixelSize;
	if (textureBuffer.get() && pictureChannel >= 3)
	{
		int x = u * textureWidth;
		int y = u * textureHeight;
		return RGB(
			textureBuffer[y * rowSize + x * pixelSize],
			textureBuffer[y * rowSize + x * pixelSize + 1],
			textureBuffer[y * rowSize + x * pixelSize + 2]);
	}
	else
		return RGB(255, 255, 255);
}

Matrix4 GraphicsLibrary::PerspectiveProjection(double l, double r, double b, double t, double n, double f)
{
	return Matrix4{
			2 * n / (r - l), 0,				  (r + l) / (r - l), 0,
			0,				 2 * n / (t - b), (t + b) / (t - b), 0,
			0,				 0,				  (n + f) / (n - f), 2 * f * n / (n - f),
			0,				 0,				  -1,				 0,
	};
}

Matrix4 GraphicsLibrary::LookAt(Vector3& eye, Vector3& up, Vector3& target)
{
	up.Normalize();

	// 相机的三个轴，摄像机正方向为视点->相机
	Vector3 cameraDirection = eye - target;
	cameraDirection.Normalize();
	Vector3 cameraRight = Vector3::Cross(up, cameraDirection);
	cameraRight.Normalize();
	Vector3 cameraUp = Vector3::Cross(cameraDirection, cameraRight); 
	cameraUp.Normalize();

	// 表示摄像机的平移，平移矩阵的逆矩阵
	Matrix4 inverseTranslation = {
		1, 0, 0, -eye.x,
		0, 1, 0, -eye.y,
		0, 0, 1, -eye.z,
		0, 0, 0, 1
	}; 

	// 表示了摄像机的旋转,并且为一个正交矩阵，其逆矩阵等于转置矩阵
	Matrix4 transposeRotate = 
	{
		cameraRight.x,	   cameraRight.y,	  cameraRight.z,	 0,
		cameraUp.x,		   cameraUp.y,		  cameraUp.z,		 0,
		cameraDirection.x, cameraDirection.x, cameraDirection.x, 0,
		0,				   0,				  0,				 1
	};

	// 观察矩阵
	return transposeRotate * inverseTranslation;
}

void GraphicsLibrary::DrawTriangle(Point4 triangle[], double attributes[])
{
	Point4 points[3];
	for (int i = 0; i < 3; i++)
	{
		points[i].x = (triangle[i].x + 1) * 0.5 * ((double)screenWidth - 1);
		points[i].y = (triangle[i].y + 1) * 0.5 * ((double)screenHeight - 1);
		points[i].z = triangle[i].z;
		points[i].w = triangle[i].w;
	}

	Vector3 ab(points[1].x - points[0].x, points[1].y - points[0].y, 0.0);
	Vector3 bc(points[2].x - points[1].x, points[2].y - points[1].y, 0.0);
	Vector3 ac(points[2].x - points[0].x, points[2].y - points[0].y, 0.0);
	Vector3 ca(points[0].x - points[2].x, points[0].y - points[2].y, 0.0);
	double doubleArea = ab.x * ac.y - ab.y * ac.x;	
	if (doubleArea == 0)
		return;
	if (cullFace)
	{
		if (cullClockwise)
		{
			if (doubleArea < 0)
				return;
		}
		else
		{
			if (doubleArea > 0)
				return;
		}
	}

	int yMin = (int)points[0].y;			// 多边形y值的最小值
	int yMax = (int)points[0].y;			// 多边形y值的最大值
	// 初始化new edge table新边表，即多边形的边构成的链表
	for (size_t i = 0; i < 3; i++)
	{
		if (points[i].y > points[(i + 1) % 3].y)
		{
			double x = points[(i + 1) % 3].x;
			double dx = (points[(i + 1) % 3].x - points[i].x)
				/ (points[(i + 1) % 3].y - points[i].y);
			double ymax = points[i].y;
			newEdgeTable[(size_t)points[(i + 1) % 3].y].push_back(Edge(x, dx, ymax));
		}
		else if (points[i].y < points[(i + 1) % 3].y)
		{
			double x = points[i].x;
			double dx = (points[(i + 1) % 3].x - points[i].x)
				/ (points[(i + 1) % 3].y - points[i].y);
			double ymax = points[(i + 1) % 3].y;
			newEdgeTable[(size_t)points[i].y].push_back(Edge(x, dx, ymax));
		}
		yMin = (int)min(points[i].y, yMin);
		yMax = (int)max(points[i].y, yMax);
	}

	// 创建AEL(active edge list)链表，即与扫描线相交的线段
	std::list<Edge> activeEdge;
	for (int y = yMin; y <= yMax; y++)
	{
		if (!newEdgeTable[y].empty())						// 如果扫描行存在边，则加入AEL中
		{
			activeEdge.splice(activeEdge.end(), newEdgeTable[y]);
			activeEdge.sort(sortEdge);						// 因为三角形不会出现两条边交叉的情况，所以出现新边后才需要重新排序
		}

		std::list<Edge>::iterator edgeStar, edgeEnd;		// 扫描行需要绘制的线段的端点的迭代器
		int countEdge = 0;									// 已存储的端点

		for (auto it = activeEdge.begin(); it != activeEdge.end(); )
		{
			// 扫描线已经到达某条边的y的最大值，则移除该边
			if ((int)it->yMax <= y)
			{
				it = activeEdge.erase(it);					// 这里记录移除元素的下一个元素的迭代器，不能用it++，因为it已经无效了。
			}
			else
			{
				if (countEdge == 0)
				{
					edgeStar = it;
					countEdge++;
				}
				else if (countEdge == 1)
				{
					countEdge = 0;
					edgeEnd = it;
					// 绘制线段
					for (int x = (int)edgeStar->x; x <= (int)edgeEnd->x; x++)
					{
						double weightA, weightB, weightC;
						Vector3 ap(x - points[0].x, y - points[0].y, 0.0);
						Vector3 bp(x - points[1].x, y - points[1].y, 0.0);
						Vector3 cp(x - points[2].x, y - points[2].y, 0.0);
						weightA = (bc.x * bp.y - bc.y * bp.x) / doubleArea;
						weightB = (ca.x * cp.y - ca.y * cp.x) / doubleArea;
						weightC = (ab.x * ap.y - ab.y * ap.x) / doubleArea;

						double omega = 1.0 / ((1.0 / points[0].w) * weightA + (1.0 / points[1].w) * weightB +
							(1.0 / points[2].w) * weightC);
						double z = omega * (points[0].z / points[0].w * weightA + points[1].z / points[1].w * weightB +
							points[2].z / points[2].w * weightC);

						// 与z缓冲区进行比较
						if (z > zBuffer[x + screenWidth * y])
							continue;
						else
							zBuffer[x + screenWidth * y] = z;

						//对顶点的属性进行插值
						for (size_t i = 0; i < attributeStride; i++)
						{
							interpolationAttributes[i] = omega * (attributes[i] / points[0].w * weightA +
								attributes[i + attributeStride] / points[1].w * weightB +
								attributes[i + attributeStride * 2] / points[2].w * weightC);
						}

						COLORREF c;
						FragmentShader(interpolationAttributes.get(), attributeStride, c);	//调用片元着色器
						FastPutPixel(x, y, c);								//填充颜色
					}
					// 这对交点的x值增加dx
					edgeStar->x += edgeStar->dx;
					edgeEnd->x += edgeEnd->dx;
				}
				it++;
			}
		}
	}
}
//对直线进行裁剪,返回-1表示平凡拒绝，返回0表示平凡接受，返回1表示对A点进行裁剪，返回2表示对B点进行裁剪，最后两个参数result和t表示裁剪点坐标和t值
//其中参数flag从0到5分别表示边界为left、right、bottom、top、near、far
int GraphicsLibrary::clipEdge(int flag, Point4& A, Point4& B, Point4& result, double& t)
{
	double aClipCondition = 0;
	double bClipCondition = 0;
	switch (flag)
	{
	case 0:
		aClipCondition = A.x + A.w;
		bClipCondition = B.x + B.w;
		break;
	case 1:
		aClipCondition = A.w - A.x;
		bClipCondition = B.w - B.x;
		break;
	case 2:
		aClipCondition = A.y + A.w;
		bClipCondition = B.y + B.w;
		break;
	case 3:
		aClipCondition = A.w - A.y;
		bClipCondition = B.w - B.y;
		break;
	case 4:
		aClipCondition = A.z + A.w;
		bClipCondition = B.z + B.w;
		break;
	case 5:
		aClipCondition = A.w - A.z;
		bClipCondition = B.z - B.z;
		break;
	default:
		return 0;
	}
	if (aClipCondition < 0 && bClipCondition < 0)
		return -1;
	if (aClipCondition >= 0 && bClipCondition >= 0)
		return 0;

	t = aClipCondition / (aClipCondition - bClipCondition);
	result.x = A.x + (B.x - A.x) * t;
	result.y = A.y + (B.y - A.y) * t;
	result.z = A.z + (B.z - A.z) * t;
	result.w = A.w + (B.w - A.w) * t;
	if (aClipCondition < 0)
		return 1;
	else
		return 2;
}

void GraphicsLibrary::clipTriangleAndDraw(Point4 triangle[], double attributes[])
{
	std::vector<Point4> points(triangle, triangle + 3);								 // 裁剪后顶点数量
	std::vector<double> newAttributes(attributes, attributes + attributeStride * 3); // 裁剪后顶点属性

	// 用视椎体对三角形进行裁剪，裁剪方法与Suthland-Hodgman算法类似
	for (int clipflag = 0; clipflag < 6; clipflag++)
	{
		std::vector<Point4> temp;		// 用于储存本次裁剪之后的顶点
		std::vector<double> tempAttri;	// 用于储存本次裁剪之后的顶点属性
		int size = points.size();
		for (int i = 0; i < size; i++)
		{
			Point4 result;				// 边界与三角形边的交点
			double t;					// 交点线性插值的系数
			size_t indexA = i, indexB = (i + 1) % size;
			int ret = clipEdge(clipflag, points[indexA], points[indexB], result, t);
			// 平凡接受
			if (ret == 0)
			{
				temp.push_back(points[indexB]);
				for (size_t j = 0; j < attributeStride; j++)
					tempAttri.push_back(newAttributes[indexB * attributeStride + j]);
			}
			// 裁剪A点，加入入点和B点
			else if (ret == 1)
			{
				temp.push_back(result);
				for (size_t j = 0; j < attributeStride; j++)
				{
					tempAttri.push_back(newAttributes[indexA * attributeStride + j] + 
						(newAttributes[indexB * attributeStride + j] - newAttributes[indexA * attributeStride + j]) * t);
				}
				temp.push_back(points[(i + 1) % size]);
				for (size_t j = 0; j < attributeStride; j++)
					tempAttri.push_back(newAttributes[indexB * attributeStride + j]);
			}
			// 裁剪B点，加入出点
			else if (ret == 2)
			{
				temp.push_back(result);
				for (size_t j = 0; j < attributeStride; j++)
				{
					tempAttri.push_back(newAttributes[indexA * attributeStride + j] +
						(newAttributes[indexB * attributeStride + j] - newAttributes[indexA * attributeStride + j]) * t);
				}
			}
		}
		points.clear();
		points.assign(temp.begin(), temp.end());
		newAttributes.clear();
		newAttributes.assign(tempAttri.begin(), tempAttri.end());
	}
	// 将裁剪后的图形分解成多个三角形进行绘制，记得要先进行透视除法将坐标变换到CVV空间
	std::unique_ptr<Point4[]> out(new Point4[3]);
	std::unique_ptr<double[]> outAttributes(new double[attributeStride * 3]);
	for (size_t i = 1; i < points.size() - 1; i++)
	{
		out[0].x = points[0].x / points[0].w;
		out[0].y = points[0].y / points[0].w;
		out[0].z = points[0].z / points[0].w;
		out[0].w = points[0].w;
		out[1].x = points[i].x / points[i].w;
		out[1].y = points[i].y / points[i].w;
		out[1].z = points[i].z / points[i].w;
		out[1].w = points[i].w;
		out[2].x = points[i + 1].x / points[i + 1].w;
		out[2].y = points[i + 1].y / points[i + 1].w;
		out[2].z = points[i + 1].z / points[i + 1].w;
		out[2].w = points[i + 1].w;

		for (size_t j = 0; j < attributeStride; j++)
		{
			outAttributes[j] = newAttributes[j];	// points中第一个顶点总是每个三角形的第一个顶点
			outAttributes[j + attributeStride] = newAttributes[i * attributeStride + j];
			outAttributes[j + attributeStride * 2] = newAttributes[(i + 1) * attributeStride + j];
		}

		DrawTriangle(out.get(), outAttributes.get());
	}
}
