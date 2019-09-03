#include "Application.h"
#include <fstream>


ofstream g_debug("debug.txt");


IDirect3DDevice9* g_pDevice = NULL;
ID3DXEffect* g_pEffect = NULL;


#define SCREEN_WIDTH 1920
#define SCREEN_HEIGHT 1080


bool KeyDown(int vk_code) { return (GetAsyncKeyState(vk_code) & 0x8000) ? true : false; }
bool KeyUp(int vk_code) { return (GetAsyncKeyState(vk_code) & 0x8000) ? false : true; }

LRESULT CALLBACK WindowProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam);

using namespace std;

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE prevInstance, LPSTR cmdLine, int showCmd)
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


HRESULT Application::Init(HINSTANCE hInstance, bool windowed)
{
	g_debug << "Application started \n";

	WNDCLASSEX wc;

	ZeroMemory(&wc, sizeof(WNDCLASSEX));
	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)COLOR_WINDOW;//tell windows leave background to us
	wc.lpszClassName = L"WindowClass1";


	RECT rc = { 0, 0, SCREEN_WIDTH, SCREEN_HEIGHT };
	AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);

	//创建窗口类
	//注册窗口类
	//创建窗口实例
	//显示窗口实例
	//更新窗口实例

	//创建d3d9接口
	//通过d3d9接口 创建d3d设备device
	//设置d3d 显示 参数
	RegisterClassEx(&wc);
	m_mainWindow = CreateWindowEx(NULL,
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
	ShowWindow(m_mainWindow, SW_SHOW);
	UpdateWindow(m_mainWindow);

	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	if (FAILED( d3d9 ))
	{
		g_debug << "Direct3DCreate9 failed";
		return E_FAIL;
	}

	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	if (caps.VertexShaderVersion < D3DVS_VERSION(2,0) || caps.PixelShaderVersion < D3DPS_VERSION(2,0))
	{
		g_debug << "warning your device not support vs or ps in version 2.0";
	}

	m_present.BackBufferWidth = SCREEN_WIDTH;
	m_present.BackBufferHeight = SCREEN_HEIGHT;
	m_present.BackBufferCount = 2;
	m_present.BackBufferFormat = D3DFMT_A8R8G8B8;
	m_present.MultiSampleQuality = 0;
	m_present.MultiSampleType = D3DMULTISAMPLE_NONE;
	m_present.SwapEffect = D3DSWAPEFFECT_DISCARD;
	m_present.hDeviceWindow = m_mainWindow;
	m_present.Windowed = windowed;
	m_present.EnableAutoDepthStencil = true;
	m_present.AutoDepthStencilFormat = D3DFMT_D24S8;
	m_present.Flags = 0;
	m_present.FullScreen_RefreshRateInHz = D3DPRESENT_RATE_DEFAULT;
	m_present.PresentationInterval = D3DPRESENT_INTERVAL_IMMEDIATE;

	int vertexProcessingType = 0;
	if (caps.DevCaps & D3DDEVCAPS_HWTRANSFORMANDLIGHT)
	{
		vertexProcessingType = D3DCREATE_HARDWARE_VERTEXPROCESSING;
	}
	else
	{
		vertexProcessingType = D3DCREATE_SOFTWARE_VERTEXPROCESSING;
	}

	if (FAILED(d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, m_mainWindow, vertexProcessingType, &m_present, &g_pDevice)))
	{
		g_debug << "failed to create d3d device ";
	}

	d3d9->Release();
	ID3DXBuffer* pErrorMsgs = NULL;
	HRESULT hRes = D3DXCreateEffectFromFile(g_pDevice, L"resources/fx/lighting.fx", NULL, NULL, D3DXSHADER_DEBUG, NULL, &g_pEffect, &pErrorMsgs);

	if (FAILED(hRes) && (pErrorMsgs != NULL))
	{
		MessageBox(NULL, (LPCWSTR)pErrorMsgs->GetBufferPointer(), L"effect error", MB_OK);
		return E_FAIL;
	}

	m_soldier.Load(L"resources/meshes/soldier.x");
	m_deviceLost = false;

	return S_OK;
}


void Application::DeviceLost()
{
	try
	{
		//g_pFont->OnLostDevice();
		//g_pSprite->OnLostDevice();
		g_pEffect->OnLostDevice();
		m_deviceLost = true;
	}
	catch (...)
	{
		g_debug << "Error occured in Application::DeviceLost() \n";
	}
}

void Application::DeviceGained()
{
	try
	{
		g_pDevice->Reset(&m_present);
		//g_pFont->OnResetDevice();
		//g_pSprite->OnResetDevice();
		g_pEffect->OnResetDevice();
		m_deviceLost = false;
	}
	catch (...)
	{
		g_debug << "Error occured in Application::DeviceGained() \n";
	}
}

void Application::Update(float deltaTime)
{
	try
	{
		HRESULT coop = g_pDevice->TestCooperativeLevel();
		if (coop != D3D_OK)
		{
			if (coop == D3DERR_DEVICELOST)
			{
				if (m_deviceLost == false)
					DeviceLost();
			}
			else if (coop == D3DERR_DEVICENOTRESET)
			{
				if (m_deviceLost == true)
					DeviceGained();
			}

			Sleep(100);
			return;
		}

		m_angle += deltaTime;
		if (KeyDown(VK_ESCAPE))
		{
			Quit();
		}

		if (KeyDown(VK_RETURN) && KeyDown(18))
		{
			m_present.Windowed = !m_present.Windowed;
			DeviceLost();
			DeviceGained();

			if (m_present.Windowed)
			{
				RECT rc = {0, 0, SCREEN_WIDTH, SCREEN_HEIGHT};
				AdjustWindowRect(&rc, WS_OVERLAPPEDWINDOW, false);
				SetWindowPos(m_mainWindow, HWND_NOTOPMOST, 0, 0, rc.right - rc.left, rc.bottom - rc.top, SWP_SHOWWINDOW);
				UpdateWindow(m_mainWindow);
			}
		}


	}
	catch (...)
	{
		g_debug << "Error in Application::Update() \n";
	}
}


void Application::Render()
{
	if (!m_deviceLost)
	{
		try
		{
			D3DXMATRIX identity, shadow;
			D3DXMatrixIdentity(&identity);

			D3DXPLANE ground(0.0f, 1.0f, 0.0f, 0.0f);
			D3DXVECTOR4 lightPos(-20.f, 75.f, -120.0f, 0.0f);

			//why shadow matrix like this
			D3DXMatrixShadow(&shadow, &lightPos, &ground);

			D3DXMATRIX view, proj, world;
			D3DXMatrixIdentity(&world);
			D3DXMatrixLookAtLH(&view, &D3DXVECTOR3(cos(m_angle) * 2.0f, 2.0f, sin(m_angle)), &D3DXVECTOR3(0.0f, 1.0f, 0.0f), &D3DXVECTOR3(0.0f, 10.f, 0.0f));
			D3DXMatrixPerspectiveFovLH(&proj, D3DX_PI / 4.0f, (float)SCREEN_WIDTH / SCREEN_HEIGHT, 0.1f, 1000.f);




		}
		catch (...)
		{

		}
	}
}