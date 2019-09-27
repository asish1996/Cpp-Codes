
#include <stdio.h>
#include <iostream>
#include <stdlib.h>
#include<math.h>
#include "fssimplewindow.h"
using namespace std;

void DrawLine(double x1, double y1, double x2, double y2) {

	glColor3ub(1.0, 0.0, 0.0);
	glBegin(GL_LINES);
	glVertex2d(x1, y1);
	glVertex2d(x2, y2);
	glEnd();
}

void DrawClock(int hour, int minute) {
	const double PI = 3.1415927;
	int cx = 400;
	int cy = 300;
	int rad = 250; //radius of clock
	int fill = 0;

	if (0 != fill)
	{
		glBegin(GL_POLYGON);
	}
	else
	{
		glBegin(GL_LINE_LOOP);
	}

	for (int i = 0; i < 64; i++) {
		double angle = (double)i * PI / 32.0;
		double x = (double)cx + cos(angle) * (double)rad;
		double y = (double)cy + sin(angle) * (double)rad;
		glVertex2d(x, y);
	}
	glEnd();

	//Drawing lines
	int rad1 = 240;
	int rad2 = 200;
	int rad3 = 230;
	//Hours lines
	for (int i = 0; i < 12; i++) {
		double angle = (double)i * PI / 6;
		double x1 = (double)cx + cos(angle) * (double)rad1;
		double y1 = (double)cx + sin(angle) * (double)rad1;
		double x2 = (double)cx + cos(angle) * (double)rad2;
		double y2 = (double)cx + sin(angle) * (double)rad2;
		DrawLine(x1, y1-100, x2, y2-100);
	}
	//minutes lines
	for (int j = 0; j < 60; j++) {
		double angle1 = (double)j * PI / 30;
		double x11 = (double)cx + cos(angle1) * (double)rad1;
		double y11 = (double)cy + sin(angle1) * (double)rad1;
		double x22 = (double)cx + cos(angle1) * (double)rad3;
		double y22 = (double)cy + sin(angle1) * (double)rad3;
		DrawLine(x11, y11, x22, y22);
	}
	//hands
	int rad4 = 180; //length of hour hand
	int rad5 = 220; // length of minute hand
	double angle_hour = (double)hour * PI / 6;
	double angle_minute = (double)minute * PI / 30;
	double xh2 = (double)cx + cos(angle_hour - (PI/2.0)) * (double)rad4;
	double xm2 = (double)cx + cos(angle_minute - (PI/2.0)) * (double)rad5;
	double yh2 = (double)cy + sin(angle_hour - (PI / 2.0)) * (double)rad4;
	double ym2 = (double)cy + sin(angle_minute - (PI / 2.0)) * (double)rad5;
	//cout << xh2 << endl;
	DrawLine(cx, cy, xh2, yh2);
	DrawLine(cx, cy, xm2, ym2);
}

int main() {
	cout << "Please enter hours followed by minutes\n" << endl;
	int hour, minute;
	cin >> hour >> minute;

	FsOpenWindow(16, 16, 800, 600, 0);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	DrawClock(hour,minute);
	glFlush();

	while (FSKEY_NULL == FsInkey())
	{
		FsPollDevice();
		FsSleep(10);
	}

	return 0;
}

