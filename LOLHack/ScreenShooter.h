#pragma once
#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>
#include <Windows.h>
#include "DirectxHelper.hpp"
#include <d3d9.h>
#include <d3d9helper.h>
#include <winnt.h>
#include <dinput.h>
#pragma comment(lib,"d3d9.lib") 
using namespace cv;
using namespace std;

class ScreenShooter {
private:

	static BITMAPINFOHEADER createBitmapHeader(int width, int height);

	static inline HWND hwnd;
	static inline cuda::GpuMat src;
	static inline Mat srcTemp;
	static inline HDC hwindowDC;
	static inline HDC hwindowCompatibleDC;
	static inline int screenx;
	static inline int screeny;
	static inline int width;
	static inline int height;
	static inline HBITMAP hbwindow;
	static inline BITMAPINFOHEADER bi;

	static inline int BITSPERPIXEL;
	static inline IDirect3D9* d3d;
	static inline D3DDISPLAYMODE mode;
	static inline D3DPRESENT_PARAMETERS parameters = { 0 };
	static inline IDirect3DDevice9* g_pd3dDevice;
	static inline IDirect3DSurface9* pSurface;
	static inline D3DLOCKED_RECT lockedRect;
	static inline HWND hwnd2;
	static inline Mat src2;
	static inline int width2;
	static inline int height2;

	//static inline HRESULT hr;
	//static inline UINT32 uiWidth;
	//static inline UINT32 uiHeight;
	//static inline IDirect3DDevice9* pDevice;
	//static inline IDirect3DSurface9* pSurface;
	//static inline IMFSinkWriter* pSinkWriter;
	//static inline DWORD stream;
	//static inline LONGLONG rtStart = 0;




public:
	struct constructor{
		constructor(){
			// get handles to a device context (DC)
			//hwnd = GetDesktopWindow();
			hwnd = GetActiveWindow();
			//hwindowDC = MonitorFromWindow(hwnd, MONITOR_DEFAULTTOPRIMARY);
			const POINT ptZero = { 0, 0 };
			HMONITOR hh = MonitorFromPoint(ptZero, MONITOR_DEFAULTTOPRIMARY);

			hwindowDC = GetDC(hwnd);
			hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
			SetStretchBltMode(hwindowCompatibleDC, COLORONCOLOR);

			screenx = 0;
			screeny = 0;
			width = GetSystemMetrics(SM_CXSCREEN);
			height = GetSystemMetrics(SM_CYSCREEN);

			src.create(height, width, CV_8UC3);
			srcTemp.create(height, width, CV_8UC3);

			hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
			bi = createBitmapHeader(width, height);
			cout << "fk oytu" << endl;


			// use the previously created device context with the bitmap
			SelectObject(hwindowCompatibleDC, hbwindow);
		}
	};

	struct constructor2 {
		constructor2() {
			BITSPERPIXEL = 24;
			hwnd2 = GetActiveWindow();
			d3d = Direct3DCreate9(D3D_SDK_VERSION);
			HRESULT hr = d3d->GetAdapterDisplayMode(D3DADAPTER_DEFAULT, &mode);
			if (FAILED(hr)) {
				cout << "something went wrong 1" << endl;
			}
			parameters.BackBufferWidth = 1;
			parameters.BackBufferHeight = mode.Height;
			parameters.BackBufferWidth = mode.Width;
			parameters.SwapEffect = D3DSWAPEFFECT_DISCARD;
			parameters.hDeviceWindow = NULL;
			parameters.Windowed = true;
			src2.create(mode.Height, mode.Width, CV_8UC3);
			width2 = mode.Width;
			height2 = mode.Height;
			hr = d3d->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, NULL, D3DCREATE_HARDWARE_VERTEXPROCESSING, &parameters, &g_pd3dDevice);
			if (FAILED(hr)) {
				cout << "something went wrong 2" << endl;
			}
			/*hr = g_pd3dDevice->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL);
			if (FAILED(hr)) {
				cout << "something went wrong 3" << endl;
			}*/
		
		}
	};

	static inline constructor2 cons2;

	static inline constructor cons;

	static  cuda::GpuMat TakeScreenShot();
	static cuda::GpuMat TakeScreenShotV2();

	static void Destroy();
};
