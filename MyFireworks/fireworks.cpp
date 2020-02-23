/*
	程序名称：
			烟花(Fireworks)
	简介：
			使用EasyX实现烟花效果。
			用物理公式模拟的过程中，1像素等于10cm，坐标原点在屏幕左上角，x轴向右，y轴向下。
			为了实现立体效果，引入z轴，z轴方向垂直屏幕向里。

	实现思路：
			一共有四个类，分别是计时器(Timer)，发射后爆炸前的烟花(LightLine)，爆炸后烟花(Fireworks)，
			以及由粒子组成的条状烟花(ParticleSwarm)。其中，Timer用来提供总时间和每一帧的时间间隔，
			LightLine用来模拟发射后爆炸前烟花的动画，Fireworks用来模拟爆炸后的烟花动画，ParticleSwarm
			用来模拟爆炸后烟花的一部分，也就是ParticleSwarm组成了Fireworks。

	环境：
			VS2017 + EasyX_20200109(beta)

	时间：
			2020.2.22

	作者：
			gaiyitp9

	参考自Teternity(QQ:1926594835)，原地址https://codebus.cn/teternity/post/Fireworks
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
const double G = 9.8;									// 重力加速度(m/(s*s))
const double PI = 3.1415926;
const int lenMax = 80;									// 烟花的最大长度(pixels)。烟花长度与速度成正比。
const int heigthMax = screenHeight - lenMax;			/* 烟花发射的最大高度(pixels)，即烟花发射位置到屏幕顶端的距离。
														   烟花以最大速度发射时，长度为lenMax，为了保证不同速度的烟花都能完整绘制，
														   所以烟花发射位置的y值至少要为heigthMax */
const double velocityMax = sqrt(2 * G * heigthMax / 10.0);	/* 烟花发射的初始速率(m/s)的最大值。
														   1pixel等于现实中10cm，使得烟花发射之后飞行时间在3s内。
														   根据牛顿第二定律可推出v * v = 2 * a * x, 两边开方得到左边的式子*/
const int maxFireworks = 5;								// 屏幕中烟花的最大数量

//===========================================
// Timer
//===========================================
class Timer
{
public:
	Timer() { reset(); }
	void tick();				// 更新时间间隔
	void reset();				// 计时器归零
	void update();				// 更新计时器
	double deltaTime() const;	// 两帧之间的时间间隔
	double totalTime() const;	// 总时间
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
// 全局计时器
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
	bool overLine() const { return mY < heigthMax * maxFireworks / (maxFireworks + 1); }  // 判断烟花是否到达给定的高度
	int getX() const { return mX; }
	int getY() const { return mY; }

private:
	int mX = rand() % (screenWidth - 80) + 40;	// 烟花的x坐标，[40, screenWidth-40)之间随机值
	int mY = heigthMax;							// 烟花的y坐标，烟花发射的最大高度，原因在heigthMax定义处进行了说明
	int mLen;									// 烟花的长度
	double velocity = -(rand() % 20 + 76.0) / 100.0 * velocityMax;	/* 烟花的速度，[0.76, 0.96)之间的随机值乘以最大速度
																	   注意，烟花速度的方向与y轴方向相反 */
};

LightLine::LightLine()
{
	mLen = int(abs(velocity) / velocityMax * lenMax);
}

void LightLine::draw() const
{
	for (int j = mY; j < mY + mLen; j++)
	{
		float vHSV = 0.8f * (mLen - (j - mY)) / mLen + 0.2f;	// 设置颜色梯度，HSV中v的值在[0.2, 1]线性变化
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
	velocity = preVelocity + G * deltaTime;		// 更新烟花的速度，根据v1 = v0 + a * (t1 - t0)得到

	// 更新烟花的y轴坐标
	if (velocity < 0)
	{
		mY += int(10 * (velocity * velocity - preVelocity * preVelocity) / 2 / G);		// x = (v1*v1 - v0*v0)/(2*a)
	}
	else
	{
		mY += int(10 * (-preVelocity * preVelocity) / 2 / G);	// 同上，velocity为0
		velocity = 0;		
	}

	mLen = int(abs(velocity) / velocityMax * lenMax);	// 更新烟花的长度

	draw();		// 绘制烟花
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
	float hHSV;		// 颜色参数
	std::list<Particle> vec;
};

ParticleSwarm::ParticleSwarm(int x, int y, float h = float(rand() % 360))
{
	hHSV = h + rand() % 20;									/* 烟花的色相。这里加上一个[0, 20)的随机数是针对单色的烟花，
															   使单色的烟花不完全是一种颜色。*/
	hHSV = hHSV >= 360 ? hHSV - 360 : hHSV;					// 保证色相在[0, 360)之间

	double v = velocityMax * (rand() % 5 + 15.0) / 40.0;	// 粒子群的速率在[0.375, 0.5)*velocityMax之间。

	double theta = (rand() % 90) * PI / 180 + PI / 2;		/* 粒子群速度v与y轴的夹角θ，在[90, 180)之间，
															   这表示烟花爆炸后，粒子群在y轴的速度分量指向屏幕上方，与y轴相反*/
	double phi = (rand() % 360) * PI / 180;					// 粒子群速度在xz平面的速度分量与x轴的夹角ψ，在[0, 360)之间

	mVelocityX = v * sin(theta) * cos(phi);					// 粒子群在x轴方向上的速度
	mVelocityZ = v * sin(theta) * sin(phi);					// 粒子群在z轴方向上的速度
	double velocityY = v * cos(theta);						// 粒子群在y轴方向上的速度，这里均表示速度，不仅是数值，带有符号

	int num = rand() % 30 + 50;								// 粒子群中包含的粒子数，在[30, 50)之间
	while (num)
	{
		/* 粒子群构成爆炸的烟花中的一条烟花，在一条烟花中，离爆炸点最近的粒子先消失，最远的粒子最后消失。
		   这里根据粒子的编号来分配每个粒子离爆炸点的距离，粒子在list中储存，处于尾部的粒子离爆炸点最近*/
		double deltaTime = num / 200.0;			// 根据编号来分配每一个粒子运动时间
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
			float vHSV = 0.2f + 0.8f * (size - n) / size - particle.mZ / 40 * 0.1f;		/* 不同粒子设置不同明度。
																						   z越小，说明离观察者越近，明度越大；反之越小 */
			vHSV = vHSV > 0 ? (vHSV < 1 ? vHSV : 1) : 0;		// 保证明度在[0, 1]之间
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
		vec.pop_back();				// 离爆炸中心最近的粒子先消失

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
// Fireworks，由ParticleSwarm组成的烟花
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
	std::list<decltype(vec.begin())> toDel;		// 待移除的ParticleSwarm
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
		timer.update();		// 更新计时器
		if (timer.totalTime() - elapsed > 0.05)			// 限制画面刷新率为20帧
		{
			cleardevice();
			elapsed = timer.totalTime();
			timer.tick();	// 更新两帧的时间间隔

			std::list<decltype(vecLightLine.begin())> toDel1;
			if (vecLightLine.size() == 0)
				vecLightLine.push_back(LightLine());
			// 爆炸前的烟花数小于最大烟花数，且最后发射的烟花达到一定高度后才发射下一个烟花
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