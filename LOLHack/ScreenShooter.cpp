#include "ScreenShooter.h"
BITMAPINFOHEADER ScreenShooter::createBitmapHeader(int width, int height) {
	BITMAPINFOHEADER  bi;

	// create a bitmap
	bi.biSize = sizeof(BITMAPINFOHEADER);
	bi.biWidth = width;
	bi.biHeight = -height;  //this is the line that makes it draw upside down or not
	bi.biPlanes = 1;
	bi.biBitCount = 24;
	bi.biCompression = BI_RGB;
	bi.biSizeImage = 0;
	bi.biXPelsPerMeter = 0;
	bi.biYPelsPerMeter = 0;
	bi.biClrUsed = 0;
	bi.biClrImportant = 0;

	return bi;
}

cuda::GpuMat ScreenShooter::TakeScreenShot() {
	//SelectObject(hwindowCompatibleDC, hbwindow);
	//StretchBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, width, height, SRCCOPY);  //change SRCCOPY to NOTSRCCOPY for wacky colors !
	BitBlt(hwindowCompatibleDC, 0, 0, width, height, hwindowDC, screenx, screeny, SRCCOPY);
	GetDIBits(hwindowCompatibleDC, hbwindow, 0, height, srcTemp.data, (BITMAPINFO*)&bi, DIB_RGB_COLORS);
	src.upload(srcTemp);
	return src;
}

void ScreenShooter::Destroy() {
	DeleteObject(hbwindow);
	DeleteDC(hwindowCompatibleDC);
	ReleaseDC(hwnd, hwindowDC);

	pSurface->UnlockRect();
	pSurface->Release();

	/*SafeRelease(&pSinkWriter);
	SafeRelease(&pDevice);
	SafeRelease(&pSurface);
	MFShutdown();
	CoUninitialize();*/
}

//HDC ScreenShooter::hwindowDC = GetDC(hwnd);
//HDC ScreenShooter::hwindowCompatibleDC = CreateCompatibleDC(hwindowDC);
//
//// define scale, height and width
//int ScreenShooter::screenx = GetSystemMetrics(SM_XVIRTUALSCREEN);
//int ScreenShooter::screeny = GetSystemMetrics(SM_YVIRTUALSCREEN);
//int ScreenShooter::width = GetSystemMetrics(SM_CXVIRTUALSCREEN);
//int ScreenShooter::height = GetSystemMetrics(SM_CYVIRTUALSCREEN);
//
//// create mat object
//
//// create a bitmap
//HBITMAP ScreenShooter::hbwindow = CreateCompatibleBitmap(hwindowDC, width, height);
//BITMAPINFOHEADER ScreenShooter::bi = createBitmapHeader(width, height);

//ScreenShooter::constructor ScreenShooter::cons;

cuda::GpuMat ScreenShooter::TakeScreenShotV2() {
	//void* pBits = NULL;
	g_pd3dDevice->CreateOffscreenPlainSurface(mode.Width, mode.Height, D3DFMT_A8R8G8B8, D3DPOOL_SCRATCH, &pSurface, NULL);
	g_pd3dDevice->GetFrontBufferData(0, pSurface);
	//LPDIRECT3DSURFACE9 back_buffer = NULL;
	//g_pd3dDevice->GetBackBuffer(0, 0, D3DBACKBUFFER_TYPE_MONO,  &pSurface);
	//D3DSURFACE_DESC desc;
	//back_buffer->GetDesc(&desc);
	//g_pd3dDevice->GetRenderTargetData(back_buffer, pSurface);
	/*for (int i = 0; i < height2; i++)
	{
		memcpy((BYTE*)pBits + i * width2 * BITSPERPIXEL / 8,(BYTE*)lockedRect.pBits + i * lockedRect.Pitch, width2 * BITSPERPIXEL / 8);
	}*/
	//g_pSurface->UnlockRect();

	//src2.data = (uchar*)pBits;
	//return src2;
	pSurface->LockRect(&lockedRect, NULL, D3DLOCK_READONLY);
	Mat D3DSurface(height2, width2, CV_8UC4, lockedRect.pBits, lockedRect.Pitch);
	//ZeroMemory(&lockedRect, sizeof(D3DLOCKED_RECT));
	//if (D3DSurfaceC.empty()) cout << "emty fuk" << endl;
	/*Mat D3DSurface(height2, width2, CV_8UC4);
	D3DSurfaceC.download(D3DSurface);*/
	cuda::GpuMat mgpu;
	mgpu.upload(D3DSurface);
	return mgpu;
}
