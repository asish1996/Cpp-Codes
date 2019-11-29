#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <thread>
#include <mutex>
#include "fssimplewindow.h"

void DrawCircle(int cx, int cy, int rad, int fill)
{
	const double YS_PI = 3.1415927;

	if (0 != fill)
	{
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}

	int i;
	for (i = 0; i < 64; i++)
	{
		double angle = (double)i * YS_PI / 32.0;
		double x = (double)cx + cos(angle) * (double)rad;
		double y = (double)cy + sin(angle) * (double)rad;
		glVertex2d(x, y);
	}

	glEnd();
}

void Move(double& x, double& y, double& vx, double& vy, double m, double dt)
{
	double fx, fy, ax, ay;

	x += vx * dt;
	y += vy * dt;

	fx = 0.0;
	fy = -m * 9.8;  // Gravity

	ax = fx / m;
	ay = fy / m;

	vx += dt * ax;
	vy += dt * ay;
}

void Bounce(double& x, double& y, double& vx, double& vy)
{
	if (y < 1.0 && vy < 0.0)
	{
		vy = -vy;
	}

	if (x < -39.0 && vx < 0.0)
	{
		vx = -vx;
	}
	else if (x > 39.0 && vx > 0.0)
	{
		vx = -vx;
	}
}


void CalculateCollision(
	double x1, double y1, double& vx1, double& vy1,
	double x2, double y2, double& vx2, double& vy2,
	double radius)
{
	double dx = x2 - x1, dy = y2 - y1;
	double d = dx * dx + dy * dy;

	if (d < (radius * 2.0) * (radius * 2.0))
	{
		double nx, ny, nl;
		nx = x2 - x1;
		ny = y2 - y1;

		nl = nx * nx + ny * ny;
		if (0.0 < nl)
		{
			nl = sqrt(nl);
			nx /= nl;
			ny /= nl;

			double nv1, nv2;
			nv1 = (vx1 * nx + vy1 * ny);
			nv2 = (vx2 * nx + vy2 * ny);

			if ((nv2 > 0.0 && nv2 < nv1) || (nv1 < 0.0 && nv2 < nv1) || (nv1 > 0.0 && nv2 < 0.0)) // Means closing in.  Otherwise, two molecules are moving away.
			{
				vx1 = vx1 - nx * nv1 + nx * nv2;
				vy1 = vy1 - ny * nv1 + ny * nv2;

				vx2 = vx2 - nx * nv2 + nx * nv1;
				vy2 = vy2 - ny * nv2 + ny * nv1;
			}
		}
	}

}

class CollisionParam
{
public:
	int n;
	std::mutex mtx1;
	double* x, * y, * vx, * vy;

	int i0, i1;
};

void CollisionDetection(CollisionParam* param)
{
	// Assuming all ball has the same radius and same mass

	auto n = param->n;
	auto x = param->x;
	auto y = param->y;
	auto vx = param->vx;
	auto vy = param->vy;

	for (int i = param->i0; i < param->i1; i++)
	{
		for (int j = i + 1; j < n; j++)
		{
			double vx1, vy1, vx2, vy2;

			{
				vx1 = vx[i];
				vy1 = vy[i];
				vx2 = vx[j];
				vy2 = vy[j];
			}

			param->mtx1.lock();
			CalculateCollision(x[i], y[i], vx1, vy1, x[j], y[j], vx2, vy2, 1.0);
			param->mtx1.unlock();

			{
				vx[i] = vx1;
				vy[i] = vy1;
				vx[j] = vx2;
				vy[j] = vy2;
			}

		}
	}
}

class MoveParam
{
public:
	std::mutex mtx2;
	int n;
	double* x, * y, * vx, * vy, * m;
	double dt;

	int i0, i1;

};

void MoveAll(MoveParam* paramPtr)
{
	auto& param = paramPtr;
	auto n = param->n;
	auto x = param->x;
	auto y = param->y;
	auto vx = param->vx;
	auto vy = param->vy;
	auto m = param->m;
	auto dt = param->dt;

	for (int i = param->i0; i < param->i1; i++)
	{
		param->mtx2.lock();
		Move(x[i], y[i], vx[i], vy[i], m[i], dt);
		Bounce(x[i], y[i], vx[i], vy[i]);
		param->mtx2.unlock();		
	}
}

const int nBall = 200;
double m[nBall], x[nBall], y[nBall], vx[nBall], vy[nBall], dt;

int main(void)
{
	srand(time(NULL));

	std::mutex mutex;

	for (int i = 0; i < nBall; i++)
	{
		m[i] = 1.0;

		x[i] = (double)(rand() % 80 - 40);  // -40 to  39
		y[i] = (double)(rand() % 40 + 10);  //  10 to  49

		vx[i] = (double)(rand() % 40 - 20); // -20 to +19
		vy[i] = (double)(rand() % 40 - 20); // -20 to +19
	}

	dt = 0.02;

	FsOpenWindow(16, 16, 800, 600, 1);

	FsPollDevice();
	while (FSKEY_NULL == FsInkey())
	{
		FsPollDevice();

		MoveParam moveParam[2];
		moveParam[0].n = nBall;
		moveParam[0].x = x;
		moveParam[0].y = y;
		moveParam[0].vx = vx;
		moveParam[0].vy = vy;
		moveParam[0].m = m;
		moveParam[0].dt = dt;
		moveParam[0].i0 = 0;
		moveParam[0].i1 = nBall/2;
		std::thread th0(MoveAll,&moveParam[0]);

		moveParam[1].n = nBall;
		moveParam[1].x = x;
		moveParam[1].y = y;
		moveParam[1].vx = vx;
		moveParam[1].vy = vy;
		moveParam[1].m = m;
		moveParam[1].dt = dt;
		moveParam[1].i0 = nBall/2;
		moveParam[1].i1 = nBall;
		std::thread th1(MoveAll, &moveParam[1]);


		CollisionParam collParam[2];
		collParam[0].n = nBall;
		collParam[0].x = x;
		collParam[0].y = y;
		collParam[0].vx = vx;
		collParam[0].vy = vy;
		collParam[0].i0 = 0;
		collParam[0].i1 = nBall/2;
		std::thread th2(CollisionDetection, &collParam[0]);

		collParam[1].n = nBall;
		collParam[1].x = x;
		collParam[1].y = y;
		collParam[1].vx = vx;
		collParam[1].vy = vy;
		collParam[1].i0 = nBall / 2 ;
		collParam[1].i1 = nBall;
		std::thread th3(CollisionDetection, &collParam[1]);

		th0.join();
		th1.join();
		th2.join();
		th3.join();

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		glColor3ub(0, 0, 0);
		for (int i = 0; i < nBall; i++)
		{
			DrawCircle(400 + (x[i] * 10.0), 600 - y[i] * 10.0, 10, 1); // 10pixel=1.0m
		}
		FsSwapBuffers();

		FsSleep(20);
	}

	return 0;
}



