#include <iostream>
#include <Windows.h>
#include <opencv2/highgui/highgui.hpp>
#include <mmsystem.h>
#include <thread>
#pragma comment(lib,"winmm.lib")

using namespace std; 
using namespace cv;

unsigned long __stdcall sound(void* lp) 
{
	PlaySound(TEXT("./genshin.wav"), NULL, SND_FILENAME);
	return 0;
}

unsigned long __stdcall video(void* lp)
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
		namedWindow("mainWin", WINDOW_NORMAL);
		setWindowProperty("mainWin", WND_PROP_FULLSCREEN, WINDOW_FULLSCREEN);
		setWindowProperty("mainWin", WND_PROP_TOPMOST, 1);
		HWND hwnd = FindWindow(0, L"mainWin");
		SetClassLongPtr(hwnd, GCLP_HBRBACKGROUND, (LONG)CreateSolidBrush(RGB(0, 0, 0)));
		imshow("mainWin", frame);
		char c = waitKey(7);
		if (c == 36)
			break;
	}
	capture.release();
	destroyWindow("mainWin");
	return 0;
}

bool DetectScreen()
{
	HDC hdc = GetDC(NULL);
	auto counter = 0;
	auto ScreenWidth  = GetDeviceCaps(hdc, HORZRES);
	auto ScreenHeight = GetDeviceCaps(hdc, VERTRES);
	for (int width = ScreenWidth / 3; width < 2*ScreenWidth /3; width = width + 100) {
		for (int height = ScreenWidth / 3; height < 2*ScreenWidth/3; height = height + 100) {
			auto rgb = GetPixel(hdc, width, height);
			if (rgb != RGB(255, 255, 255)) {
				counter += 1;
				if (counter > 6) {
					ReleaseDC(NULL, hdc);
					return FALSE;
				}
			}
		}
	}
	ReleaseDC(NULL,hdc);
	return TRUE;
}

void hidewindow()
{
	auto hwnd = GetForegroundWindow();
	if (hwnd)
	{
		ShowWindow(hwnd, SW_HIDE);
	}
}

int main()
{
	hidewindow();
	while (true)
	{
		if (DetectScreen() == TRUE) {
			const int EVENT_NUM = 2;
			HANDLE hobject[EVENT_NUM] = { NULL,NULL };
			auto t1 = CreateThread(NULL, 0, sound, NULL, 0, NULL);
			auto t2 = CreateThread(NULL, 0, video, NULL, 0, NULL);
			Sleep(25000);
			exit(1);
		}

		Sleep(100);
	}
	return 0;
}