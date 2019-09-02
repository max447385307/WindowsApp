#include "Application.h"


#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080
LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd)
{
	//Application app;

	//if (FAILED(app.Init(hInstance, true))) 
	//	return 0;

	HWND hWnd;
	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;//tell windows leave background to us
	wc.lpszClassName = L"WindowClass1";

	RegisterClassEx(&wc);
	hWnd = CreateWindowEx(NULL,
						  L"WindowClass1",
						  L"Our First Windowed Program",
						  //WS_EX_TOPMOST | WS_POPUP,//full screen or windowed
						  WS_OVERLAPPEDWINDOW,
						  0,//x  left 
						  0,//y  top
						  SCREEN_WIDTH,
						  SCREEN_HEIGHT,
						  NULL,
						  NULL,
						  hInstance,
						  NULL);
	BOOL isSuccess = ShowWindow(hWnd, showCmd);
	if (isSuccess)
	{

	} 

	MSG msg;

	memset(&msg, 0, sizeof(MSG));

	DWORD startTime = GetTickCount64();

	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			DWORD t = GetTickCount64();
			float deltaTime = (t - startTime) * 0.001f;
			//app.Update(deltaTime);
			//app.Render();
			startTime = t;
		}
	}

	//app.Cleanup();

	return (int)msg.wParam;

	//return 0;
}

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch(message)
	{
		case WM_DESTROY:
		{
			PostQuitMessage(0);
			return 0;
		}break;
	}
	return DefWindowProc(hWnd, message, wParam, lParam);
}