#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include<iostream>
#include "fssimplewindow.h"

const double targetW = 5.0;
const double targetH = 5.0;

class Artillery {

public:

	void PhysicalCoordToScreenCoord(int& sx, int& sy, double px, double py)
	{
		sx = (int)(px * 10.0);
		sy = 600 - (int)(py * 10.0);
	}
	
	void DrawRect(int x1, int y1, int x2, int y2, int fill)
	{
		if (0 != fill)
		{
			glBegin(GL_QUADS);
		}
		else
		{
			glBegin(GL_LINE_LOOP);
		}

		glVertex2i(x1, y1);
		glVertex2i(x2, y1);
		glVertex2i(x2, y2);
		glVertex2i(x1, y2);

		glEnd();
	}

	void DrawLine(int x1, int y1, int x2, int y2)
	{
		glBegin(GL_LINES);

		glVertex2i(x1, y1);
		glVertex2i(x2, y2);

		glEnd();
	}

	void Draw(double x, double y, double angle)
	{
		int sx, sy;
		PhysicalCoordToScreenCoord(sx, sy, x, y);

		glColor3ub(0, 0, 255);
		DrawRect(sx - 5, sy - 5, sx + 5, sy + 5, 1);

		int vx, vy;
		PhysicalCoordToScreenCoord(vx, vy, x + 3.0 * cos(angle), y + 3.0 * sin(angle));

		DrawLine(sx, sy, vx, vy);
	}
};

class CannonBall {

public:

	void PhysicalCoordToScreenCoord(int& sx, int& sy, double px, double py)
	{
		sx = (int)(px * 10.0);
		sy = 600 - (int)(py * 10.0);
	}

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

	void Draw(double x, double y)
	{
		int rad = 5;
		int sx, sy;
		PhysicalCoordToScreenCoord(sx, sy, x, y);

		glColor3ub(255, 0, 0);
		DrawCircle(sx, sy, rad, 1);
	}

	void Move(double& x, double& y, double& vx, double& vy, double m, double dt)
	{
		x += vx * dt;
		y += vy * dt;

		vy -= 9.8 * dt;
	}

	void Fire(
		double& cx, double& cy, double& vx, double& vy,
		double artX, double artY, double artAngle, double iniVel)
	{
		cx = artX;
		cy = artY;
		vx = iniVel * cos(artAngle);
		vy = iniVel * sin(artAngle);
	}

};

class Obstacle {

public:

	void PhysicalCoordToScreenCoord(int& sx, int& sy, double px, double py)
	{
		sx = (int)(px * 10.0);
		sy = 600 - (int)(py * 10.0);
	}

	void DrawRect(int x1, int y1, int x2, int y2, int fill)
	{
		if (0 != fill)
		{
			glBegin(GL_QUADS);
		}
		else
		{
			glBegin(GL_LINE_LOOP);
		}

		glVertex2i(x1, y1);
		glVertex2i(x2, y1);
		glVertex2i(x2, y2);
		glVertex2i(x1, y2);

		glEnd();
	}
	   
	void Draw(double obstacleX, double obstacleY, double obstacleW, double obstacleH, double fill)
	{
		glColor3ub(0, 255, 0);
		int x1, y1, x2, y2;
		PhysicalCoordToScreenCoord(x1, y1, obstacleX, obstacleY);
		PhysicalCoordToScreenCoord(x2, y2, obstacleX + obstacleW, obstacleY + obstacleH);
		DrawRect(x1, y1, x2, y2, fill);
	}

	void Generate(double obstacleX[], double obstacleY[], double obstacleW[], double obstacleH[], int i)
	{
		srand(time(NULL));
		obstacleX[i] = (double)(10 + rand() % 70);
		obstacleY[i] = (double)(rand() % 60);
		obstacleW[i] = (double)(8 + rand() % 8);
		obstacleH[i] = (double)(8 + rand() % 8);
		if (80.0 < obstacleX[i] + obstacleW[i])
		{
			obstacleX[i] = 80.0 - obstacleW[i];
		}
		if (60.0 < obstacleY[i] + obstacleH[i]) 
		{
			obstacleY[i] = 60.0 - obstacleH[i];
		}

	}

	int CheckHitByCannonBall(
		double missileX, double missileY,
		double targetX, double targetY, double targetSizeX, double targetSizeY)
	{
		double relativeX, relativeY;
		relativeX = missileX - targetX;
		relativeY = missileY - targetY;
		if (0 <= relativeX && relativeX < targetSizeX && 0 <= relativeY && relativeY < targetSizeY)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

};

class Target {
	
public:

	void Initialize(double &targetX, double& targetY) {

		targetX = 75.0;
		targetY = 60.0;

	}
	
	void PhysicalCoordToScreenCoord(int& sx, int& sy, double px, double py)
	{
		sx = (int)(px * 10.0);
		sy = 600 - (int)(py * 10.0);
	}

	void DrawRect(int x1, int y1, int x2, int y2, int fill)
	{
		if (0 != fill)
		{
			glBegin(GL_QUADS);
		}
		else
		{
			glBegin(GL_LINE_LOOP);
		}

		glVertex2i(x1, y1);
		glVertex2i(x2, y1);
		glVertex2i(x2, y2);
		glVertex2i(x1, y2);

		glEnd();
	}


	int CheckHitByCannonBall(double mx, double my, double targetX, double targetY) {
		
		double rad = 5.0;
		int relativeX, relativeY;
		relativeX = mx - targetX;
		relativeY = my - targetY;
		if(0 <= relativeX && relativeX < targetW && 0 <= relativeY && relativeY < targetH)
		{
			return 1;
		}
		else
		{
			return 0;
		}
	}

	void Draw(
		double targetX, double targetY, double targetW, double targetH)
	{
		int x1, y1, x2, y2;
		PhysicalCoordToScreenCoord(x1, y1, targetX, targetY);
		PhysicalCoordToScreenCoord(x2, y2, targetX + targetW, targetY + targetH);

		glColor3ub(255, 0, 0);
		DrawRect(x1, y1, x2, y2, 1);
	}

	void Move(double& targetX, double& targetY, const double dt)
	{
		targetY -= 10 * dt;
		if (0 > targetY)
		{
			targetY = 60.0;
		}
	}

};

const int nObstacle = 5;

int main(void)
{
	int i, key, nShot;
	double artX, artY, artAngle;
	const double PI = 3.1415927;

	artX = 5.0;
	artY = 5.0;
	artAngle = PI / 6.0;

	srand(time(NULL));

	int cannonState;
	double cannonX, cannonY, cannonVx, cannonVy;
	cannonState = 0;

	Obstacle obstacle;
	CannonBall cannonball;
	Target target;
	Artillery artillery;
	double targetX, targetY;
	target.Initialize(targetX,targetY);

	FsOpenWindow(16, 16, 800, 600, 1);

	int obstacleState[nObstacle] = {1,1,1,1,1};
	
	double obstacleX[nObstacle] = { 0.0 };
	double obstacleY[nObstacle] = { 0.0 };
	double obstacleW[nObstacle] = { 0.0 };
	double obstacleH[nObstacle] = { 0.0 };
	

	for (int i = 0; i < nObstacle-1; i++)
	{
		obstacle.Generate(obstacleX, obstacleY, obstacleW, obstacleH,i);
	}
		
	FsPollDevice();

	nShot = 0;
	while (FSKEY_ESC != (key = FsInkey()))
	{
		FsPollDevice();

		switch (key)
		{
		case FSKEY_SPACE:
			if (cannonState == 0)
			{
				cannonState = 1;
				cannonball.Fire(
					cannonX, cannonY, cannonVx, cannonVy,
					artX, artY, artAngle, 40.0);
				nShot++;
			}
			break;
		}

		artAngle += PI / 180.0;
		if (PI / 2.0 < artAngle)
		{
			artAngle = 0.0;
		}

		target.Move(targetX, targetY, 0.025);

		if (cannonState == 1)
		{
			cannonball.Move(cannonX, cannonY, cannonVx, cannonVy, 1.0, 0.02);
			if (cannonY < 0.0 || cannonX < 0.0 || 80.0 < cannonX)
			{
				cannonState = 0;
			}

			for (i = 0; i < nObstacle; i++)
			{
				if (obstacleState[i] == 1 &&
					obstacle.CheckHitByCannonBall(cannonX, cannonY, obstacleX[i], obstacleY[i], obstacleW[i], obstacleH[i]) == 1)
				{
					printf("Hit Obstacle!\n");
					obstacleState[i] = 0;
					cannonState = 0;
				}
			}

			
			if (target.CheckHitByCannonBall(cannonX,cannonY,targetX,targetY) == 1)
			{
				printf("Hit Target!\n");
				printf("You fired %d shots.\n", nShot);
				break;
			}
		}

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		artillery.Draw(artX, artY, artAngle);
		
		for (int i = 0; i < nObstacle; i++) 
		{
			obstacle.Draw(obstacleX[i],obstacleY[i],obstacleW[i],obstacleH[i],obstacleState[i]);
		}

		target.Draw(targetX, targetY, targetW, targetH);
		if (cannonState == 1)
		{
			cannonball.Draw(cannonX, cannonY);
		}
		
		FsSwapBuffers();

		FsSleep(25);
	}

	return 0;
}