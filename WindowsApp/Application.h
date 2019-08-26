#pragma once

#include <windows.h>
#include <d3dx9.h>

#pragma comment (lib, "d3d9.lib")
//#pragma comment (lib, "D3dx9.lib")

class Application
{
public: 
	Application(); 
	~Application(); 
	HRESULT Init(HINSTANCE hInstance, bool windowed);
	void Update(float deltaTime); 
	void Render(); 
	void Cleanup(); 
	void Quit();
	void DeviceLost(); 
	void DeviceGained();

private: 
	HWND m_mainWindow; 
	D3DPRESENT_PARAMETERS m_present; bool m_deviceLost;
};

