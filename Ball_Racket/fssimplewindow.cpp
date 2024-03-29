/* ////////////////////////////////////////////////////////////

File Name: fswin32wrapper.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */

#pragma warning(disable:4996)

// Memo about IME
//   I had to add WS_CLIPCHILDREN in the window styles and PeekMessage for NULL window handle
//   so that it looks for messages to the all window associated with this process.

// Winver needs to be 0x0601 or greater for WM_TOUCH
// Windows XP => 0x0501.  I hope this change won't disable binary compatibility with XP...

#if defined(_WIN32_WINNT) && _WIN32_WINNT<0x0601
#undef _WIN32_WINNT
#endif

#if !defined(_WIN32_WINNT)
#define _WIN32_WINNT 0x601
#endif

#ifdef PSAPI_VERSION
#undef PSAPI_VERSION
#endif
#define PSAPI_VERSION 1

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>
#include <string.h>
#include <windows.h>
#include <direct.h>
#include <gl/gl.h>
#include <gl/glu.h>

#include "fssimplewindow.h"
/* ////////////////////////////////////////////////////////////

File Name: fswin32keymap.h
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */



/* { */


const unsigned int FS_NUM_VK = 256;


class FsWin32KeyMapper
{
public:
	FsWin32KeyMapper();
	~FsWin32KeyMapper();

	int VkToFsKey(int vk);
	int FsKeyToVk(int fsKey);

protected:
	void AddKeyMapping(int fskey, int vk);

	int* mapVKtoFSKEY;
	int* mapFSKEYtoVK;
};


/* } */


#pragma comment(lib,"kernel32.lib")
#pragma comment(lib,"user32.lib")
#pragma comment(lib,"imm32.lib")
#pragma comment(lib,"gdi32.lib")
#pragma comment(lib,"opengl32.lib")
#pragma comment(lib,"glu32.lib")

static FsWin32KeyMapper fsKeyMapper;


#define IDC_NATIVE_TEXT_EDIT  101


/* ////////////////////////////////////////////////////////////

File Name: fssimplewindowinternal.h
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */



/* { */

class FsSimpleWindowInternal
{
public:
	HWND hWnd;
	HGLRC hRC;
	HPALETTE hPlt;
	HDC hDC;

	FsSimpleWindowInternal();
};

/* } */


static FsSimpleWindowInternal fsWin32Internal;

FsSimpleWindowInternal::FsSimpleWindowInternal()
{
	hWnd = NULL;
	hRC = NULL;
	hPlt = NULL;
	hDC = NULL;
}

const FsSimpleWindowInternal* FsGetSimpleWindowInternal(void)
{
	return &fsWin32Internal;
}


static LRESULT WINAPI WindowFunc(HWND wnd, UINT msg, WPARAM wp, LPARAM lp);
static void YsSetPixelFormat(HDC dc);
static HPALETTE YsCreatePalette(HDC dc);
static void InitializeOpenGL(HWND wnd);


////////////////////////////////////////////////////////////
static bool FsWin32IsWindowActive(void)
{
	HWND fgw = GetForegroundWindow();
	if (fgw != NULL && fsWin32Internal.hWnd == fgw)
	{
		return true;
	}
	return false;
}
////////////////////////////////////////////////////////////



// For OpenGL set up >>
static int doubleBuffer = 0;
// For OpenGL set up <<


class FsMouseEventLog
{
public:
	int eventType;
	int lb, mb, rb;
	int mx, my;
	unsigned int shift, ctrl;
};


#define NKEYBUF 256
static int keyBuffer[NKEYBUF];
static int nKeyBufUsed = 0;
static int charBuffer[NKEYBUF];
static int nCharBufUsed = 0;
static int nMosBufUsed = 0;
static FsMouseEventLog mosBuffer[NKEYBUF];

static const int maxNumTouch = 64;
static int nCurrentTouch = 0;
static FsVec2i currentTouch[maxNumTouch];

static int exposure = 0;


#define WINSTYLE WS_OVERLAPPED|WS_CAPTION|WS_VISIBLE|WS_SYSMENU|WS_THICKFRAME|WS_MINIMIZEBOX|WS_MAXIMIZEBOX|WS_CLIPCHILDREN

static const char* WINCLASS = "FsSimpleWindow_MainWindowClass";
static const char* WINNAME = "Main Window";


void FsOpenWindow(const FsOpenWindowOption& opt)
{
	int x0 = opt.x0;
	int y0 = opt.y0;
	int wid = opt.wid;
	int hei = opt.hei;
	int useDoubleBuffer = (int)opt.useDoubleBuffer;
	// int useMultiSampleBuffer==(int)opt.useMultiSampleBuffer;
	const char* windowName = opt.windowTitle;

	if (NULL != fsWin32Internal.hWnd)
	{
		MessageBoxA(fsWin32Internal.hWnd, "Error! Window already exists.", "Error!", MB_OK);
		exit(1);
	}

	// Note 2012/03/08 RegisterClassW and CreateWindowW doesn't seem to work.
	WNDCLASSA wc;
	HINSTANCE inst = GetModuleHandleA(NULL);

	wc.style = CS_OWNDC | CS_BYTEALIGNWINDOW;
	wc.lpfnWndProc = (WNDPROC)WindowFunc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = (HINSTANCE)inst;
	wc.hIcon = LoadIconA(inst, "MAINICON");
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = WINCLASS;
	if (0 != RegisterClassA(&wc))
	{
		doubleBuffer = useDoubleBuffer;


		RECT rc;
		rc.left = x0;
		rc.top = y0;
		rc.right = (unsigned long)(x0 + wid - 1);
		rc.bottom = (unsigned long)(y0 + hei - 1);
		AdjustWindowRect(&rc, WINSTYLE, FALSE);
		wid = rc.right - rc.left + 1;
		hei = rc.bottom - rc.top + 1;

#ifdef _UNICODE
		// What's the point of using CreateWindowA?  Another weird Microsoft logic here.
		static wchar_t buf[256];
		const char* windowNameA = (const char*)(NULL != windowName ? windowName : WINNAME);
		for (int i = 0; i < 255 && 0 != windowNameA[i]; ++i)
		{
			buf[i] = windowNameA[i];
			buf[i + 1] = 0;
		}
		const char* windowNameUsed = (const char*)buf;
#else
		const char* windowNameUsed = (const char*)(NULL != windowName ? windowName : WINNAME);
#endif

		fsWin32Internal.hWnd = CreateWindowA(WINCLASS, windowNameUsed, WINSTYLE, x0, y0, wid, hei, NULL, NULL, inst, NULL);
		if (NULL != fsWin32Internal.hWnd)
		{
			// https://msdn.microsoft.com/en-us/library/windows/desktop/dd371581%28v=vs.85%29.aspx
			// http://stackoverflow.com/questions/7560334/wm-touch-is-not-immediately-sent-with-touch-down-event
			//   Unless TWF_WANTPALM is specified, the first touch event won't be sent until the user releases or moves the touch.
#ifndef FSSIMPLEWINDOW_FOR_WINDOWS_XP
			RegisterTouchWindow(fsWin32Internal.hWnd, TWF_WANTPALM);
#endif

			if (opt.sizeOpt == opt.FULLSCREEN ||
				opt.sizeOpt == opt.MAXIMIZE_WINDOW)
			{
				if (opt.sizeOpt == opt.FULLSCREEN)
				{
					auto style = GetWindowLong(fsWin32Internal.hWnd, GWL_STYLE);
					style = (style & ~(WS_THICKFRAME | WS_BORDER | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_MAXIMIZEBOX));
					SetWindowLong(fsWin32Internal.hWnd, GWL_STYLE, style);
				}
				ShowWindow(fsWin32Internal.hWnd, SW_MAXIMIZE);
			}
			else
			{
				ShowWindow(fsWin32Internal.hWnd, SW_SHOWNORMAL);
			}
			UpdateWindow(fsWin32Internal.hWnd);

			FsPassedTime();  // Reset Timer
		}
		else
		{
			MessageBoxA(NULL, "Error! Could not open a window.", "Error!", MB_OK);
			exit(1);
		}
	}

	if (NULL != fsOpenGLInitializationCallBack)
	{
		(*fsOpenGLInitializationCallBack)(fsOpenGLInitializationCallBackParam);
	}

	if (NULL != fsAfterWindowCreationCallBack)
	{
		(*fsAfterWindowCreationCallBack)(fsAfterWindowCreationCallBackParam);
	}
}

int FsCheckWindowOpen(void)
{
	if (nullptr != fsWin32Internal.hWnd)
	{
		return 1;
	}
	return 0;
}

void FsCloseWindow(void)
{
	ImmAssociateContext(fsWin32Internal.hWnd, NULL);

	DestroyWindow(fsWin32Internal.hWnd);
	fsWin32Internal.hWnd = NULL;
	fsWin32Internal.hRC = NULL;
	fsWin32Internal.hPlt = NULL;
	fsWin32Internal.hDC = NULL;
}

void FsMaximizeWindow(void)
{
	ShowWindow(fsWin32Internal.hWnd, SW_MAXIMIZE);
}
void FsUnmaximizeWindow(void)
{
}
void FsMakeFullScreen(void)
{
}

void FsGetWindowSize(int& wid, int& hei)
{
	RECT rect;
	GetClientRect(fsWin32Internal.hWnd, &rect);
	wid = rect.right;
	hei = rect.bottom;
}

void FsGetWindowPosition(int& x0, int& y0)
{
	POINT pnt = { 0,0 };
	ClientToScreen(fsWin32Internal.hWnd, &pnt);

	RECT rect;
	GetClientRect(fsWin32Internal.hWnd, &rect);
	rect.left += pnt.x;
	rect.top += pnt.y;

	AdjustWindowRect(&rect, WINSTYLE, FALSE);

	x0 = rect.left;
	y0 = rect.top;
}

void FsSetWindowTitle(const char windowTitle[])
{
	SetWindowTextA(fsWin32Internal.hWnd, windowTitle);
}

void FsPushOnPaintEvent(void)
{
	InvalidateRect(fsWin32Internal.hWnd, NULL, FALSE);
}

void FsPollDevice(void)
{
	static bool busy = false;  // <- This will prevent interval call back called recursively from WM_PAINT
	if (true != busy)
	{
		if (NULL != fsPollDeviceHook)
		{
			(*fsPollDeviceHook)(fsPollDeviceHookParam);
		}

		busy = true;
		MSG msg;

		int wmAppCount = 0;   // Weak way to preventing infinite loop in screen savers
		while (wmAppCount < 10 && PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_APP || msg.message == WM_TIMER)
			{
				wmAppCount++;
			}
			// if(IsDialogMessage(fsWin32Internal.hWnd,&msg))
			// {
			// 	continue;
			// }
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		busy = false;
	}
}

void FsSleep(int ms)
{
	if (ms > 0)
	{
		Sleep(ms);
	}
}

long long int FsPassedTime(void)
{
	static long long int lastTick;
	long long int tick;

	static int first = 1;
	if (1 == first)
	{
		lastTick = FsSubSecondTimer();
		first = 0;
	}

	tick = FsSubSecondTimer();
	long long passed = tick - lastTick;
	lastTick = tick;

	return passed;
}

long long int FsSubSecondTimer(void)
{
	// If I give up on Windows XP, I can use GetTickCount64.

	long long int clk = GetTickCount();

	static long long int lastValue = 0;
	static long long int base = 0;

	if (clk < lastValue) // Underflow.  49.7 days have passed.
	{
		base += 0x100000000LL;
	}
	lastValue = clk;

	static int first = 1;
	static long long int t0 = 0;
	if (1 == first)
	{
		t0 = base + clk;
		first = 0;
	}

	return base + clk - t0;
}

int FsInkey(void)
{
	if (nKeyBufUsed > 0)
	{
		int i, keyCode;
		keyCode = keyBuffer[0];
		nKeyBufUsed--;
		for (i = 0; i < nKeyBufUsed; i++)
		{
			keyBuffer[i] = keyBuffer[i + 1];
		}
		return keyCode;
	}
	return 0;
}

int FsInkeyChar(void)
{
	if (nCharBufUsed > 0)
	{
		int i, asciiCode;
		asciiCode = charBuffer[0];
		nCharBufUsed--;
		for (i = 0; i < nCharBufUsed; i++)
		{
			charBuffer[i] = charBuffer[i + 1];
		}
		return asciiCode;
	}
	return 0;
}

void FsPushKey(int fskey)
{
	if (nKeyBufUsed < NKEYBUF)
	{
		keyBuffer[nKeyBufUsed++] = fskey;
	}
}

void FsPushChar(int c)
{
	if (nCharBufUsed < NKEYBUF)
	{
		charBuffer[nCharBufUsed++] = c;
	}
}

int FsGetKeyState(int fsKeyCode)
{
	if (true == FsWin32IsWindowActive())
	{
		const int vk = fsKeyMapper.FsKeyToVk(fsKeyCode);
		return (GetKeyState(vk) & 0x8000) != 0;
	}
	return 0;
}

int FsCheckWindowExposure(void)
{
	const int ret = exposure;
	exposure = 0;
	return ret;
}

void FsGetMouseState(int& lb, int& mb, int& rb, int& mx, int& my)
{
	POINT cur;
	GetCursorPos(&cur);
	ScreenToClient(fsWin32Internal.hWnd, &cur);

	mx = cur.x;
	my = cur.y;

	if (true == FsWin32IsWindowActive())
	{
		lb = ((GetKeyState(VK_LBUTTON) & 0x8000) != 0 ? 1 : 0);
		mb = ((GetKeyState(VK_MBUTTON) & 0x8000) != 0 ? 1 : 0);
		rb = ((GetKeyState(VK_RBUTTON) & 0x8000) != 0 ? 1 : 0);
	}
	else
	{
		lb = 0;
		mb = 0;
		rb = 0;
	}
}

int FsGetMouseEvent(int& lb, int& mb, int& rb, int& mx, int& my)
{
	if (0 < nMosBufUsed)
	{
		int eventType = mosBuffer[0].eventType;
		mx = mosBuffer[0].mx;
		my = mosBuffer[0].my;
		lb = mosBuffer[0].lb;
		mb = mosBuffer[0].mb;
		rb = mosBuffer[0].rb;

		int i;
		for (i = 0; i < nMosBufUsed - 1; i++)
		{
			mosBuffer[i] = mosBuffer[i + 1];
		}
		nMosBufUsed--;

		return eventType;
	}
	else
	{
		FsGetMouseState(lb, mb, rb, mx, my);
		return FSMOUSEEVENT_NONE;
	}
}

void FsSwapBuffers(void)
{
	if (NULL != fsSwapBuffersHook && true == (*fsSwapBuffersHook)(fsSwapBuffersHookParam))
	{
		return;
	}

	if (0 == doubleBuffer)
	{
		fprintf(stderr, "Error! FsSwapBuffers used in the single-buffered mode.\n");
		MessageBoxA(fsWin32Internal.hWnd, "Error! FsSwapBuffers used in the single-buffered mode.", "Error!", MB_OK);
		exit(1);
	}

	HDC hDC;
	glFlush();
	hDC = wglGetCurrentDC();
	SwapBuffers(hDC);
}

// 2015/10/31 LONG->LRESULT to remove warning.
static LRESULT WINAPI WindowFunc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	switch (msg)
	{
	case WM_TOUCH:
		// https://msdn.microsoft.com/en-us/library/windows/desktop/dd371581%28v=vs.85%29.aspx
		// https://msdn.microsoft.com/en-us/gg464991
#ifndef FSSIMPLEWINDOW_FOR_WINDOWS_XP
	{
		auto touchHd = (HTOUCHINPUT)lp;
		int nInput = LOWORD(wp);
		auto touchInput = new TOUCHINPUT[nInput];

		if (GetTouchInputInfo(touchHd, nInput, touchInput, sizeof(TOUCHINPUT)))
		{
			nCurrentTouch = 0;
			for (int i = 0; i < nInput && nCurrentTouch < maxNumTouch; ++i)
			{
				auto& t = touchInput[i];
				POINT p;
				p.x = t.x / 100;
				p.y = t.y / 100;
				ScreenToClient(fsWin32Internal.hWnd, &p);

				if ((t.dwFlags & TOUCHEVENTF_DOWN) || (t.dwFlags & TOUCHEVENTF_MOVE))
				{
					currentTouch[nCurrentTouch].v[0] = p.x;
					currentTouch[nCurrentTouch].v[1] = p.y;
					++nCurrentTouch;
				}
			}
		}

		delete[] touchInput;
		CloseTouchInputHandle(touchHd);
	}
#endif
	return 0;
	// return DefWindowProc(hWnd,msg,wp,lp);
	case WM_QUERYNEWPALETTE:
	case WM_PALETTECHANGED:
		if (NULL != fsWin32Internal.hPlt)
		{
			SelectPalette(fsWin32Internal.hDC, fsWin32Internal.hPlt, FALSE);
			RealizePalette(fsWin32Internal.hDC);
		}
		return DefWindowProc(hWnd, msg, wp, lp);
	case WM_CREATE:
		if (NULL != fsOpenGLContextCreationCallBack &&
			true == (*fsOpenGLContextCreationCallBack)(fsOpenGLContextCreationCallBackParam))
		{
			// If fsOpenGLContextCreationCallBack returns true, the OpenGL context or whatever context
			// is created in the call-back function.
			break;
		}
		fsWin32Internal.hDC = GetDC(hWnd);
		YsSetPixelFormat(fsWin32Internal.hDC);
		fsWin32Internal.hRC = wglCreateContext(fsWin32Internal.hDC);
		wglMakeCurrent(fsWin32Internal.hDC, fsWin32Internal.hRC);
		if (0 == doubleBuffer)
		{
			glDrawBuffer(GL_FRONT);
		}
		InitializeOpenGL(hWnd);
		break;
	case WM_CLOSE:
		if (NULL != fsCloseWindowCallBack &&
			false == (*fsCloseWindowCallBack)(fsCloseWindowCallBackParam))
		{
			return 0;
		}
		else
		{
			return DefWindowProc(hWnd, msg, wp, lp);
		}
		break;
	case WM_SIZE:
		if (NULL != fsOnResizeCallBack)
		{
			int wid = LOWORD(lp);
			int hei = HIWORD(lp);
			(*fsOnResizeCallBack)(fsOnResizeCallBackParam, wid, hei);
		}
		wglMakeCurrent(fsWin32Internal.hDC, fsWin32Internal.hRC);
		break;
	case WM_PAINT:
		wglMakeCurrent(fsWin32Internal.hDC, fsWin32Internal.hRC);
		exposure = 1;
		if (NULL != fsOnPaintCallback)
		{
			(*fsOnPaintCallback)(fsOnPaintCallbackParam);
		}
		if (NULL != fsIntervalCallBack)
		{
			(*fsIntervalCallBack)(fsIntervalCallBackParameter);
		}
		return DefWindowProc(hWnd, msg, wp, lp);
	case WM_COMMAND:
		break;
	case WM_DESTROY:
		fsWin32Internal.hWnd = nullptr;
		exit(0);
		break;
	case WM_MOUSEWHEEL:
	{
		int step;
		step = HIWORD(wp);
		if (step >= 0x8000)
		{
			step -= 0x10000;
		}
		if (step > 0)
		{
			while (step > 0)
			{
				if (nKeyBufUsed < NKEYBUF)
				{
					keyBuffer[nKeyBufUsed++] = FSKEY_WHEELUP;
				}
				step -= WHEEL_DELTA;
			}
		}
		else if (step < 0)
		{
			while (step < 0)
			{
				if (nKeyBufUsed < NKEYBUF)
				{
					keyBuffer[nKeyBufUsed++] = FSKEY_WHEELDOWN;
				}
				step += WHEEL_DELTA;
			}
		}
	}
	break;
	case WM_SYSCOMMAND:
		if ((wp & (~0x0f)) == SC_KEYMENU && (('0' <= lp && lp < '9') || ('A' <= lp && lp <= 'Z') || ('a' <= lp && lp <= 'z')))
		{
			// Prevent beep.
			return 0;
		}
		return DefWindowProc(hWnd, msg, wp, lp);
	case WM_SYSKEYDOWN:
	{
		// 2015/03/20 The condition need to be carefully considered.
		//            Simply pressing F10 looks to make WM_SYSKEYDOWN, instead of WM_KEYDOWN.
		bool takeKey = false;
		if ((lp & (1 << 29)) != 0 && // Alt
			(wp == VK_MENU ||
				wp == VK_OEM_1 ||
				wp == VK_OEM_PLUS ||
				wp == VK_OEM_COMMA ||
				wp == VK_OEM_MINUS ||
				wp == VK_OEM_PERIOD ||
				wp == VK_OEM_2 ||
				wp == VK_OEM_3 ||
				wp == VK_OEM_4 ||
				wp == VK_OEM_5 ||
				wp == VK_OEM_6 ||
				wp == VK_OEM_7 ||
				wp == VK_OEM_8 ||
#ifdef VK_OEM_AX
				wp == VK_OEM_AX ||
#endif
				wp == VK_OEM_102 ||
				wp == '0' ||
				wp == '1' ||
				wp == '2' ||
				wp == '3' ||
				wp == '4' ||
				wp == '5' ||
				wp == '6' ||
				wp == '7' ||
				wp == '8' ||
				wp == '9' ||
				wp == 'A' ||
				wp == 'B' ||
				wp == 'C' ||
				wp == 'D' ||
				wp == 'E' ||
				wp == 'F' ||
				wp == 'G' ||
				wp == 'H' ||
				wp == 'I' ||
				wp == 'J' ||
				wp == 'K' ||
				wp == 'L' ||
				wp == 'M' ||
				wp == 'N' ||
				wp == 'O' ||
				wp == 'P' ||
				wp == 'Q' ||
				wp == 'R' ||
				wp == 'S' ||
				wp == 'T' ||
				wp == 'U' ||
				wp == 'V' ||
				wp == 'W' ||
				wp == 'X' ||
				wp == 'Y' ||
				wp == 'Z' ||
				wp == VK_ESCAPE ||
				wp == VK_F1 ||
				wp == VK_F2 ||
				wp == VK_F3 ||
				/* wp==VK_F4 || */
				wp == VK_F5 ||
				wp == VK_F6 ||
				wp == VK_F7 ||
				wp == VK_F8 ||
				wp == VK_F9 ||
				wp == VK_F10 ||
				wp == VK_F11 ||
				wp == VK_F12 ||
				wp == VK_RETURN ||
				wp == VK_NUMLOCK ||
				wp == VK_NUMPAD0 ||
				wp == VK_NUMPAD1 ||
				wp == VK_NUMPAD2 ||
				wp == VK_NUMPAD3 ||
				wp == VK_NUMPAD4 ||
				wp == VK_NUMPAD5 ||
				wp == VK_NUMPAD6 ||
				wp == VK_NUMPAD7 ||
				wp == VK_NUMPAD8 ||
				wp == VK_NUMPAD9 ||
				wp == VK_DECIMAL ||
				wp == VK_DIVIDE ||
				wp == VK_MULTIPLY ||
				wp == VK_SUBTRACT ||
				wp == VK_ADD))
		{
			takeKey = true;
		}
		if (wp == VK_F10)
		{
			takeKey = true;
		}
		if (true == takeKey)
		{
			int keyCode;
			keyCode = fsKeyMapper.VkToFsKey((int)wp);
			if (keyCode != 0 && nKeyBufUsed < NKEYBUF)
			{
				keyBuffer[nKeyBufUsed++] = keyCode;
			}
			return 0;
		}
	}
	return DefWindowProc(hWnd, msg, wp, lp);
	case WM_SYSKEYUP:
		return 0;
	case WM_KEYDOWN:
		if (nKeyBufUsed < NKEYBUF)
		{
			const int keyCode = fsKeyMapper.VkToFsKey((int)wp);
			if (keyCode != 0)
			{
				FsPushKey(keyCode);
			}
		}
		break;
	case WM_CHAR:
		if (nCharBufUsed < NKEYBUF)
		{
			FsPushChar((int)wp);
		}
		break;
	case WM_IME_COMPOSITION:
	{
		HIMC hImc = ImmGetContext(hWnd);
		if (NULL != hImc)
		{
			// WM_IME_COMPOSITION message is shot multiple times during the composition.
			// But, GCS_RESULTSTR will return non-zero string only when the user chooses the candidate.
			auto lenInByte = ImmGetCompositionStringW(hImc, GCS_RESULTSTR, nullptr, 0);
			auto len = lenInByte / sizeof(wchar_t);
			auto buf = new wchar_t[len + 1];
			ImmGetCompositionStringW(hImc, GCS_RESULTSTR, buf, sizeof(wchar_t) * len);
			for (int i = 0; i < len; ++i)
			{
				FsPushChar(buf[i]);
			}
			ImmReleaseContext(hWnd, hImc);
		}
	}
	return DefWindowProc(hWnd, msg, wp, lp);
	case WM_IME_CHAR:
		// The following approach works, but dirty.
		// I want Unicode directly.
//		if(nCharBufUsed<NKEYBUF)
//		{
//			char multiByte[8]=
//			{
//				(char)((wp>>8)&255),
//				(char)(wp&255),
//				0
//			};
//			wchar_t wcout[8];
//			MultiByteToWideChar(CP_ACP,0,multiByte,-1,wcout,8);
//			FsPushChar((int)wcout[0]);
//		}
		break;
	case WM_ERASEBKGND:
		return 1;

	case WM_LBUTTONDOWN:
	case WM_LBUTTONUP:
	case WM_MBUTTONDOWN:
	case WM_MBUTTONUP:
	case WM_RBUTTONDOWN:
	case WM_RBUTTONUP:
	case WM_MOUSEMOVE:
		if (nMosBufUsed < NKEYBUF)
		{
			int eventType;
			switch (msg)
			{
			default:
				eventType = FSMOUSEEVENT_NONE;
				break;
			case WM_LBUTTONDOWN:
				eventType = FSMOUSEEVENT_LBUTTONDOWN;
				break;
			case WM_LBUTTONUP:
				eventType = FSMOUSEEVENT_LBUTTONUP;
				break;
			case WM_MBUTTONDOWN:
				eventType = FSMOUSEEVENT_MBUTTONDOWN;
				break;
			case WM_MBUTTONUP:
				eventType = FSMOUSEEVENT_MBUTTONUP;
				break;
			case WM_RBUTTONDOWN:
				eventType = FSMOUSEEVENT_RBUTTONDOWN;
				break;
			case WM_RBUTTONUP:
				eventType = FSMOUSEEVENT_RBUTTONUP;
				break;
			case WM_MOUSEMOVE:
				eventType = FSMOUSEEVENT_MOVE;
				break;
			}

			int lb = ((wp & MK_LBUTTON) != 0);
			int mb = ((wp & MK_MBUTTON) != 0);
			int rb = ((wp & MK_RBUTTON) != 0);
			unsigned int shift = ((wp & MK_SHIFT) != 0);
			unsigned int ctrl = ((wp & MK_CONTROL) != 0);
			int mx = LOWORD(lp);
			int my = HIWORD(lp);

			if (eventType == FSMOUSEEVENT_MOVE &&
				0 < nMosBufUsed &&
				mosBuffer[nMosBufUsed - 1].eventType == FSMOUSEEVENT_MOVE &&
				mosBuffer[nMosBufUsed - 1].lb == lb &&
				mosBuffer[nMosBufUsed - 1].mb == mb &&
				mosBuffer[nMosBufUsed - 1].rb == rb &&
				mosBuffer[nMosBufUsed - 1].shift == shift &&
				mosBuffer[nMosBufUsed - 1].ctrl == ctrl)
			{
				mosBuffer[nMosBufUsed - 1].mx = mx;
				mosBuffer[nMosBufUsed - 1].my = my;
				break;
			}

			mosBuffer[nMosBufUsed].eventType = eventType;
			mosBuffer[nMosBufUsed].lb = lb;
			mosBuffer[nMosBufUsed].mb = mb;
			mosBuffer[nMosBufUsed].rb = rb;
			mosBuffer[nMosBufUsed].shift = shift;
			mosBuffer[nMosBufUsed].ctrl = ctrl;
			mosBuffer[nMosBufUsed].mx = mx;
			mosBuffer[nMosBufUsed].my = my;
			nMosBufUsed++;
		}
		break;

	default:
		return DefWindowProc(hWnd, msg, wp, lp);
	}
	return 1;
}

static void YsSetPixelFormat(HDC hDC)
{
	PIXELFORMATDESCRIPTOR pfd =
	{
		sizeof(PIXELFORMATDESCRIPTOR),                            // nSize
		1,                                                        // nVersion
		PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,   // dwFlags
		PFD_TYPE_RGBA,                                            // iPixelType
		24,                                                       // cColorBits
		0,0,0,0,0,0,                                              // cRedBits,cRedShift,cGreenBits,cGreenShift,cBlueBits,cBlueShift
		0,                                                        // cAlphaBits
		0,                                                        // cAlphaShift
		0,                                                        // cAccumBits
		0,0,0,0,                                                  // cAccumRedBits,cAccumGreenBits,cAccumBlueBits,cAccumAlphaBits
		32,                                                       // cDepthBits
		1,                                                        // cStencilBits
		0,                                                        // cAuxBuffers
		PFD_MAIN_PLANE,                                           // iLayerType
		0,                                                        // bReserved
		0,0,0                                                     // dwLayerMask,dwVisibleMask,dwDamageMask
	};

	int pfm;
	pfm = ChoosePixelFormat(hDC, &pfd);
	if (pfm != 0)
	{
		if (SetPixelFormat(hDC, pfm, &pfd) != FALSE)
		{
			{
				PIXELFORMATDESCRIPTOR verifyPfd;
				DescribePixelFormat(hDC, pfm, sizeof(verifyPfd), &verifyPfd);
				printf("Depth Bits:   %d\n", verifyPfd.cDepthBits);
				printf("Stencil Bits: %d\n", verifyPfd.cStencilBits);
			}

			fsWin32Internal.hPlt = YsCreatePalette(hDC);
			SelectPalette(hDC, fsWin32Internal.hPlt, FALSE);
			RealizePalette(hDC);
			return;
		}
	}

	MessageBoxA(NULL, "Error In YsSetPixelFormat.", NULL, MB_OK);
	exit(1);
}

static unsigned char YsPalVal(unsigned long n, unsigned bit, unsigned sft)
{
	unsigned long msk;
	n >>= sft;
	msk = (1 << bit) - 1;
	n &= msk;
	return (unsigned char)(n * 255 / msk);
}

/* ? lp=LocalAlloc(LMEM_FIXED,sizeof(LOGPALETTE)+n*sizeof(PALETTEENTRY)); */
/* ? LocalFree(lp); */

static HPALETTE YsCreatePalette(HDC dc)
{
	HPALETTE neo;
	PIXELFORMATDESCRIPTOR pfd;
	LOGPALETTE* lp;
	int n, i;

	n = GetPixelFormat(dc);
	DescribePixelFormat(dc, n, sizeof(PIXELFORMATDESCRIPTOR), &pfd);

	if (pfd.dwFlags & PFD_NEED_PALETTE)
	{
		n = 1 << pfd.cColorBits;
		lp = (LOGPALETTE*)malloc(sizeof(LOGPALETTE) + n * sizeof(PALETTEENTRY));
		lp->palVersion = 0x300;
		lp->palNumEntries = (WORD)n;
		for (i = 0; i < n; i++)
		{
			lp->palPalEntry[i].peRed = YsPalVal(i, pfd.cRedBits, pfd.cRedShift);
			lp->palPalEntry[i].peGreen = YsPalVal(i, pfd.cGreenBits, pfd.cGreenShift);
			lp->palPalEntry[i].peBlue = YsPalVal(i, pfd.cBlueBits, pfd.cBlueShift);
			lp->palPalEntry[i].peFlags = 0;
		}

		neo = CreatePalette(lp);
		free(lp);
		return neo;
	}
	return NULL;
}

static void InitializeOpenGL(HWND wnd)
{
	RECT rect;

	GetClientRect(wnd, &rect);

	glClearColor(1.0F, 1.0F, 1.0F, 0.0F);
	glClearDepth(1.0F);
	glDisable(GL_DEPTH_TEST);

	glViewport(0, 0, rect.right, rect.bottom);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, (float)rect.right - 1, (float)rect.bottom - 1, 0, -1, 1);

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glShadeModel(GL_FLAT);
	glPointSize(1);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glColor3ub(0, 0, 0);
}

int FsGetNumCurrentTouch(void)
{
	return nCurrentTouch;
}

const FsVec2i* FsGetCurrentTouch(void)
{
	return currentTouch;
}

void FsChangeToProgramDir(void)
{
	char fn[MAX_PATH];
	GetModuleFileNameA(NULL, fn, MAX_PATH);

	const int l = (int)strlen(fn);
	int i;
	for (i = l - 1; i >= 0; i--)
	{
		if (fn[i] == '\\' || fn[i] == '/')
		{
			fn[i] = 0;
			break;
		}
	}

	_chdir(fn);
}



class FsWin32WaitInTheLastMoment
{
public:
	~FsWin32WaitInTheLastMoment();
};

FsWin32WaitInTheLastMoment::~FsWin32WaitInTheLastMoment()
{
	// This function will wait for 100ms before the program terminates.
	// If I let Windows destroy all the resources, this wait is apparently necessary, or
	// Windows falls into the dead lock.  
	// Well, it's my sloppy programming.  :-P
	FsSleep(100);
}

static FsWin32WaitInTheLastMoment fsWait100msInTheEnd;

////////////////////////////////////////////////////////////

int FsEnableIME(void)
{
	// See Memo about IME at the beginning of this source.
	// Actually, I don't have to do anything here and in FsDisableIME because PeekMessage with NULL as hWnd
	// allows the user to turn on/off the IME from the short-cut.
	// Calling ImmSetOpenStatus forces Japanese input mode turned on/off which is quite annoying.

	// auto hWnd=fsWin32Internal.hWnd;
	// HIMC hImc=ImmGetContext(hWnd);
	// if(NULL!=hImc)
	// {
	// 	ImmSetOpenStatus(hImc,true);
	// 	ImmReleaseContext(hWnd,hImc);
	// }
	return 1;
}

void FsDisableIME(void)
{
	// auto hWnd=fsWin32Internal.hWnd;
	// HIMC hImc=ImmGetContext(hWnd);
	// if(NULL!=hImc)
	// {
	// 	ImmSetOpenStatus(hImc,false);
	// 	ImmReleaseContext(hWnd,hImc);
	// }
}

////////////////////////////////////////////////////////////

int FsIsNativeTextInputAvailable(void)
{
	return 0;
}

int FsOpenNativeTextInput(int, int, int, int)
{
	return 0;
}

void FsCloseNativeTextInput(void)
{
}

void FsSetNativeTextInputText(const wchar_t[])
{
}

int FsGetNativeTextInputTextLength(void)
{
	return 0;
}

void FsGetNativeTextInputText(wchar_t str[], int bufLen)
{
	if (0 < bufLen)
	{
		str[0] = 0;
	}
}

int FsGetNativeTextInputEvent(void)
{
	return FSNATIVETEXTEVENT_NONE;
}
/* ////////////////////////////////////////////////////////////

File Name: fswin32keymap.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */






















FsWin32KeyMapper::FsWin32KeyMapper()
{
	int i;

	mapVKtoFSKEY = new int[FS_NUM_VK];
	mapFSKEYtoVK = new int[FSKEY_NUM_KEYCODE];

	for (i = 0; i < FS_NUM_VK; i++)
	{
		mapVKtoFSKEY[i] = FSKEY_NULL;
	}
	for (i = 0; i < FSKEY_NUM_KEYCODE; i++)
	{
		mapFSKEYtoVK[i] = 0;
	}

	AddKeyMapping(FSKEY_SPACE, VK_SPACE);
	AddKeyMapping(FSKEY_0, '0');
	AddKeyMapping(FSKEY_1, '1');
	AddKeyMapping(FSKEY_2, '2');
	AddKeyMapping(FSKEY_3, '3');
	AddKeyMapping(FSKEY_4, '4');
	AddKeyMapping(FSKEY_5, '5');
	AddKeyMapping(FSKEY_6, '6');
	AddKeyMapping(FSKEY_7, '7');
	AddKeyMapping(FSKEY_8, '8');
	AddKeyMapping(FSKEY_9, '9');
	AddKeyMapping(FSKEY_A, 'A');
	AddKeyMapping(FSKEY_B, 'B');
	AddKeyMapping(FSKEY_C, 'C');
	AddKeyMapping(FSKEY_D, 'D');
	AddKeyMapping(FSKEY_E, 'E');
	AddKeyMapping(FSKEY_F, 'F');
	AddKeyMapping(FSKEY_G, 'G');
	AddKeyMapping(FSKEY_H, 'H');
	AddKeyMapping(FSKEY_I, 'I');
	AddKeyMapping(FSKEY_J, 'J');
	AddKeyMapping(FSKEY_K, 'K');
	AddKeyMapping(FSKEY_L, 'L');
	AddKeyMapping(FSKEY_M, 'M');
	AddKeyMapping(FSKEY_N, 'N');
	AddKeyMapping(FSKEY_O, 'O');
	AddKeyMapping(FSKEY_P, 'P');
	AddKeyMapping(FSKEY_Q, 'Q');
	AddKeyMapping(FSKEY_R, 'R');
	AddKeyMapping(FSKEY_S, 'S');
	AddKeyMapping(FSKEY_T, 'T');
	AddKeyMapping(FSKEY_U, 'U');
	AddKeyMapping(FSKEY_V, 'V');
	AddKeyMapping(FSKEY_W, 'W');
	AddKeyMapping(FSKEY_X, 'X');
	AddKeyMapping(FSKEY_Y, 'Y');
	AddKeyMapping(FSKEY_Z, 'Z');
	AddKeyMapping(FSKEY_ESC, VK_ESCAPE);
	AddKeyMapping(FSKEY_F1, VK_F1);
	AddKeyMapping(FSKEY_F2, VK_F2);
	AddKeyMapping(FSKEY_F3, VK_F3);
	AddKeyMapping(FSKEY_F4, VK_F4);
	AddKeyMapping(FSKEY_F5, VK_F5);
	AddKeyMapping(FSKEY_F6, VK_F6);
	AddKeyMapping(FSKEY_F7, VK_F7);
	AddKeyMapping(FSKEY_F8, VK_F8);
	AddKeyMapping(FSKEY_F9, VK_F9);
	AddKeyMapping(FSKEY_F10, VK_F10);
	AddKeyMapping(FSKEY_F11, VK_F11);
	AddKeyMapping(FSKEY_F12, VK_F12);
	AddKeyMapping(FSKEY_PRINTSCRN, 0 /* Unassignable */);
	AddKeyMapping(FSKEY_SCROLLLOCK, VK_SCROLL);
	AddKeyMapping(FSKEY_PAUSEBREAK, VK_PAUSE);
	AddKeyMapping(FSKEY_TILDA, VK_OEM_3);
	AddKeyMapping(FSKEY_MINUS, VK_OEM_MINUS);
	AddKeyMapping(FSKEY_PLUS, VK_OEM_PLUS);
	AddKeyMapping(FSKEY_BS, VK_BACK);
	AddKeyMapping(FSKEY_TAB, VK_TAB);
	AddKeyMapping(FSKEY_LBRACKET, VK_OEM_4);
	AddKeyMapping(FSKEY_RBRACKET, VK_OEM_6);
	AddKeyMapping(FSKEY_BACKSLASH, VK_OEM_5);
	AddKeyMapping(FSKEY_CAPSLOCK, VK_CAPITAL);
	AddKeyMapping(FSKEY_SEMICOLON, VK_OEM_1);
	AddKeyMapping(FSKEY_SINGLEQUOTE, VK_OEM_7);
	AddKeyMapping(FSKEY_ENTER, VK_RETURN);
	AddKeyMapping(FSKEY_SHIFT, VK_SHIFT);
	AddKeyMapping(FSKEY_COMMA, VK_OEM_COMMA);
	AddKeyMapping(FSKEY_DOT, VK_OEM_PERIOD);
	AddKeyMapping(FSKEY_SLASH, VK_OEM_2);
	AddKeyMapping(FSKEY_CTRL, VK_CONTROL);
	AddKeyMapping(FSKEY_ALT, VK_MENU);
	AddKeyMapping(FSKEY_INS, VK_INSERT);
	AddKeyMapping(FSKEY_DEL, VK_DELETE);
	AddKeyMapping(FSKEY_HOME, VK_HOME);
	AddKeyMapping(FSKEY_END, VK_END);
	AddKeyMapping(FSKEY_PAGEUP, VK_PRIOR);
	AddKeyMapping(FSKEY_PAGEDOWN, VK_NEXT);
	AddKeyMapping(FSKEY_UP, VK_UP);
	AddKeyMapping(FSKEY_DOWN, VK_DOWN);
	AddKeyMapping(FSKEY_LEFT, VK_LEFT);
	AddKeyMapping(FSKEY_RIGHT, VK_RIGHT);
	AddKeyMapping(FSKEY_NUMLOCK, VK_NUMLOCK);
	AddKeyMapping(FSKEY_TEN0, VK_NUMPAD0);
	AddKeyMapping(FSKEY_TEN1, VK_NUMPAD1);
	AddKeyMapping(FSKEY_TEN2, VK_NUMPAD2);
	AddKeyMapping(FSKEY_TEN3, VK_NUMPAD3);
	AddKeyMapping(FSKEY_TEN4, VK_NUMPAD4);
	AddKeyMapping(FSKEY_TEN5, VK_NUMPAD5);
	AddKeyMapping(FSKEY_TEN6, VK_NUMPAD6);
	AddKeyMapping(FSKEY_TEN7, VK_NUMPAD7);
	AddKeyMapping(FSKEY_TEN8, VK_NUMPAD8);
	AddKeyMapping(FSKEY_TEN9, VK_NUMPAD9);
	AddKeyMapping(FSKEY_TENDOT, VK_DECIMAL);
	AddKeyMapping(FSKEY_TENSLASH, VK_DIVIDE);
	AddKeyMapping(FSKEY_TENSTAR, VK_MULTIPLY);
	AddKeyMapping(FSKEY_TENMINUS, VK_SUBTRACT);
	AddKeyMapping(FSKEY_TENPLUS, VK_ADD);
	AddKeyMapping(FSKEY_TENENTER, 0 /* Unassignable */);
	AddKeyMapping(FSKEY_CONTEXT, 0 /* Unassignable */);

	AddKeyMapping(FSKEY_CONVERT, VK_CONVERT);
	AddKeyMapping(FSKEY_NONCONVERT, VK_NONCONVERT);
	AddKeyMapping(FSKEY_KANA, VK_KANA);       // Japanese JIS Keyboard Only => Win32 VK_KANA

	AddKeyMapping(FSKEY_LEFT_CTRL, VK_LCONTROL);
	AddKeyMapping(FSKEY_RIGHT_CTRL, VK_RCONTROL);
	AddKeyMapping(FSKEY_LEFT_SHIFT, VK_LSHIFT);
	AddKeyMapping(FSKEY_RIGHT_SHIFT, VK_RSHIFT);
	AddKeyMapping(FSKEY_LEFT_ALT, VK_LMENU);
	AddKeyMapping(FSKEY_RIGHT_ALT, VK_RMENU);
}

void FsWin32KeyMapper::AddKeyMapping(int fskey, int vk)
{
	if (fskey < 0 || FSKEY_NUM_KEYCODE <= fskey)
	{
		printf("FSKEY is out of range\n");
		exit(1);
	}
	if (vk < 0 || FS_NUM_VK <= vk)
	{
		printf("VK is out of range\n");
		exit(1);
	}

	mapVKtoFSKEY[vk] = fskey;
	mapFSKEYtoVK[fskey] = vk;
}

FsWin32KeyMapper::~FsWin32KeyMapper()
{
	delete[] mapFSKEYtoVK;
	delete[] mapVKtoFSKEY;
}

int FsWin32KeyMapper::VkToFsKey(int vk)
{
	if (0 <= vk && vk < FS_NUM_VK)
	{
		return mapVKtoFSKEY[vk];
	}
	return FSKEY_NULL;
}

int FsWin32KeyMapper::FsKeyToVk(int fsKey)
{
	if (0 <= fsKey && fsKey < FSKEY_NUM_KEYCODE)
	{
		return mapFSKEYtoVK[fsKey];
	}
	return 0;
}
/* ////////////////////////////////////////////////////////////

File Name: fssimplewindowcommon.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */







FsOpenWindowOption::FsOpenWindowOption()
{
	x0 = 16;
	y0 = 16;
	wid = 800;
	hei = 600;

	windowTitle = "Main Window";

	useDoubleBuffer = false;
	useMultiSampleBuffer = false;

	sizeOpt = NORMAL_WINDOW;
}

void FsOpenWindow(int x0, int y0, int wid, int hei, int useDoubleBuffer)
{
	FsOpenWindow(x0, y0, wid, hei, useDoubleBuffer, NULL);
}

void FsOpenWindow(int x0, int y0, int wid, int hei, int useDoubleBuffer, const char windowName[])
{
	FsOpenWindowOption opt;
	opt.x0 = x0;
	opt.y0 = y0;
	opt.wid = wid;
	opt.hei = hei;
	opt.windowTitle = windowName;
	opt.useDoubleBuffer = (bool)useDoubleBuffer;
	FsOpenWindow(opt);
}

void FsClearEventQueue(void)
{
	auto intervalFunc = fsIntervalCallBack;
	auto intervalParam = fsIntervalCallBackParameter;
	FsRegisterIntervalCallBack(NULL, NULL);

	for (;;)
	{
		int checkAgain = 0;

		FsPollDevice();

		int lb, mb, rb, mx, my;
		while (FSMOUSEEVENT_NONE != FsGetMouseEvent(lb, mb, rb, mx, my) ||
			FSKEY_NULL != FsInkey() ||
			0 != FsInkeyChar() ||
			0 != FsCheckWindowExposure())
		{
			checkAgain = 1;
		}

		if (0 != lb || 0 != rb || 0 != mb)
		{
			checkAgain = 1;
		}

		if (0 != FsCheckKeyHeldDown())
		{
			checkAgain = 1;
		}

		if (0 == checkAgain)
		{
			break;
		}

		FsSleep(50);
	}

	FsRegisterIntervalCallBack(intervalFunc, intervalParam);
}

int FsCheckKeyHeldDown(void)
{
	int keyCode;
	for (keyCode = FSKEY_NULL + 1; keyCode < FSKEY_NUM_KEYCODE; keyCode++)
	{
		if (0 != FsGetKeyState(keyCode))
		{
			return 1;
		}
	}
	return 0;
}


static const char* const keyCodeToStr[] =
{
	"NULL",
	"SPACE",
	"0",
	"1",
	"2",
	"3",
	"4",
	"5",
	"6",
	"7",
	"8",
	"9",
	"A",
	"B",
	"C",
	"D",
	"E",
	"F",
	"G",
	"H",
	"I",
	"J",
	"K",
	"L",
	"M",
	"N",
	"O",
	"P",
	"Q",
	"R",
	"S",
	"T",
	"U",
	"V",
	"W",
	"X",
	"Y",
	"Z",
	"ESC",
	"F1",
	"F2",
	"F3",
	"F4",
	"F5",
	"F6",
	"F7",
	"F8",
	"F9",
	"F10",
	"F11",
	"F12",
	"PRINTSCRN",
	"CAPSLOCK",
	"SCROLLLOCK",
	"PAUSEBREAK",
	"BS",
	"TAB",
	"ENTER",
	"SHIFT",
	"CTRL",
	"ALT",
	"INS",
	"DEL",
	"HOME",
	"END",
	"PAGEUP",
	"PAGEDOWN",
	"UP",
	"DOWN",
	"LEFT",
	"RIGHT",
	"NUMLOCK",
	"TILDA",
	"MINUS",
	"PLUS",
	"LBRACKET",
	"RBRACKET",
	"BACKSLASH",
	"SEMICOLON",
	"SINGLEQUOTE",
	"COMMA",
	"DOT",
	"SLASH",
	"TEN0",
	"TEN1",
	"TEN2",
	"TEN3",
	"TEN4",
	"TEN5",
	"TEN6",
	"TEN7",
	"TEN8",
	"TEN9",
	"TENDOT",
	"TENSLASH",
	"TENSTAR",
	"TENMINUS",
	"TENPLUS",
	"TENENTER",
	"WHEELUP",
	"WHEELDOWN",
	"CONTEXT",

	"HENKAN",
	"MUHENKAN",
	"KANA",       // Japanese JIS Keyboard Only => Win32 VK_KANA

	"LEFT_CTRL",
	"RIGHT_CTRL",
	"LEFT_SHIFT",
	"RIGHT_SHIFT",
	"LEFT_ALT",
	"RIGHT_ALT",
};

const char* FsKeyCodeToString(int keyCode)
{
	if (0 <= keyCode && keyCode < FSKEY_NUM_KEYCODE)
	{
		return keyCodeToStr[keyCode];
	}
	return "(Undefined keycode)";
}

int FsStringToKeyCode(const char str[])
{
	if (nullptr == str)
	{
		return FSKEY_NULL;
	}

	char upper[256];
	for (int i = 0; i < 255 && 0 != str[i]; ++i)
	{
		upper[i] = str[i];
		upper[i + 1] = 0;
		if ('a' <= upper[i] && upper[i] <= 'z')
		{
			upper[i] = upper[i] + 'A' - 'a';
		}
	}
	for (int i = 0; i < FSKEY_NUM_KEYCODE; ++i)
	{
		if (0 == strcmp(upper, keyCodeToStr[i]))
		{
			return i;
		}
	}
	return FSKEY_NULL;
}


void* fsCloseWindowCallBackParam = NULL;
bool (*fsCloseWindowCallBack)(void* param) = NULL;

void* fsOpenGLContextCreationCallBackParam = NULL;
bool (*fsOpenGLContextCreationCallBack)(void* param) = NULL;

void* fsAfterWindowCreationCallBackParam = NULL;
void (*fsAfterWindowCreationCallBack)(void* param) = NULL;

void* fsOpenGLInitializationCallBackParam = NULL;
bool (*fsOpenGLInitializationCallBack)(void* param) = NULL;

void* fsSwapBuffersHookParam = NULL;
bool (*fsSwapBuffersHook)(void* param) = NULL;

void* fsOnPaintCallbackParam = NULL;
void (*fsOnPaintCallback)(void* param) = NULL;

void* fsOnResizeCallBackParam = NULL;
void (*fsOnResizeCallBack)(void* param, int wid, int hei) = NULL;

void (*fsIntervalCallBack)(void*) = NULL;
void* fsIntervalCallBackParameter = NULL;

void (*fsPollDeviceHook)(void*) = NULL;
void* fsPollDeviceHookParam = NULL;

void FsRegisterCloseWindowCallBack(bool (*callback)(void*), void* param)
{
	fsCloseWindowCallBack = callback;
	fsCloseWindowCallBackParam = param;
}

void FsRegisterBeforeOpenGLContextCreationCallBack(bool (*callback)(void*), void* param)
{
	fsOpenGLContextCreationCallBack = callback;
	fsOpenGLContextCreationCallBackParam = param;
}

void FsRegisterAfterWindowCreationCallBack(void (*callback)(void*), void* param)
{
	fsAfterWindowCreationCallBackParam = param;
	fsAfterWindowCreationCallBack = callback;
}

void FsRegisterOpenGLInitializationCallBack(bool (*callback)(void*), void* param)
{
	fsOpenGLInitializationCallBack = callback;
	fsOpenGLInitializationCallBackParam = param;
}

void FsRegisterSwapBuffersCallBack(bool (*callback)(void*), void* param)
{
	fsSwapBuffersHookParam = param;
	fsSwapBuffersHook = callback;
}

void FsRegisterOnPaintCallBack(void (*callback)(void*), void* param)
{
	fsOnPaintCallbackParam = param;
	fsOnPaintCallback = callback;
}

void FsRegisterWindowResizeCallBack(void (*callback)(void*, int wid, int hei), void* param)
{
	fsOnResizeCallBackParam = param;
	fsOnResizeCallBack = callback;
}

void FsRegisterIntervalCallBack(void (*callback)(void*), void* param)
{
	fsIntervalCallBack = callback;
	fsIntervalCallBackParameter = param;
}

void FsRegisterPollDeviceCallBack(void (*callback)(void*), void* param)
{
	fsPollDeviceHook = callback;
	fsPollDeviceHookParam = param;
}

#include <ctype.h>
/* ////////////////////////////////////////////////////////////

File Name: fswin32winmain.cpp
Copyright (c) 2017 Soji Yamakawa.  All rights reserved.
http://www.ysflight.com

Redistribution and use in source and binary forms, with or without modification,
are permitted provided that the following conditions are met:

1. Redistributions of source code must retain the above copyright notice,
   this list of conditions and the following disclaimer.

2. Redistributions in binary form must reproduce the above copyright notice,
   this list of conditions and the following disclaimer in the documentation
   and/or other materials provided with the distribution.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE
GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

//////////////////////////////////////////////////////////// */





#undef PSAPI_VERSION








static char* YsStrtok(char** src);
static int YsArguments(int* ac, char* av[], int mxac, char* src);

extern int main(int ac, char* av[]);

int PASCAL WinMain(HINSTANCE inst, HINSTANCE, LPSTR param, int)
{
	int ac;
	static char* av[512], tmp[4096], prog[MAX_PATH];

	strcpy(prog, "Unknown");
	GetModuleFileNameA(inst, prog, 260);

	strncpy(tmp, param, 256);
	av[0] = prog;

	YsArguments(&ac, av + 1, 510, tmp);

	return main(ac + 1, av);
}




static int YsArguments(int* ac, char* av[], int mxac, char* src)
{
	char* arg;
	int len;

	while (*src == ' ' || *src == '\t')
	{
		src++;
	}

	len = (int)strlen(src) - 1;
	while (len >= 0 && isprint(src[len]) == 0)
	{
		src[len] = 0;
		len--;
	}

	*ac = 0;
	while ((arg = YsStrtok(&src)) != NULL && *ac < mxac)
	{
		av[*ac] = arg;
		(*ac)++;
	}

	if (*ac < mxac)  // 2007/06/22 Added this check.
	{
		av[*ac] = NULL;
	}

	return *ac;
}

static char* YsStrtok(char** src)
{
	char* r;
	switch (**src)
	{
	case 0:
		return NULL;
	case '\"':
		(*src)++;
		r = (*src);
		while ((**src) != '\"' && (**src) != 0)
		{
			(*src)++;
		}
		break;
	default:
		r = (*src);
		while ((**src) != ' ' && (**src) != '\t' && (**src) != 0)
		{
			(*src)++;
		}
		break;
	}
	if ((**src) != 0)
	{
		(**src) = 0;
		(*src)++;
	}
	while ((**src) != 0 && ((**src) == ' ' || (**src) == '\t'))
	{
		(*src)++;
	}
	return r;
}