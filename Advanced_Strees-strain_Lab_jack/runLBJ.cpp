#include <iostream>
#include "zaber/motion/ascii.h"
#include "class_cm.h"
#include <chrono>
#include <string>
#include <thread>
#include <stdio.h>
#include <windows.h>
#include <math.h>
#include <stdlib.h>
#include "LabJackUD.h"
#include "fssimplewindow.h"
#include "ysglfontdata.h"
#include <stddef.h>

#define KS_PI 3.1415926535

using namespace std;
using namespace zaber::motion;
using namespace zaber::motion::ascii;

//////////////////////////////////////////////////////////////////////////
//GUI
int myAbs(int x)
{
	if (x < 0)
	{
		x = -x;
	}
	return x;
}

void DrawCircle(int cx, int cy, int rad, int r, int g, int b)
{
	glBegin(GL_POLYGON);
	glColor3ub(r, g, b);
	for (int i = 0; i < 360; i++)
	{
		double angle = (double)i * KS_PI / 180.0;
		double x = (double)cx + cos(angle) * (double)rad;
		double y = (double)cy + sin(angle) * (double)rad;
		glVertex2d(x, y);
	}
	glEnd();
}

void DrawRectangle(int cx, int cy, int w, int h, int r, int g, int b)
{
	glBegin(GL_QUADS);
	glColor3ub(r, g, b);
	glVertex2d(cx - w, cy - h);
	glVertex2d(cx + w, cy - h);
	glVertex2d(cx + w, cy + h);
	glVertex2d(cx - w, cy + h);
	glEnd();
}

int getTextWidth(char name[])
{
	int len = 0;
	for (int i = 0; name[i] != 0; ++i)
	{
		++len;
	}
	return len;
}

int getTextWidthConst(const char name[])
{
	int len = 0;
	for (int i = 0; name[i] != 0; ++i)
	{
		++len;
	}
	return len;
}

class Button
{
protected:
	int cx, cy, w, h;
	int fontSize; // 0 is 8x8, 1 is 8x12, 2 is 12x16, 3 is 16x20
	int state; // 0 is off, 1 is on, 2 is button getting clicked;
	char shape;

public:
	char name[255] = "";
	int color[3] = { 0,0,0 };

	void Draw(void);
	int CheckClick(int lb, int rx, int ry);
	void ChangeState(int newState);

	Button(int x, int y, char shp, char nm[], int clr[], int fSize)
	{
		cx = x;
		cy = y;
		w = 0;
		h = 0;
		state = 1;
		shape = shp;
		fontSize = fSize;
		for (int i = 0; i < 255; ++i)
		{
			if (i < 3)
			{
				color[i] = clr[i];
			}
			name[i] = nm[i];
		}
	}
};

void Button::Draw(void)
{
	int r = color[0];
	int g = color[1];
	int b = color[2];
	// each character is 8 pixels wide because of YsGlDrawFontBitmap8x8() on line 111,119
	// 8/2 = 4 so the width must be multiplied by 1/2 x character width (because of how I draw boxes
	// doing x + width, x - width)
	int textW;

	// textMultiplier and textAdd are things that help the text be aligned in the center of the button
	int textMult;
	int textAdd;

	switch (shape)
	{
		// rectangle / square
	case 'R':

		// 'off' state -- user cannot click the button so it is grayed out
		if (state == 0)
		{
			switch (fontSize)
			{
			case 0: // 8x8 = w x h
				textMult = 4; // must be half of the width
				textAdd = 4; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 11; // height + 7 on top and bottom -> ( 8 + 7 + 7 ) / 2 = 11
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawRectangle(cx, cy, w, h, 170, 170, 170);
				glColor3ub(200, 200, 200);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap8x8(name);
				break;
			case 1: // 8x12 = w x h
				textMult = 4; // must be half of the width
				textAdd = 6; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 13; // height + 7 on top and bottom -> ( 12 + 7 + 7 ) / 2 = 13
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawRectangle(cx, cy, w, h, 170, 170, 170);
				glColor3ub(200, 200, 200);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap8x12(name);
				break;
			case 2: // 12x16 = w x h
				textMult = 6; // must be half of the width
				textAdd = 8; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 15; // height + 7 on top and bottom -> ( 16 + 7 + 7 ) / 2 = 15
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawRectangle(cx, cy, w, h, 170, 170, 170);
				glColor3ub(200, 200, 200);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap12x16(name);
				break;
			case 3: // 16x20 = w x h
				textMult = 8; // must be half of the width
				textAdd = 10; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 17; // height + 7 on top and bottom -> ( 20 + 7 + 7 ) / 2 = 17
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawRectangle(cx, cy, w, h, 170, 170, 170);
				glColor3ub(200, 200, 200);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap16x20(name);
				break;
			}

		}

		// regular 'on' state
		if (state == 1)
		{
			switch (fontSize)
			{
			case 0: // 8x8 = w x h
				textMult = 4; // must be half of the width
				textAdd = 4; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 11; // height + 7 on top and bottom -> ( 8 + 7 + 7 ) / 2 = 11
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawRectangle(cx, cy, w, h, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap8x8(name);
				break;
			case 1: // 8x12 = w x h
				textMult = 4; // must be half of the width
				textAdd = 6; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 13; // height + 7 on top and bottom -> ( 12 + 7 + 7 ) / 2 = 13
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawRectangle(cx, cy, w, h, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap8x12(name);
				break;
			case 2: // 12x16 = w x h
				textMult = 6; // must be half of the width
				textAdd = 8; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 15; // height + 7 on top and bottom -> ( 16 + 7 + 7 ) / 2 = 15
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawRectangle(cx, cy, w, h, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap12x16(name);
				break;
			case 3: // 16x20 = w x h
				textMult = 8; // must be half of the width
				textAdd = 10; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 17; // height + 7 on top and bottom -> ( 20 + 7 + 7 ) / 2 = 17
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawRectangle(cx, cy, w, h, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap16x20(name);
				break;
			}

		}

		// 'clicked' state -- button appears slightly smaller
		if (state == 2)
		{
			switch (fontSize)
			{
			case 0: // 8x8 = w x h
				textMult = 4; // must be half of the width
				textAdd = 4; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 9; // height + 5 on top and bottom -> ( 8 + 5 + 5 ) / 2 = 9
				w = textW + 5;
				DrawRectangle(cx, cy, w, h, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap8x8(name);
				state = 1;
				break;
			case 1: // 8x12 = w x h
				textMult = 4; // must be half of the width
				textAdd = 6; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 11; // height + 5 on top and bottom -> ( 12 + 5 + 5 ) / 2 = 11
				w = textW + 5;
				DrawRectangle(cx, cy, w, h, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap8x12(name);
				state = 1;
				break;
			case 2: // 12x16 = w x h
				textMult = 6; // must be half of the width
				textAdd = 8; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 13; // height + 5 on top and bottom -> ( 16 + 5 + 5 ) / 2 = 13
				w = textW + 5;
				DrawRectangle(cx, cy, w, h, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap12x16(name);
				state = 1;
				break;
			case 3: // 16x20 = w x h
				textMult = 8; // must be half of the width
				textAdd = 10; // must be half of the height
				textW = getTextWidth(name) * textMult;
				h = 15; // height + 5 on top and bottom -> ( 20 + 5 + 5 ) / 2 = 15
				w = textW + 5;
				DrawRectangle(cx, cy, w, h, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW, cy + textAdd);
				YsGlDrawFontBitmap16x20(name);
				state = 1;
				break;
			}
		}
		break;

		// circle
	case 'C':
		// 'off' state -- user cannot click the button so it is grayed out
		if (state == 0)
		{
			switch (fontSize)
			{
			case 0: // 8x8 = w x h
				textMult = 4; // must be half of the width
				textAdd = 4; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawCircle(cx, cy, w, 170, 170, 170);
				glColor3ub(200, 200, 200);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap8x8(name);
				break;
			case 1: // 8x12 = w x h
				textMult = 4; // must be half of the width
				textAdd = 6; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawCircle(cx, cy, w, 170, 170, 170);
				glColor3ub(200, 200, 200);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap8x12(name);
				break;
			case 2: // 12x16 = w x h
				textMult = 6; // must be half of the width
				textAdd = 8; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawCircle(cx, cy, w, 170, 170, 170);
				glColor3ub(200, 200, 200);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap12x16(name);
				break;
			case 3: // 16x20 = w x h
				textMult = 8; // must be half of the width
				textAdd = 10; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawCircle(cx, cy, w, 170, 170, 170);
				glColor3ub(200, 200, 200);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap16x20(name);
				break;
			}

		}

		// regular 'on' state
		if (state == 1)
		{
			switch (fontSize)
			{
			case 0: // 8x8 = w x h
				textMult = 4; // must be half of the width
				textAdd = 4; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawCircle(cx, cy, w, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap8x8(name);
				break;
			case 1: // 8x12 = w x h
				textMult = 4; // must be half of the width
				textAdd = 6; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawCircle(cx, cy, w, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap8x12(name);
				break;
			case 2: // 12x16 = w x h
				textMult = 6; // must be half of the width
				textAdd = 8; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawCircle(cx, cy, w, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap12x16(name);
				break;
			case 3: // 16x20 = w x h
				textMult = 8; // must be half of the width
				textAdd = 10; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 7; // adding 7 pixels to each side to make it look nicer
				DrawCircle(cx, cy, w, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap16x20(name);
				break;
			}
		}

		// 'clicked' state -- button appears slightly smaller
		if (state == 2)
		{
			switch (fontSize)
			{
			case 0: // 8x8 = w x h
				textMult = 4; // must be half of the width
				textAdd = 4; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 5;
				DrawCircle(cx, cy, w, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap8x8(name);
				state = 1;
				break;
			case 1: // 8x12 = w x h
				textMult = 4; // must be half of the width
				textAdd = 6; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 5;
				DrawCircle(cx, cy, w, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap8x12(name);
				state = 1;
				break;
			case 2: // 12x16 = w x h
				textMult = 6; // must be half of the width
				textAdd = 8; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 5;
				DrawCircle(cx, cy, w, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap12x16(name);
				state = 1;
				break;
			case 3: // 16x20 = w x h
				textMult = 8; // must be half of the width
				textAdd = 10; // must be half of the height
				textW = getTextWidth(name) * textMult;
				w = textW + 5;
				DrawCircle(cx, cy, w, r, g, b);
				glColor3ub(255, 255, 255);
				glRasterPos2d(cx - textW + 2, cy + textAdd);
				YsGlDrawFontBitmap16x20(name);
				state = 1;
				break;
			}
		}
		break;
	}
}

int Button::CheckClick(int lb, int mx, int my)
{
	if (state > 0)
	{
		switch (shape)
		{
			// rectangle / square
		case 'R':
			if (mx >= (cx - w) && mx <= (cx + w))
			{
				if (my >= (cy - h) && my <= (cy + h))
				{
					if (lb)
					{
						state = 2;
						return 1;
					}
				}
			}
			break;

			// circle
		case 'C':
			int dx = cx - mx; // myAbs(cx - mx);
			int dy = myAbs(cy - my);
			int dist = (int)sqrt(dx * dx + dy * dy);
			if (dist < w)
			{
				if (lb)
				{
					state = 2;
					return 1;
				}
			}
			break;
		}
	}
	return 0;
}

void Button::ChangeState(int newState)
{
	state = newState;
}

class TextBox
{
protected:
	int cx, cy, w, h;
	int state; // 0 = off, 1 = on

public:
	char textInput[255];
	char name[255];
	void Draw(void);
	void ChangeValue(double val);

	TextBox(int x, int y, int wi, int hi, char text[], char nm[])
	{
		cx = x;
		cy = y;
		w = wi;
		h = hi;
		state = 1;
		for (int i = 0; i < 255; ++i)
		{
			textInput[i] = text[i];
			name[i] = nm[i];
		}
	}

};

void TextBox::Draw(void)
{
	int lineWidth = 5;
	int nameLen = getTextWidthConst(name) * 20;
	int textLen = getTextWidthConst(textInput) * 20;

	glLineWidth(lineWidth);
	glBegin(GL_LINE_LOOP);
	glColor3ub(0, 0, 0);
	glVertex2d(cx - w, cy - h);
	glVertex2d(cx + w, cy - h);
	glVertex2d(cx + w, cy + h);
	glVertex2d(cx - w, cy + h);
	glEnd();

	glColor3ub(0, 0, 0);
	glRasterPos2i(cx - textLen / 2, cy + (h - 4));
	YsGlDrawFontBitmap16x20(textInput);

	glColor3ub(0, 0, 0);
	glRasterPos2i(cx - (w - 10) - nameLen, cy + (h - 4));
	YsGlDrawFontBitmap16x20(name);
}

void TextBox::ChangeValue(double val)
{
	char valStr[10];
	sprintf(valStr, "%llf", val);
	for (int i = 0; i < 10; ++i)
	{
		textInput[i] = valStr[i];
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class TextString
{
protected:
	char* str;

public:
	TextString();
	TextString(const char incoming[]);
	TextString(const TextString& incoming);
	TextString& operator=(const TextString& incoming);
	~TextString();
	void clear(void);

	const char* fgets(FILE* fp);

	void set(const char str[]);
	void push_back(char c);
	void pop_back(void);
	//char pop_front(void);
	const char* c_str(void) const;
	long long int size(void) const;
};
TextString::TextString()
{
	str = nullptr;
}
TextString::TextString(const char incoming[])
{
	str = nullptr;
	set(incoming);
}
TextString::TextString(const TextString& incoming)
{
	str = nullptr;
	set(incoming.c_str());
}
TextString& TextString::operator=(const TextString& incoming)
{
	set(incoming.c_str());
	return *this;
}
TextString::~TextString()
{
	clear();
}
void TextString::clear(void)
{
	if (nullptr != str)
	{
		delete[] str;
	}
	str = nullptr;
}
const char* TextString::fgets(FILE* fp)
{
	clear();

	char buf[16];
	while (nullptr != ::fgets(buf, 15, fp))
	{
		for (int i = 0; 0 != buf[i]; ++i)
		{
			if (buf[i] == '\n')
			{
				return c_str();
			}
			this->push_back(buf[i]);
		}
	}
	if (0 < size())
	{
		return str;
	}
	return nullptr;
}

void TextString::set(const char incoming[])
{
	if (incoming != str)
	{
		clear();
		if (nullptr != incoming)
		{
			int n = 0;
			for (n = 0; 0 != incoming[n]; ++n)
			{
			}

			auto* newStr = new char[n + 1];
			for (int i = 0; i < n; ++i)
			{
				newStr[i] = incoming[i];
			}
			newStr[n] = 0;

			str = newStr;
		}
	}
}

void TextString::push_back(char c)
{
	auto l = size();
	auto newL = l + 1;
	auto ptr = c_str();

	auto newStr = new char[newL + 1];
	for (int i = 0; i < l; ++i)
	{
		newStr[i] = ptr[i];
	}
	newStr[l] = c;
	newStr[newL] = 0;

	clear();

	str = newStr;
}

void TextString::pop_back(void)
{
	auto l = size();
	if (0 < l)
	{
		auto newL = l - 1;
		auto newStr = new char[newL + 1];

		for (int i = 0; i < newL; ++i)
		{
			newStr[i] = str[i];
		}
		newStr[newL] = 0;

		clear();

		str = newStr;
	}
}

//char TextString::pop_front(void)
//{
//    auto l = size();
//    if (0 < l)
//    {
//        auto newL = l - 1;
//        auto newStr = new char[newL + 1];
//
//        for (int i = 0; i < newL; ++i)
//        {
//            newStr[i] = str[i];
//        }
//        newStr[newL] = 0;
//
//        clear();
//
//        str = newStr;
//    }
//}

const char* TextString::c_str(void) const
{
	if (nullptr != str)
	{
		return str;
	}
	else
	{
		return "";
	}
}

long long int TextString::size(void) const
{
	auto ptr = c_str();
	long long int n = 0;
	for (n = 0; 0 != ptr[n]; ++n)
	{
	}
	return n;
}

class FilenameInput
{
public:
	TextString output;
	int Input(const char msg[], int cx, int cy);
	long long int size(void);
};

long long int FilenameInput::size(void)
{
	return output.size();
}

int FilenameInput::Input(const char msg[], int cx, int cy)
{
	output = "";

	for (;;)
	{
		FsPollDevice();
		auto key = FsInkey();
		if (FSKEY_ENTER == key)
		{
			return key;
		}
		if (FSKEY_ESC == key)
		{
			output = "";
			return key;
		}
		if (FSKEY_BS == key)
		{
			output.pop_back();
		}

		auto c = FsInkeyChar();
		if (' ' <= c)
		{
			output.push_back(c);
		}

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		int lineWidth = 5;
		int w = 300;
		int h = 20; // must be at least half of the height of the characters

		// drawing "Enter Filename"
		glColor3ub(0, 0, 255);
		glRasterPos2i(cx - 135, cy - 30);
		YsGlDrawFontBitmap20x32(msg);

		TextString s2 = output;
		if (0 != time(nullptr) % 2)
		{
			s2.push_back('|');
		}
		else
		{
			s2.push_back(' ');
		}

		int len = getTextWidthConst(s2.c_str()) * 20;
		int b = cx + w - len;
		int a = cy + (h - 4);
		/*printf("x = %d\n", b);
		printf("y = %d\n", a);
		printf("s = %s\n", s2.c_str());*/
		glRasterPos2i(cx + w - len, cy + (h - 4));
		YsGlDrawFontBitmap20x32(s2.c_str());

		// drawing bounding box
		glLineWidth(lineWidth);
		glBegin(GL_LINE_LOOP);
		glColor3ub(0, 0, 0);
		glVertex2d(cx - w, cy - h);
		glVertex2d(cx + w, cy - h);
		glVertex2d(cx + w, cy + h);
		glVertex2d(cx - w, cy + h);
		glEnd();

		glColor3ub(0, 0, 0);
		glRasterPos2i(cx + w + 7, cy + (h - 4));
		const char txt[5] = ".txt";
		YsGlDrawFontBitmap20x32(txt);

		glBegin(GL_QUADS);
		glColor3ub(255, 255, 255);
		glVertex2d(0, cy - h);
		glVertex2d(cx - w - 1, cy - h);
		glVertex2d(cx - w - 1, cy + h);
		glVertex2d(0, cy + h);
		glEnd();

		FsSwapBuffers();

		FsSleep(10);
	}
	return 0;
}
//////////////////////////////////////////////////////////////////////////




//The project must also know where to find labjackud.lib.  Here we do
//that by putting the lib file in the file view to the left.  The relative
//path stored by Visual Studio might not be the same on your machine, so
//you will probably have to delete the lib in the file view and right-click
//to re-add it from the install location ...\labjack\drivers\labjackud.lib.
//Another option, besides using the .lib, is dynamic linking.  Some
//compilers might not be able to link the .lib, and dynamic linking
//is usually the solution in those cases.



//This is our simple error handling function that is called after every UD
//function call.  This function displays the errorcode and string description
//of the error.  It also has a line number input that can be used with the
//macro __LINE__ to display the line number in source code that called the
//error handler.  It also has an iteration input is useful when processing
//results in a loop (getfirst/getnext).
void ErrorHandler(LJ_ERROR lngErrorcode, long lngLineNumber, long lngIteration)
{
	char err[255];

	if (lngErrorcode != LJE_NOERROR)
	{
		ErrorToString(lngErrorcode, err);
		printf("Error number = %d\n", lngErrorcode);
		printf("Error string = %s\n", err);
		printf("Source line number = %d\n", lngLineNumber);
		printf("Iteration = %d\n\n", lngIteration);
		if (lngErrorcode > LJE_MIN_GROUP_ERROR)
		{
			//Quit if this is a group error.
			getchar();
			exit(0);
		}
	}
}

void LBJStart(LJ_HANDLE &lngHandle)
{
	LJ_ERROR lngErrorcode;
	//LJ_HANDLE lngHandle = 0;
	long lngGetNextIteration;
	char ch;

	long lngIOType = 0, lngChannel = 0;
	double dblValue = 0;
	double ValueDIPort = 0;
	double ValueAIN[16] = { 9999 };

	long time = 0, i = 0, j = 0;
	long numIterations = 100;
	long numChannels = 6;  //Number of AIN channels, 0-16.
	long quickSample = 1;  //Set to TRUE for quick AIN sampling.
	long longSettling = 0;  //Set to TRUE for extra AIN settling time.

	//Open the first found LabJack.
	lngErrorcode = OpenLabJack(LJ_dtU3, LJ_ctUSB, "1", 1, &lngHandle);
	ErrorHandler(lngErrorcode, __LINE__, 0);

	//Start by using the pin_configuration_reset IOType so that all
	//pin assignments are in the factory default condition.
	lngErrorcode = ePut(lngHandle, LJ_ioPIN_CONFIGURATION_RESET, 0, 0, 0);
	ErrorHandler(lngErrorcode, __LINE__, 0);

	//Configure quickSample.
	lngErrorcode = ePut(lngHandle, LJ_ioPUT_CONFIG, LJ_chAIN_RESOLUTION, quickSample, 0);
	ErrorHandler(lngErrorcode, __LINE__, 0);

	//Configure longSettling.
	lngErrorcode = ePut(lngHandle, LJ_ioPUT_CONFIG, LJ_chAIN_SETTLING_TIME, longSettling, 0);
	ErrorHandler(lngErrorcode, __LINE__, 0);

	//Configure the necessary lines as analog.
	lngErrorcode = ePut(lngHandle, LJ_ioPUT_ANALOG_ENABLE_PORT, 0, pow(2, numChannels) - 1, numChannels);
	ErrorHandler(lngErrorcode, __LINE__, 0);

	//Now add requests that will be processed every iteration of the loop.

	//Add analog input requests.
	for (j = 0; j < numChannels; j++)
	{
		lngErrorcode = AddRequest(lngHandle, LJ_ioGET_AIN, j, 0, 0, 0);
		ErrorHandler(lngErrorcode, __LINE__, 0);
	}

}

double getForce(LJ_HANDLE &lngHandle)
{
	LJ_ERROR lngErrorcode;
	long lngGetNextIteration;
	long lngIOType = 0, lngChannel = 0;
	double dblValue = 0;
	double ValueDIPort = 0;
	double ValueAIN[16] = { 9999 };
	lngErrorcode = GoOne(lngHandle);
	ErrorHandler(lngErrorcode, __LINE__, 0);

	//Get all the results.  The input measurement results are stored.  All other
	//results are for configuration or output requests so we are just checking
	//whether there was an error.
	lngErrorcode = GetFirstResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
	ErrorHandler(lngErrorcode, __LINE__, 0);
	lngGetNextIteration = 0;	//Used by the error handling function.
	while (lngErrorcode < LJE_MIN_GROUP_ERROR)
	{
		switch (lngIOType)
		{

		case LJ_ioGET_AIN:
			ValueAIN[lngChannel] = dblValue;
			break;

		case LJ_ioGET_DIGITAL_PORT:
			ValueDIPort = dblValue;
			break;
		}

		lngErrorcode = GetNextResult(lngHandle, &lngIOType, &lngChannel, &dblValue, 0, 0);
		if (lngErrorcode != LJE_NO_MORE_DATA_AVAILABLE)
		{
			ErrorHandler(lngErrorcode, __LINE__, lngGetNextIteration);
		}

		lngGetNextIteration++;
	}
	double force = ValueAIN[5] * 2.233963 + 2.775769;//Newtons
	return force;
}
/////////////////////////////////////////////////////////////////////////////
//Data Structure
/////////////////////////////////////////////////////////////////////////////
class RawData
{
public:
	double x, force;
};

const int px = 400, py = 400;
const int wx = 800, wy = 600;
/////////////////////////////////////////////////////////////////////////////
//Plotter functions
/////////////////////////////////////////////////////////////////////////////
void drawBoundBox() {
	glBegin(GL_LINE_LOOP);
	glColor3f(0.0, 0.0, 0.0);
	glVertex2d(10   , wy - 10);
	glVertex2d(px+10, wy - 10);
	glVertex2d(px+10, wy - py - 10);
	glVertex2d(10,    wy - py - 10);
	glEnd();
}

void drawYAxesTics() {
	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 0.0);
	for (int i = 0; i < 10; i++) {
		double vertex1x = 10;
		double vertex1y = wy-10 - (py/10 * i);
		double vertex2x = 10 + 10;
		double vertex2y = wy - 10 - (py / 10 * i);
		glVertex2d(vertex1x, vertex1y);
		glVertex2d(vertex2x, vertex2y);
	}
	glEnd();
}

void drawXAxesTics() {
	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 0.0);
	for (int i = 0; i < 10; i++) {
		double vertex1x = 10 + (px / 10 * i);
		double vertex1y = wy-10;
		double vertex2x = 10 + (px / 10 * i);
		double vertex2y = wy - 10-10;
		glVertex2d(vertex1x, vertex1y);
		glVertex2d(vertex2x, vertex2y);
	}
	glEnd();
}

void drawGraph(std::vector <RawData> rawData) {

	//glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
	drawBoundBox();
	drawYAxesTics();
	drawXAxesTics();

	//set max displacement
	double max_displacement = 0.1;
	double max_force = 0.1;//newtons
	double td = 0.1;
	double tf = 0.1;

	for (int i = 0; i < rawData.size(); i++) {
		if (rawData[i].x > max_displacement)
		{
			max_displacement = rawData[i].x;
		}
		if (rawData[i].force > max_force)
		{
			max_force = rawData[i].force;
		}
	}
	while (td < max_displacement) td += .1;
	while (tf < max_force) tf += .1;
	max_displacement = td;
	max_force = tf;

	glBegin(GL_LINES);
	glColor3f(0.0, 0.0, 1.0);

	for (int i = 0; i < (rawData.size()-1); i++) {
		double data1x = 10 + rawData[i].x * (px / max_displacement);
		double data1y = (wy-10 - (rawData[i].force * py / max_force));
		double data2x = 10 + rawData[i + 1].x * (px / max_displacement);
		double data2y = (wy-10 - (rawData[i + 1].force * py / max_force));
		glVertex2d(data1x, data1y);
		glVertex2d(data2x, data2y);
	}
	glEnd();
	FsSwapBuffers();
	FsSleep(100);
}
/////////////////////////////////////////////////////////////////////////////////////////


int stopButtonPressed(void)
{
	FsPollDevice();
	int lb, mb, rb, mx, my;
	FsGetMouseEvent(lb, mb, rb, mx, my);
	//int eStopTrig = EStop.CheckClick(lb, mx, my);
	//return eStopTrig;
	return 0;
}


std::vector <RawData> compTest(LJ_HANDLE &lngHandle, Ascii &motor, const double initPos, const double initForce, 
								Button &EStop, TextBox &forceBox, TextBox &velocityBox)
{
	std::vector <RawData> rawData;
	double d = 0.0;
	double vel;
	double maxAllowableForce = 1;//Newtons (*100 to get to grams)
	//std::cout << "Enter Compression velocity (mm/min)>";
	//std::cin >> vel;
	vel = 10;
	vel = vel / 60;//convert to millimeters per second
	//start linear stage
	motor.move_vel(vel, "millimetre/s");

	//collect data until 1)stopbutton 2)motor not moveing 3)max force reached
	for (;;)
	{
		double newForce = (getForce(lngHandle) - initForce);
		double newPos = motor.get_position() - initPos;
		FsPollDevice();
		int lb, mb, rb, mx, my;
		FsGetMouseEvent(lb, mb, rb, mx, my);
		auto key = FsInkey();
		if (EStop.CheckClick(lb,mx,my) || //<---still need to get from kenny
			motor.is_idle() ||
			//newForce > maxAllowableForce ||
			FSKEY_ESC == key)
		{
			break;
		}
		RawData temp;
		temp.x = newPos;
		temp.force = newForce; 
		//temp.x = d - initPos;
		//temp.force = d - initForce;
		rawData.push_back(temp);

		forceBox.ChangeValue(newForce);
		velocityBox.ChangeValue(newPos);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
		forceBox.Draw();
		velocityBox.Draw();
		EStop.Draw();
		drawGraph(rawData);
		//forceBox.Draw();
		//velocityBox.Draw();

		//mainMenuButton.Draw();
		
		//startProcButton.Draw();
		//zeroMotorButton.Draw();
		//zeroForceButton.Draw();
		//saveTestDataButton.Draw();

	}

	//flip state for GUI
	motor.stop();
	return rawData;
}

int main(void)
{	
	//initialize all classes
	LJ_ERROR lngErrorcode;
	LJ_HANDLE lngHandle = 0;
	LBJStart(lngHandle);

	Ascii motor;
	motor.move_home();
	//motor.move_vel(0.1, "millimetre/s");

	//FsOpenWindow(0, 0, 800, 800, 1);
	//FsOpenWindow(0, 0, wx, wy, 1);

	std::vector<RawData> rawData;
	double initial_pos = motor.get_position();
	double initial_force = getForce(lngHandle);
	/////////////////////////////////////////////////////
	int windowWidth = wx;
	int windowHeight = wy;
	int terminateProgram = 0;
	int blackRGB[3] = { 0,0,0 };
	int redRGB[3] = { 255,0,0 };
	int greenRGB[3] = { 0,255,0 };
	int blueRGB[3] = { 0,0,255 };
	int whiteRGB[3] = { 255,255,255 };

	// 0 = menu, 1 = constant velocity procedure, 2 = constant force procedure
	// 3 = filename input 
	int progState = 0;
	int prevProgState = 0;

	char mbCVName[255] = "Constant Velocity Procedure";
	char mbCFName[255] = "Constant Force Procedure";
	char mmBName[255] = "Main Menu";
	char stopName[255] = "STOP";
	char startProcedureName[255] = "Start Procedure";
	char zeroForceName[255] = "Zero Force";
	char zeroMotorName[255] = "Zero Motor";
	char saveTestName[255] = "Save Test Data";
	char forceBoxName[255] = "Force";
	char velBoxName[255] = "Velocity";
	char PLACEHOLDERFORCEBOX[255] = "8";

	// Menu Button Constant Velocity/Force
	Button CVButton(400, 200, 'R', mbCVName, blackRGB, 3);
	Button CFButton(400, 400, 'R', mbCFName, blackRGB, 3);
	Button mainMenuButton(100, 40, 'R', mmBName, blackRGB, 1);
	Button EStop(700, 500, 'C', stopName, redRGB, 3);
	Button startProcButton(700, 100, 'R', startProcedureName, blackRGB, 1);
	Button zeroForceButton(700, 200, 'R', zeroForceName, blackRGB, 1);
	Button zeroMotorButton(700, 300, 'R', zeroMotorName, blackRGB, 1);
	Button saveTestDataButton(700, 400, 'R', saveTestName, blackRGB, 1);

	TextBox forceBox(350, 50, 30, 15, PLACEHOLDERFORCEBOX, forceBoxName);
	TextBox velocityBox(650, 50, 30, 15, PLACEHOLDERFORCEBOX, velBoxName);


	FsOpenWindow(150, 16, windowWidth, windowHeight, 1);

	// menu loop
	while (0 == terminateProgram)
	{
		FsPollDevice();
		int key = FsInkey();
		if (key == FSKEY_ESC)
		{
			terminateProgram = 1;
		}

		int lb, mb, rb, mx, my;
		FsGetMouseEvent(lb, mb, rb, mx, my);

		glClear(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);

		if (0 == progState)
		{
			// Menu

			int constVTrig = CVButton.CheckClick(lb, mx, my);
			int constFTrig = CFButton.CheckClick(lb, mx, my);

			CVButton.Draw();
			CFButton.Draw();

			if (constVTrig)
			{
				progState = 1;
			}

			if (constFTrig)
			{
				progState = 2;
			}
		}

		if (1 == progState)
		{
			// Constant Velocity Procedure
			int mainMenuTrig = mainMenuButton.CheckClick(lb, mx, my);
			int eStopTrig = EStop.CheckClick(lb, mx, my);
			int startProcTrig = startProcButton.CheckClick(lb, mx, my);
			int zeroMotorTrig = zeroMotorButton.CheckClick(lb, mx, my);
			int zeroForceTrig = zeroForceButton.CheckClick(lb, mx, my);
			int saveTestTrig = saveTestDataButton.CheckClick(lb, mx, my);

			forceBox.Draw();
			velocityBox.Draw();

			mainMenuButton.Draw();
			EStop.Draw();
			startProcButton.Draw();
			zeroMotorButton.Draw();
			zeroForceButton.Draw();
			saveTestDataButton.Draw();

			if (startProcTrig)
			{
				mainMenuButton.ChangeState(0);
				startProcButton.ChangeState(0);
				zeroMotorButton.ChangeState(0);
				zeroForceButton.ChangeState(0);
				saveTestDataButton.ChangeState(0);
				/////////////////////////////////////////////////////////////////////
				rawData = compTest(lngHandle, motor, initial_pos, initial_force, EStop, forceBox, velocityBox);
			}

			if (mainMenuTrig)
			{
				progState = 0;
			}

			if (saveTestTrig)
			{
				prevProgState = 1;
				progState = 3;
			}

		}

		if (2 == progState)
		{
			// Constant Force Procedure
			int mainMenuTrig = mainMenuButton.CheckClick(lb, mx, my);
			int eStopTrig = EStop.CheckClick(lb, mx, my);
			int startProcTrig = startProcButton.CheckClick(lb, mx, my);
			int zeroMotorTrig = zeroMotorButton.CheckClick(lb, mx, my);
			int zeroForceTrig = zeroForceButton.CheckClick(lb, mx, my);
			int saveTestTrig = saveTestDataButton.CheckClick(lb, mx, my);

			mainMenuButton.Draw();
			EStop.Draw();
			startProcButton.Draw();
			zeroMotorButton.Draw();
			zeroForceButton.Draw();
			saveTestDataButton.Draw();

			if (mainMenuTrig)
			{
				progState = 0;
			}

			if (saveTestTrig)
			{
				prevProgState = 2;
				progState = 3;
			}
		}

		if (3 == progState)
		{
			// Filename input Screen
			FilenameInput input;
			int fNameBox = input.Input("Enter Filename", 400, 300);

			if (FSKEY_ENTER == fNameBox)
			{
				progState = prevProgState;
			}
			else
			{
				printf("User cancelled.\n");
			}
		}

		FsSwapBuffers();
		FsSleep(100);
	}




	////////////////////////////////////////////////////

	//rawData = compTest(lngHandle,motor,initial_pos, initial_force);


	for (int i = 0; i < rawData.size(); ++i)
	{
		std::cout << rawData[i].x << "\t" << rawData[i].force << std::endl;
	}

	//while(1)
	//{
	//	double fourceout = getForce(lngHandle);
	//	double posout = motor.get_position();
	//	std::cout << fourceout <<"\t"<<posout<< std::endl;
	//	FsSleep(100);
	//}



	//close all connections

	motor.stop();
	motor.close();


	return 0;

}
