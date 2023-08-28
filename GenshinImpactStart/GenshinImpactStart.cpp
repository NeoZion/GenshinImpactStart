#include <iostream>
#include <Windows.h>
#include <opencv2/highgui/highgui.hpp>
#include <mmsystem.h>
#include <thread>
#pragma comment(lib,"winmm.lib")

using namespace std; 
using namespace cv;

unsigned long __stdcall Sound(void* lp) 
{
	PlaySound(TEXT("./genshin.wav"), NULL, SND_FILENAME);
	return 0;
}

unsigned long __stdcall Video(void* lp)
{
	VideoCapture capture("./genshin.mp4");
	if (!capture.isOpened())
	{
		return -1;
	}

	Mat frame;
	while (1)
	{
		if (!capture.read(frame))
			break;
		namedWindow("原神", WINDOW_NORMAL);
		setWindowProperty("原神", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
		setWindowProperty("原神", WND_PROP_TOPMOST, 1);
		HWND hwnd = FindWindow(0, L"原神");
		SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(0, 0, 0)));
		imshow("原神", frame);
		char c = waitKey(7);
		if (c == 36)
			break;
	}
	capture.release();
	destroyWindow("原神");
	return 0;
}

bool DetectScreen()
{
	COLORREF rgbwhite = 0xFFFFFFFF;
	double whitecount = 0.0;

	double width = GetSystemMetrics(SM_CXSCREEN);
	double height = GetSystemMetrics(SM_CYSCREEN);

	HDC hScreenDC = GetDC(NULL); 
	HDC hMemDC = CreateCompatibleDC(hScreenDC); 

	BITMAPINFO bmi; 
	ZeroMemory(&bmi, sizeof(BITMAPINFO)); 
	bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmi.bmiHeader.biWidth = width;
	bmi.bmiHeader.biHeight = -height;
	bmi.bmiHeader.biPlanes = 1;
	bmi.bmiHeader.biBitCount = 32;
	bmi.bmiHeader.biCompression = BI_RGB;

	LPVOID pBits;
	HBITMAP hDIB = CreateDIBSection(hMemDC, &bmi, DIB_RGB_COLORS, &pBits, NULL, 0);

	HBITMAP hOldBitmap = (HBITMAP)SelectObject(hMemDC, hDIB);

	BitBlt(hMemDC, 0, 0, width, height, hScreenDC, 0, 0, SRCCOPY);

	COLORREF* pPixel = (COLORREF*)pBits;
	for (int y = 0; y < height; y++)
	{
		for (int x = 0; x < width; x++)
		{
			if (*pPixel++ == rgbwhite)
			{
				whitecount++;
			}

		}
	}

	ReleaseDC(NULL, hScreenDC);
	DeleteDC(hMemDC);
	DeleteObject(hDIB);

	return (whitecount / (width * height)) > 0.9 ? TRUE : FALSE;
}

void HideWindow()
{
	auto hwnd = GetForegroundWindow();
	if (hwnd)
	{
		ShowWindow(hwnd, SW_HIDE);
	}
}

int main()
{
	HideWindow();
	while (true)
	{
		if (DetectScreen()) 
		{
			const int EVENT_NUM = 2;
			auto t1 = CreateThread(NULL, 0, Sound, NULL, 0, NULL);
			auto t2 = CreateThread(NULL, 0, Video, NULL, 0, NULL);
			HANDLE hobject[EVENT_NUM] = { t1,t2 };
			
			if (WaitForMultipleObjects(EVENT_NUM, hobject, FALSE, INFINITE))
			{
				CloseHandle(t1);
				CloseHandle(t2);
				exit(1);
			}
		}

		Sleep(100);
	}
	return 0;
}
