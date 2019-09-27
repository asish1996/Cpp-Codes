#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
//#include "graphics.h"
#include "fssimplewindow.h"
using namespace std;

const double YS_PI = 3.1415927;

void DrawCannonBall(double cx, double cy)
{

	double rad = 3.0;
	double fill = 1.0;
	glColor3ub(255, 0, 0);
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

void DrawSquare(double cx, double cy, double size, double fill) {
	double x1 = cx - (size / 2.0);
	double x2 = cx + (size / 2.0);
	double y1 = cy - (size / 2.0);
	double y2 = cy + (size / 2.0);
	if (0 != fill)
	{
		glBegin(GL_QUADS);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}
	glVertex2d(x1, y1);
	glVertex2d(x2, y1);
	glVertex2d(x2, y2);
	glVertex2d(x1, y2);
	glEnd();
}

void DrawLine(double length, double angle) {
	double x2, y2;
	x2 = 50 + length * cos(angle);
	y2 = 550 + length * sin(angle);
	glColor3ub(0, 0, 255);
	glBegin(GL_LINES);
	glVertex2d(50, 550);
	glVertex2d(x2, y2);
	glEnd();
}

void DrawCannon(double angle) {
	double cx = 50.0;
	double cy = 550.0;
	double size = 10.0;
	glColor3ub(0, 0, 255);
	DrawSquare(cx, cy, size, 1);
	DrawLine(20,angle);
}

void DrawTarget(double cx, double cy) {
	double size = 50;
	glColor3ub(255, 0, 0);
	DrawSquare(cx, cy, size, 1);
}

void MoveTarget(int& x, int& y)
{
	y += 2;
	if (y > 600)
	{
		y = 0;
	}
}

void MoveCannonShooter(double& angle) {
	angle -= YS_PI / 720;
	if (angle < (-YS_PI / 2)) {
		angle = 0;
	}
}



void MoveCannonball(double& x, double& y, double& vx, double& vy, double m, double dt)
{
	double fx, fy, ax, ay;

	x = x + vx * dt;
	y = y + vy * dt;

	fx = 0.0;
	fy = -m * 9.8;  // Gravity

	ax = fx / m;
	ay = fy / m;

	vx = vx + dt * ax;
	vy = vy + dt * ay;
}


void DrawObstacles(double x1,double x2, double x3, double x4, double x5, double y1, double y2, double y3, double y4, double y5, double size1, double size2, double size3, double size4, double size5, int fill1, int fill2, int fill3, int fill4, int fill5) {
	
	glColor3ub(0, 255, 0);
	DrawSquare(x1, y1, size1, fill1);
	DrawSquare(x2, y2, size2, fill2);
	DrawSquare(x3, y3, size3, fill3);
	DrawSquare(x4, y4, size4, fill4);
	DrawSquare(x5, y5, size5, fill5);
}

void BounceBall(double& x, double& y, double& vx, double& vy)
{
	if (y < 0)
	{
		vy = -vy;
		y = -y;
	}

	/*else if (vy < 0)
	{
		vy = -vy;
	}*/
}


int CheckHitTarget(double cx, double cy, double sx, double sy) {
	double target_size = 50;
	if (cy <= sy + (target_size / 2) && cy >= sy - (target_size / 2) && cx<=sx+(target_size/2) && cx>=sx-(target_size/2)) {
		return 1;
	}
	else {
		return 0;
	}
}

void CheckHitObstacle(double& x1, double& x2, double& x3, double& x4, double& x5, double& y1, double& y2, double& y3, double& y4, double& y5, double& size1, double& size2, double& size3, double& size4, double& size5, int& fill1, int& fill2, int& fill3, int& fill4, int& fill5, double cx, double cy) {
	if (cy <= y1 + (size1 / 2.0) && cy >= y1 - (size1 / 2.0) && cx <= x1 + (size1 / 2.0) && cx >= x1 - (size1 / 2.0)) {
		fill1 = 0;
	}
	if (cy <= y2 + (size2 / 2.0) && cy >= y2 - (size2 / 2.0) && cx <= x2 + (size2 / 2.0) && cx >= x2 - (size2 / 2.0)) {
		fill2 = 0;
	}
	if (cy <= y3 + (size3 / 2.0) && cy >= y3 - (size3 / 2.0) && cx <= x3 + (size3 / 2.0) && cx >= x3 - (size3 / 2.0)) {
		fill3 = 0;
	}
	if (cy <= y4 + (size4 / 2.0) && cy >= y4 - (size4 / 2.0) && cx <= x4 + (size4 / 2.0) && cx >= x4 - (size4 / 2.0)) {
		fill4 = 0;
	}
	if (cy <= y5 + (size5 / 2.0) && cy >= y5 - (size5 / 2.0) && cx <= x5 + (size5 / 2.0) && cx >= x5 - (size5 / 2.0)) {
		fill5 = 0;
	}
}

int main(void) {

	double x1, x2, x3, x4, x5, y1, y2, y3, y4, y5, size1, size2, size3, size4, size5;
	srand(time(NULL));
	x1 = (rand() % 5 + 2.0) * 100.0;
	y1 = (rand() % 5 + 1.0) * 100.0;
	x2 = (rand() % 5 + 2.0) * 100.0;
	x3 = (rand() % 5 + 2.0) * 100.0;
	x4 = (rand() % 5 + 2.0) * 100.0;
	x5 = (rand() % 5 + 2.0) * 100.0;
	y2 = (rand() % 5 + 1.0) * 100.0;
	y3 = (rand() % 5 + 1.0) * 100.0;
	y4 = (rand() % 5 + 1.0) * 100.0;
	y5 = (rand() % 5 + 1.0) * 100.0;
	size1 = (rand() % 8 + 8.0) * 10.0;
	size2 = (rand() % 8 + 8.0) * 10.0;
	size3 = (rand() % 8 + 8.0) * 10.0;
	size4 = (rand() % 8 + 8.0) * 10.0;
	size5 = (rand() % 8 + 8.0) * 10.0;
	int fill1 = 1;
	int fill2 = 1;
	int fill3 = 1;
	int fill4 = 1;
	int fill5 = 1;

	int terminate = 0;
	int ball = 0;
	int tx = 775;
	int ty = 575;
	double angle = (-YS_PI / 6);
	double velocity = 40;
	FsOpenWindow(16, 16, 800, 600, 1);
	double cx = 50.0;
	double cy = 550.0;
	double theta = -YS_PI/6;
	double vx = 100.0 * cos(-YS_PI/6);
	double vy = 100.0 * sin(-YS_PI/6);
	
	while (0 == terminate) {

		

		FsPollDevice();
		int key = FsInkey();
		switch (key)
		{
		case FSKEY_ESC:
			terminate = 1;
			break;
		
		case FSKEY_SPACE:
			if (0 == ball)
			{
				ball = 1;
				theta = -YS_PI/6;
				cx = 50.0;
				cy = 550;				
			}
			
			break;
			
		}
		
		glClear(GL_DEPTH_BUFFER_BIT |	GL_COLOR_BUFFER_BIT);
		DrawCannon(angle);
		DrawCannonBall(cx, cy);
		MoveCannonShooter(angle);
		DrawTarget(tx, ty);
		DrawObstacles(x1,x2,x3,x4,x5,y1,y2,y3,y4, y5, size1, size2, size3, size4, size5,fill1,fill2,fill3,fill4,fill5);
		MoveTarget(tx, ty);
		theta = angle;
		//double vx = 100.0 * cos(theta);
		//double vy = 100.0 * sin(theta);
		if (ball != 0) {
			
			if (cx >= 800 || cy < 0) {
				ball = 0;
			}
			if (CheckHitTarget(cx, cy, tx, ty)==1) {
				terminate = 1;
				//cout << "GameOver" << endl;
			}
			//double t = 0;
			MoveCannonball(cx, cy, vx, vy,1.0,0.1);
			BounceBall(cx, cy, vx, vy);
		}
		CheckHitObstacle(x1, x2, x3, x4, x5, y1, y2, y3, y4, y5, size1, size2, size3, size4, size5, fill1, fill2, fill3, fill4, fill5, cx, cy);
		FsSwapBuffers();
		FsSleep(25);
	}

	return 0;

}


