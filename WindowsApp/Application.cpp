#include "Application.h"



int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, PSTR cmdLine, int showCmd) {

	Application app;

	if (FAILED(app.Init(hInstance, true))) 
		return 0;

	MSG msg;

	memset(&msg, 0, sizeof(MSG));

	DWORD startTime = GetTickCount();



}