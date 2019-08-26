#include "Application.h"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd)
{
	Application app;

	if (FAILED(app.Init(hInstance, true))) 
		return 0;

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
			app.Update(deltaTime);
			app.Render();
			startTime = t;
		}
	}

	app.Cleanup();

	return (int)msg.wParam;

	//return 0;
}