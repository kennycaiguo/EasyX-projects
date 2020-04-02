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
	// Repeatģʽ����
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

	// ����������ᣬ�����������Ϊ�ӵ�->���
	Vector3 cameraDirection = eye - target;
	cameraDirection.Normalize();
	Vector3 cameraRight = Vector3::Cross(up, cameraDirection);
	cameraRight.Normalize();
	Vector3 cameraUp = Vector3::Cross(cameraDirection, cameraRight); 
	cameraUp.Normalize();

	// ��ʾ�������ƽ�ƣ�ƽ�ƾ���������
	Matrix4 inverseTranslation = {
		1, 0, 0, -eye.x,
		0, 1, 0, -eye.y,
		0, 0, 1, -eye.z,
		0, 0, 0, 1
	}; 

	// ��ʾ�����������ת,����Ϊһ��������������������ת�þ���
	Matrix4 transposeRotate = 
	{
		cameraRight.x,	   cameraRight.y,	  cameraRight.z,	 0,
		cameraUp.x,		   cameraUp.y,		  cameraUp.z,		 0,
		cameraDirection.x, cameraDirection.x, cameraDirection.x, 0,
		0,				   0,				  0,				 1
	};

	// �۲����
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

	int yMin = (int)points[0].y;			// �����yֵ����Сֵ
	int yMax = (int)points[0].y;			// �����yֵ�����ֵ
	// ��ʼ��new edge table�±߱�������εı߹��ɵ�����
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

	// ����AEL(active edge list)��������ɨ�����ཻ���߶�
	std::list<Edge> activeEdge;
	for (int y = yMin; y <= yMax; y++)
	{
		if (!newEdgeTable[y].empty())						// ���ɨ���д��ڱߣ������AEL��
		{
			activeEdge.splice(activeEdge.end(), newEdgeTable[y]);
			activeEdge.sort(sortEdge);						// ��Ϊ�����β�����������߽������������Գ����±ߺ����Ҫ��������
		}

		std::list<Edge>::iterator edgeStar, edgeEnd;		// ɨ������Ҫ���Ƶ��߶εĶ˵�ĵ�����
		int countEdge = 0;									// �Ѵ洢�Ķ˵�

		for (auto it = activeEdge.begin(); it != activeEdge.end(); )
		{
			// ɨ�����Ѿ�����ĳ���ߵ�y�����ֵ�����Ƴ��ñ�
			if ((int)it->yMax <= y)
			{
				it = activeEdge.erase(it);					// �����¼�Ƴ�Ԫ�ص���һ��Ԫ�صĵ�������������it++����Ϊit�Ѿ���Ч�ˡ�
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
					// �����߶�
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

						// ��z���������бȽ�
						if (z > zBuffer[x + screenWidth * y])
							continue;
						else
							zBuffer[x + screenWidth * y] = z;

						//�Զ�������Խ��в�ֵ
						for (size_t i = 0; i < attributeStride; i++)
						{
							interpolationAttributes[i] = omega * (attributes[i] / points[0].w * weightA +
								attributes[i + attributeStride] / points[1].w * weightB +
								attributes[i + attributeStride * 2] / points[2].w * weightC);
						}

						COLORREF c;
						FragmentShader(interpolationAttributes.get(), attributeStride, c);	//����ƬԪ��ɫ��
						FastPutPixel(x, y, c);								//�����ɫ
					}
					// ��Խ����xֵ����dx
					edgeStar->x += edgeStar->dx;
					edgeEnd->x += edgeEnd->dx;
				}
				it++;
			}
		}
	}
}
//��ֱ�߽��вü�,����-1��ʾƽ���ܾ�������0��ʾƽ�����ܣ�����1��ʾ��A����вü�������2��ʾ��B����вü��������������result��t��ʾ�ü��������tֵ
//���в���flag��0��5�ֱ��ʾ�߽�Ϊleft��right��bottom��top��near��far
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
	std::vector<Point4> points(triangle, triangle + 3);								 // �ü��󶥵�����
	std::vector<double> newAttributes(attributes, attributes + attributeStride * 3); // �ü��󶥵�����

	// ����׵��������ν��вü����ü�������Suthland-Hodgman�㷨����
	for (int clipflag = 0; clipflag < 6; clipflag++)
	{
		std::vector<Point4> temp;		// ���ڴ��汾�βü�֮��Ķ���
		std::vector<double> tempAttri;	// ���ڴ��汾�βü�֮��Ķ�������
		int size = points.size();
		for (int i = 0; i < size; i++)
		{
			Point4 result;				// �߽��������αߵĽ���
			double t;					// �������Բ�ֵ��ϵ��
			size_t indexA = i, indexB = (i + 1) % size;
			int ret = clipEdge(clipflag, points[indexA], points[indexB], result, t);
			// ƽ������
			if (ret == 0)
			{
				temp.push_back(points[indexB]);
				for (size_t j = 0; j < attributeStride; j++)
					tempAttri.push_back(newAttributes[indexB * attributeStride + j]);
			}
			// �ü�A�㣬��������B��
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
			// �ü�B�㣬�������
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
	// ���ü����ͼ�ηֽ�ɶ�������ν��л��ƣ��ǵ�Ҫ�Ƚ���͸�ӳ���������任��CVV�ռ�
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
			outAttributes[j] = newAttributes[j];	// points�е�һ����������ÿ�������εĵ�һ������
			outAttributes[j + attributeStride] = newAttributes[i * attributeStride + j];
			outAttributes[j + attributeStride * 2] = newAttributes[(i + 1) * attributeStride + j];
		}

		DrawTriangle(out.get(), outAttributes.get());
	}
}
