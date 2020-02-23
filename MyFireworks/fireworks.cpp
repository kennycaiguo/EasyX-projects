/*
	�������ƣ�
			�̻�(Fireworks)
	��飺
			ʹ��EasyXʵ���̻�Ч����
			������ʽģ��Ĺ����У�1���ص���10cm������ԭ������Ļ���Ͻǣ�x�����ң�y�����¡�
			Ϊ��ʵ������Ч��������z�ᣬz�᷽��ֱ��Ļ���

	ʵ��˼·��
			һ�����ĸ��࣬�ֱ��Ǽ�ʱ��(Timer)�������ըǰ���̻�(LightLine)����ը���̻�(Fireworks)��
			�Լ���������ɵ���״�̻�(ParticleSwarm)�����У�Timer�����ṩ��ʱ���ÿһ֡��ʱ������
			LightLine����ģ�ⷢ���ըǰ�̻��Ķ�����Fireworks����ģ�ⱬը����̻�������ParticleSwarm
			����ģ�ⱬը���̻���һ���֣�Ҳ����ParticleSwarm�����Fireworks��

	������
			VS2017 + EasyX_20200109(beta)

	ʱ�䣺
			2020.2.22

	���ߣ�
			gaiyitp9

	�ο���Teternity(QQ:1926594835)��ԭ��ַhttps://codebus.cn/teternity/post/Fireworks
*/

//===========================================
// head files
//===========================================
#include <easyx.h>
#include <cmath>
#include <ctime>
#include <list>

//===========================================
// global variables
//===========================================
const int screenWidth = 640;
const int screenHeight = 480;
const double G = 9.8;									// �������ٶ�(m/(s*s))
const double PI = 3.1415926;
const int lenMax = 80;									// �̻�����󳤶�(pixels)���̻��������ٶȳ����ȡ�
const int heigthMax = screenHeight - lenMax;			/* �̻���������߶�(pixels)�����̻�����λ�õ���Ļ���˵ľ��롣
														   �̻�������ٶȷ���ʱ������ΪlenMax��Ϊ�˱�֤��ͬ�ٶȵ��̻������������ƣ�
														   �����̻�����λ�õ�yֵ����ҪΪheigthMax */
const double velocityMax = sqrt(2 * G * heigthMax / 10.0);	/* �̻�����ĳ�ʼ����(m/s)�����ֵ��
														   1pixel������ʵ��10cm��ʹ���̻�����֮�����ʱ����3s�ڡ�
														   ����ţ�ٵڶ����ɿ��Ƴ�v * v = 2 * a * x, ���߿����õ���ߵ�ʽ��*/
const int maxFireworks = 5;								// ��Ļ���̻����������

//===========================================
// Timer
//===========================================
class Timer
{
public:
	Timer() { reset(); }
	void tick();				// ����ʱ����
	void reset();				// ��ʱ������
	void update();				// ���¼�ʱ��
	double deltaTime() const;	// ��֮֡���ʱ����
	double totalTime() const;	// ��ʱ��
private:	
	clock_t mCurrTime;
	clock_t mPrevTime;
	double mDeltaTime;
	double mTotalTime;
};

void Timer::tick()
{
	mCurrTime = clock();
	mDeltaTime = (double)(mCurrTime - mPrevTime) / CLOCKS_PER_SEC;
	mPrevTime = mCurrTime;
}

void Timer::reset()
{
	mCurrTime = 0;
	mPrevTime = 0;
	mDeltaTime = 0;
	mTotalTime = 0;
}

void Timer::update()
{
	mCurrTime = clock();
	mTotalTime = (double)mCurrTime / CLOCKS_PER_SEC;
}

double Timer::deltaTime() const
{
	return mDeltaTime;
}

double Timer::totalTime() const
{
	return mTotalTime;
}
// ȫ�ּ�ʱ��
Timer timer;

//===========================================
// light line
//===========================================
class LightLine
{
public:
	LightLine();
	void draw() const;
	void move();
	bool stopped() const { return velocity == 0; }
	bool overLine() const { return mY < heigthMax * maxFireworks / (maxFireworks + 1); }  // �ж��̻��Ƿ񵽴�����ĸ߶�
	int getX() const { return mX; }
	int getY() const { return mY; }

private:
	int mX = rand() % (screenWidth - 80) + 40;	// �̻���x���꣬[40, screenWidth-40)֮�����ֵ
	int mY = heigthMax;							// �̻���y���꣬�̻���������߶ȣ�ԭ����heigthMax���崦������˵��
	int mLen;									// �̻��ĳ���
	double velocity = -(rand() % 20 + 76.0) / 100.0 * velocityMax;	/* �̻����ٶȣ�[0.76, 0.96)֮������ֵ��������ٶ�
																	   ע�⣬�̻��ٶȵķ�����y�᷽���෴ */
};

LightLine::LightLine()
{
	mLen = int(abs(velocity) / velocityMax * lenMax);
}

void LightLine::draw() const
{
	for (int j = mY; j < mY + mLen; j++)
	{
		float vHSV = 0.8f * (mLen - (j - mY)) / mLen + 0.2f;	// ������ɫ�ݶȣ�HSV��v��ֵ��[0.2, 1]���Ա仯
		setfillcolor(HSVtoRGB(0, 1.0f, vHSV));
		solidcircle(mX, j, 1);
	}
}

void LightLine::move()
{
	if (velocity == 0)
		return;
	
	double deltaTime = timer.deltaTime();

	double preVelocity = velocity;
	velocity = preVelocity + G * deltaTime;		// �����̻����ٶȣ�����v1 = v0 + a * (t1 - t0)�õ�

	// �����̻���y������
	if (velocity < 0)
	{
		mY += int(10 * (velocity * velocity - preVelocity * preVelocity) / 2 / G);		// x = (v1*v1 - v0*v0)/(2*a)
	}
	else
	{
		mY += int(10 * (-preVelocity * preVelocity) / 2 / G);	// ͬ�ϣ�velocityΪ0
		velocity = 0;		
	}

	mLen = int(abs(velocity) / velocityMax * lenMax);	// �����̻��ĳ���

	draw();		// �����̻�
}

//===========================================
// ParticleSwarm
//===========================================
class ParticleSwarm
{
	struct Particle
	{
		int mX;
		int mY;
		int mZ;
		double mVelocityY;
		Particle(int x, int y, int z, double v) 
			: mX(x), mY(y), mZ(z), mVelocityY(v) {}
	};

public:
	ParticleSwarm(int x, int y, float h);
	void draw() const;
	void move();
	bool finish() const { return vec.size() <= 1; }

private:
	double mVelocityX;
	double mVelocityZ;
	float hHSV;		// ��ɫ����
	std::list<Particle> vec;
};

ParticleSwarm::ParticleSwarm(int x, int y, float h = float(rand() % 360))
{
	hHSV = h + rand() % 20;									/* �̻���ɫ�ࡣ�������һ��[0, 20)�����������Ե�ɫ���̻���
															   ʹ��ɫ���̻�����ȫ��һ����ɫ��*/
	hHSV = hHSV >= 360 ? hHSV - 360 : hHSV;					// ��֤ɫ����[0, 360)֮��

	double v = velocityMax * (rand() % 5 + 15.0) / 40.0;	// ����Ⱥ��������[0.375, 0.5)*velocityMax֮�䡣

	double theta = (rand() % 90) * PI / 180 + PI / 2;		/* ����Ⱥ�ٶ�v��y��ļнǦȣ���[90, 180)֮�䣬
															   ���ʾ�̻���ը������Ⱥ��y����ٶȷ���ָ����Ļ�Ϸ�����y���෴*/
	double phi = (rand() % 360) * PI / 180;					// ����Ⱥ�ٶ���xzƽ����ٶȷ�����x��ļнǦף���[0, 360)֮��

	mVelocityX = v * sin(theta) * cos(phi);					// ����Ⱥ��x�᷽���ϵ��ٶ�
	mVelocityZ = v * sin(theta) * sin(phi);					// ����Ⱥ��z�᷽���ϵ��ٶ�
	double velocityY = v * cos(theta);						// ����Ⱥ��y�᷽���ϵ��ٶȣ��������ʾ�ٶȣ���������ֵ�����з���

	int num = rand() % 30 + 50;								// ����Ⱥ�а���������������[30, 50)֮��
	while (num)
	{
		/* ����Ⱥ���ɱ�ը���̻��е�һ���̻�����һ���̻��У��뱬ը���������������ʧ����Զ�����������ʧ��
		   ����������ӵı��������ÿ�������뱬ը��ľ��룬������list�д��棬����β���������뱬ը�����*/
		double deltaTime = num / 200.0;			// ���ݱ��������ÿһ�������˶�ʱ��
		int xx = x + int(10 * mVelocityX * deltaTime);
		int zz = int(10 * mVelocityZ * deltaTime);
		double vy = velocityY + G * deltaTime;
		int yy = y + int(10 * (vy * vy - velocityY * velocityY) / 2 / G);
		vec.push_back(Particle(xx, yy, zz, vy));
		--num;
	}
}

void ParticleSwarm::draw() const
{
	int n = 0;
	int size = vec.size();
	for (auto & particle : vec)
	{
		if (particle.mX >= 0 && particle.mX <= screenWidth && particle.mY <= screenHeight)
		{
			float vHSV = 0.2f + 0.8f * (size - n) / size - particle.mZ / 40 * 0.1f;		/* ��ͬ�������ò�ͬ���ȡ�
																						   zԽС��˵����۲���Խ��������Խ�󣻷�֮ԽС */
			vHSV = vHSV > 0 ? (vHSV < 1 ? vHSV : 1) : 0;		// ��֤������[0, 1]֮��
			auto color = HSVtoRGB(hHSV, 1.0f, vHSV);
			if (particle.mZ < 0)
			{
				setfillcolor(color);
				solidcircle(particle.mX, particle.mY, abs(particle.mZ) / 80 > 1 ? 2 : 1);
			}
			else
				putpixel(particle.mX, particle.mY, color);
		}
		++n;
	}
}

void ParticleSwarm::move()
{
	double deltaTime = timer.deltaTime();

	for (int i = 0; i < 3 && vec.size() > 1; i++)
		vec.pop_back();				// �뱬ը�����������������ʧ

	for (auto & particle : vec)
	{
		double currentVY = particle.mVelocityY + G * deltaTime;
		particle.mX += int(10 * mVelocityX * deltaTime);
		particle.mZ += int(10 * mVelocityZ * deltaTime);
		particle.mY += int(10 * (currentVY * currentVY - particle.mVelocityY * particle.mVelocityY) / 2 / G);
		particle.mVelocityY = currentVY;
	}
	draw();
}

//============================================
// Fireworks����ParticleSwarm��ɵ��̻�
//============================================
class Fireworks
{
public:
	Fireworks(int x, int y);
	void move();
	bool empty() const { return vec.empty(); }

private:
	std::list<ParticleSwarm> vec;
};

Fireworks::Fireworks(int x, int y)
{
	bool colorful = rand() % 100 < 20 ? true : false;
	float h = float(rand() % 360);

	int n = rand() % 5 + 45;
	for (int i = 0; i < n; i++)
	{
		if (colorful)
			vec.push_back(ParticleSwarm(x, y));
		else
			vec.push_back(ParticleSwarm(x, y, h));
	}
}

void Fireworks::move()
{
	std::list<decltype(vec.begin())> toDel;		// ���Ƴ���ParticleSwarm
	for (auto it = vec.begin(); it != vec.end(); ++it)
	{
		if (it->finish())
		{
			toDel.push_back(it);
			continue;
		}
		it->move();
	}
	for (auto & x: toDel)
		vec.erase(x);
}

/*************************************************/
int main()
{
	initgraph(screenWidth, screenHeight, SHOWCONSOLE);
	setrop2(R2_MERGEPEN);
	srand((unsigned)time(nullptr));

	double elapsed = 0;		

	std::list<LightLine> vecLightLine;
	vecLightLine.push_back(LightLine());
	std::list<Fireworks> vecFireworks;

	BeginBatchDraw();
	while (!(GetAsyncKeyState(VK_ESCAPE) & 0x8000))
	{
		timer.update();		// ���¼�ʱ��
		if (timer.totalTime() - elapsed > 0.05)			// ���ƻ���ˢ����Ϊ20֡
		{
			cleardevice();
			elapsed = timer.totalTime();
			timer.tick();	// ������֡��ʱ����

			std::list<decltype(vecLightLine.begin())> toDel1;
			if (vecLightLine.size() == 0)
				vecLightLine.push_back(LightLine());
			// ��ըǰ���̻���С������̻��������������̻��ﵽһ���߶Ⱥ�ŷ�����һ���̻�
			else if (vecLightLine.size() < maxFireworks && rand() % 100 < 10 && (--vecLightLine.end())->overLine())
				vecLightLine.push_back(LightLine());
			for (auto it = vecLightLine.begin(); it != vecLightLine.end(); ++it)
			{
				if (it->stopped())
				{
					vecFireworks.push_back(Fireworks(it->getX(), it->getY()));
					toDel1.push_back(it);
					continue;
				}
				it->move();
			}
			for (auto & it : toDel1)
				vecLightLine.erase(it);

			std::list<decltype(vecFireworks.begin())> toDel2;
			for (auto it = vecFireworks.begin(); it != vecFireworks.end(); ++it)
			{
				if (it->empty())
				{
					toDel2.push_back(it);
					continue;
				}
				it->move();
			}
			for (auto & it : toDel2)
				vecFireworks.erase(it);

			FlushBatchDraw();
		}
	}
	EndBatchDraw();

	closegraph();
	return 0;
}