#include <stdio.h>
#include<iostream>
#include "fssimplewindow.h"
#include<vector>

class GameObject {

public:

	GameObject(){}

	void DrawObject(int x, int y, int width, int height, int fill) 
	{
		if (0 != fill)
		{
			glBegin(GL_QUADS);
		}
		else
		{
			glBegin(GL_LINE_LOOP);
		}

		glColor3ub(0, 0, 0);
		glVertex2i(x - width/2, y - height/2);
		glVertex2i(x - width/2, y + height/2);
		glVertex2i(x + width/2, y + height/2);
		glVertex2i(x + width/2, y - height/2);

		glEnd();
	}

	virtual void HitTest(int& x, int& y)	//This function has no signifance here. Just to show inheritance it is kept here.
	{
		int vy = 0;
		vy = -vy;
	}

};

class Ball :public GameObject {

private:

	const int ball_width = 6;
	const int ball_height = 6;
	const int ball_fill = 1;
	
public:

	void Draw(int &x, int &y)
	{
		DrawObject(x, y, ball_width, ball_height, ball_fill);
	}

	void Move(int& x, int& y, int &vx, int &vy)
	{
		if ((x < 0 && vx < 0) || (800 < x && 0 < vx))
		{
			vx = -vx;
		}
		if ((y < 0 && vy < 0))
		{
			vy = -vy;
		}
		x += vx;
		y += vy;
		//Draw(x, y);
	}

	int Miss(int& y)
	{
		if (600 < y)
		{
			return 1;
		}
		return 0;
	}

};

class Racket :public GameObject {

private:

	const int racket_width = 80;
	const int racket_height = 20;
	const int racket_fill = 1;	

public:

	void Draw(int &x, int &y) 
	{
		DrawObject(x, y, racket_width, racket_height, racket_fill);
	}

	void Move(int &x, int &y, int& mx, int& my)
	{
		if (mx < 0)
		{
			mx = 0;
		}
		else if (800 < mx)
		{
			mx = 800;
		}
		if (my < 500)
		{
			my = 500;
		}
		else if (600 < my)
		{
			my = 600;
		}
		/*if (mx - (racket_width / 2) <= x && x <= mx + (racket_width / 2) && my <= y - (racket_height / 2) && y <= my + (racket_height / 2) && 0 < vy)
		{
			vy = -vy;
		}*/
		//Draw(x, y);
	}

	void HitTest(int& x, int& y, int& mx, int& my, int& vy)
	{
		if (mx - (racket_width / 2) <= x && x <= mx + (racket_width / 2) && my <= y - (racket_height / 2) && y <= my + (racket_height / 2) && 0 < vy)
		{
			vy = -vy;
		}
	}
	
};

class Block :public GameObject {

private:

	const int block_width = 80;
	const int block_height = 20;
	const int block_y = 50;
	std::vector <int> block_cords{ 40,120,200,280,360,440,520,600,680,760 };
	const int num_blocks = 10;

public:


	void Draw(int block_fill[])
	{
		for (int i = 0; i < num_blocks; i++) 
		{
			DrawObject(block_cords[i], block_y, block_width, block_height, block_fill[i]);
		}
		
	}

	void HitTest(int& x, int& y, int block_fill[],int &vy) 
	{
		for (int i = 0; i < num_blocks; i++) 
		{
			if (block_fill[i] != 0) 
			{
				if (x<block_cords[i] + (block_width / 2) && x>block_cords[i] - (block_width / 2) && y < block_width / 2 && y>0) 
				{
					vy = -vy;
					block_fill[i] = 0;
				}

			}
			
		}
	}
};

int main(void)
{
	Racket racket;
	Block block;
	Ball ball;
	int block_state[10] = { 1,1,1,1,1,1,1,1,1,1 };

	FsOpenWindow(64, 16, 800, 600, 1);

	int bx, by, vx, vy, rx, ry;

	bx = 400;
	by = 300;
	vx = 5;
	vy = -5;

	rx = 300;
	ry = 550;

	
	for (;;)
	{
		FsPollDevice();

		if (FSKEY_ESC == FsInkey())
		{
			break;
		}

		
		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		ball.Draw(bx, by);
		racket.Draw(rx, ry);
		block.Draw(block_state);
		
		ball.Move(bx, by,vx,vy);
		//racket.Move(bx, by, rx, ry,vy);
		block.HitTest(bx, by, block_state,vy);
		//racket.HitTest(bx, by, rx, ry, vy);

		if (ball.Miss(by)==1)
		{
			printf("Miss!\n");
			break;
		}

		int sum = 0;
		for (int j = 0; j < 10; j++) 
		{
			//int sum = 0;
			sum += block_state[j];
		}
		if (sum == 0)
		{
			break;
		}

		int lb, mb, rb;
		FsGetMouseEvent(lb, mb, rb, rx, ry);	
		racket.Move(bx, by, rx, ry);
		racket.HitTest(bx, by, rx, ry, vy);
		FsSwapBuffers();
		FsSleep(25);
	}

	int score = 0;
	for (int k=0;k<10;k++)
	{
		if (block_state[k]==0)
		{
			score += 1;
		}
	}

	std::cout << "\nYour score is: " << score << std::endl;

	if (score == 10) 
	{
		std::cout << "\nCongratulations!! You successfully cleared all the blocks" << std::endl;
	}

}