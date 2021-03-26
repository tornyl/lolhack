#pragma once
#include <iostream>
#include <winsock2.h>
#include <windows.h>
#include <wininet.h>
#include <process.h>
#include <commctrl.h>
#include <stdio.h>
#include <mutex>
#include <queue>
#include <sstream>
#include <map>
#include <thread>
#include <string>
#include <vector>
#include <tchar.h>
#include <memory>
#include <functional>
#include <atlbase.h>
#include <codecapi.h>
#include <mfidl.h>
#include <mfapi.h>
#include <mfreadwrite.h>
#include <dshow.h>
#include <effects.h>
#include <Wmcodecdsp.h>
#include <setjmp.h>
#include <Mferror.h>
#include <optional>
#include <d2d1.h>
#include <d2d1_1.h>
#include <d2d1_2.h>
#include <d2d1_3.h>
#include <dxgi.h>
#include <wincodec.h>
#include <dxgi1_2.h>
#include <d3d11.h>
#include <shlobj.h>
#include <propvarutil.h>
#include <wrl/client.h>
#include <wrl.h>
#include <comdef.h>
#include <system_error>
//#include <opencv2/opencv.hpp>
#include <opencv2/core/cuda.hpp>

#pragma comment(lib,"ws2_32.lib")
#pragma comment(lib,"wininet.lib")
#pragma comment(lib,"d3d11.lib")
#pragma comment(lib,"bcrypt.lib")
#pragma comment(lib,"crypt32.lib")
#pragma comment(lib,"Comctl32.lib")
#pragma comment(lib,"Msacm32.lib")
#pragma comment(lib,"Mfplat.lib")
#pragma comment(lib,"mfreadwrite.lib")
#pragma comment(lib,"mfuuid.lib")
#pragma comment(lib, "strmiids.lib")
#pragma comment(lib, "windowscodecs.lib")
#pragma comment(lib, "propsys.lib")
#pragma comment(lib, "wmcodecdspuuid.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "d2d1.lib")
#pragma comment(lib, "gdiplus.lib")
#pragma comment(lib, "Mf.lib")
#pragma comment(lib, "dxgi.lib")

using namespace Microsoft::WRL;\

std::string format_error(unsigned __int32 hr)
{
    std::stringstream ss;
    ss << "Failed to Initialize COM. Error code = 0x" << std::hex << hr << std::endl;
    return ss.str();
}

void GetAdapters(std::vector<IDXGIAdapter*>& a)
{
    IDXGIFactory* df;
    CreateDXGIFactory1(__uuidof(IDXGIFactory), (void**)&df);
    a.clear();
    if (!df)
        return;
    int L = 0;
    for (;;)
    {
        IDXGIAdapter* lDxgiAdapter;
        DXGI_ADAPTER_DESC dscAdapter;
        df->EnumAdapters(L, &lDxgiAdapter);
        if (!lDxgiAdapter)
            break;
        L++;
        lDxgiAdapter->GetDesc(&dscAdapter);
        std::wcout << dscAdapter.Description << std::endl;
        a.push_back(lDxgiAdapter);
    }
}

class ScreenCapture {
public:
    D3D_FEATURE_LEVEL lFeatureLevel;

    UINT gNumFeatureLevels;

    //UINT createDeviceFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;
    UINT createDeviceFlags = D3D11_CREATE_DEVICE_VIDEO_SUPPORT;
    ComPtr<ID3D11Device> _lDevice;
    ComPtr<ID3D11DeviceContext> _lImmediateContext;
    DXGI_OUTPUT_DESC _lOutputDesc;
    ComPtr<IDXGIOutputDuplication> _lDeskDupl;
    DXGI_OUTDUPL_DESC _lOutputDuplDesc;
    D3D11_TEXTURE2D_DESC _desc;
    D3D11_MAPPED_SUBRESOURCE _resource;
    cv::Mat _result;

    std::vector<IDXGIAdapter*> adapters;
    IDXGIAdapter* realAdapter;

    ScreenCapture() {
    }



    bool Init() {
        int lresult(-1);
 /*       gFeatureLevels[] = {
            D3D_FEATURE_LEVEL_11_0,
                D3D_FEATURE_LEVEL_10_1,
                D3D_FEATURE_LEVEL_10_0,
                D3D_FEATURE_LEVEL_9_3,
                D3D_FEATURE_LEVEL_9_2,
                D3D_FEATURE_LEVEL_9_1,
        };*/
        D3D_FEATURE_LEVEL gFeatureLevels[] = {
        D3D_FEATURE_LEVEL_11_0,
        D3D_FEATURE_LEVEL_10_1,
        D3D_FEATURE_LEVEL_10_0,
        D3D_FEATURE_LEVEL_9_3,
        D3D_FEATURE_LEVEL_9_2,
        D3D_FEATURE_LEVEL_9_1,
        };
        gNumFeatureLevels = ARRAYSIZE(gFeatureLevels);
      
        GetAdapters(adapters);
        //std::cout << adapters.size() << std::endl;
        realAdapter = adapters[0];


        //ID3D11Device _lDevice;

        HRESULT hr(E_FAIL);
        hr = D3D11CreateDevice(
            realAdapter,
            D3D_DRIVER_TYPE_UNKNOWN,
            nullptr,
            createDeviceFlags,
            gFeatureLevels,
            gNumFeatureLevels,
            D3D11_SDK_VERSION,
            &_lDevice,
            &lFeatureLevel,
            &_lImmediateContext);


        if (FAILED(hr)) {
            return false;
        }


        if (!_lDevice)
            return false;

        // Get DXGI device
        ComPtr<IDXGIDevice> lDxgiDevice;
        hr = _lDevice.As(&lDxgiDevice);

        if (FAILED(hr))
            return false;



        // Get DXGI adapter
        ComPtr<IDXGIAdapter> lDxgiAdapter;
        hr = lDxgiDevice->GetParent(__uuidof(IDXGIAdapter), &lDxgiAdapter);

        if (FAILED(hr))
            return false;

        //std::cout << "erroris" << std::endl;
        //DXGI_ADAPTER_DESC dscAdapter;
        //lDxgiAdapter->GetDesc(&dscAdapter);
        //std::wcout << dscAdapter.Description << std::endl;
        //if (lDxgiAdapter == nullptr) std::cout << "dog" << std::endl;

        lDxgiDevice.Reset();

        UINT Output=0;

        // Get output
        ComPtr<IDXGIOutput> lDxgiOutput;
        hr = lDxgiAdapter->EnumOutputs(Output, &lDxgiOutput);
        /*for (UINT i = 0; lDxgiAdapter->EnumOutputs(i, &lDxgiOutput) != DXGI_ERROR_NOT_FOUND; ++i)
        {
            std::cout << "wuhaa" << std::endl;
        }*/


        if (FAILED(hr)) {
            std::cout << format_error(hr) << std::endl;
            return false;
        }



        lDxgiAdapter.Reset();

        hr = lDxgiOutput->GetDesc(&_lOutputDesc);

        if (FAILED(hr))
            return false;


        // QI for Output 1
        ComPtr<IDXGIOutput1> lDxgiOutput1;
        hr = lDxgiOutput.As(&lDxgiOutput1);


        if (FAILED(hr))
            return false;





        lDxgiOutput.Reset();
        if (_lDevice == nullptr) std::cout << "nullptr" << std::endl;
        if (_lDeskDupl == nullptr) std::cout << "nullptr222" << std::endl;

        // Create desktop duplication
        hr = lDxgiOutput1->DuplicateOutput(_lDevice.Get(), &_lDeskDupl);

        if (FAILED(hr)) {
            //_com_error err(hr);
            std::cout << format_error(hr) << std::endl;
            return false;
        }

        std::cout << "zumba" << std::endl;


        lDxgiOutput1.Reset();

        // Create GUI drawing texture
        _lDeskDupl->GetDesc(&_lOutputDuplDesc);
        // Create CPU access texture
        _desc.Width = _lOutputDuplDesc.ModeDesc.Width;
        _desc.Height = _lOutputDuplDesc.ModeDesc.Height;
        _desc.Format = _lOutputDuplDesc.ModeDesc.Format;
        std::cout << _desc.Width << "x" << _desc.Height << "\n\n\n";

        _desc.ArraySize = 1;
        _desc.BindFlags = 0;
        _desc.MiscFlags = 0;
        _desc.SampleDesc.Count = 1;
        _desc.SampleDesc.Quality = 0;
        _desc.MipLevels = 1;
        _desc.CPUAccessFlags = D3D11_CPU_ACCESS_FLAG::D3D11_CPU_ACCESS_READ;
        _desc.Usage = D3D11_USAGE::D3D11_USAGE_STAGING;

        while (!CaptureScreen(_result));
        _result = cv::Mat(_desc.Height, _desc.Width, CV_8UC4, _resource.pData);
        return true;
    }

    bool CaptureScreen(cv::Mat& output)
    {
        HRESULT hr(E_FAIL);
        ComPtr<IDXGIResource> lDesktopResource = nullptr;
        DXGI_OUTDUPL_FRAME_INFO lFrameInfo;
        ID3D11Texture2D* currTexture = NULL;

        hr = _lDeskDupl->AcquireNextFrame(999, &lFrameInfo, &lDesktopResource);

        if (FAILED(hr)) {
            std::cout << "error 1" << std::endl;
            std::cout << format_error(hr) << std::endl;
            return false;
        }

        if (lFrameInfo.LastPresentTime.HighPart == 0) // not interested in just mouse updates, which can happen much faster than 60fps if you really shake the mouse
        {
            hr = _lDeskDupl->ReleaseFrame();
            std::cout << "error 2" << std::endl;
            std::cout << format_error(hr) << std::endl;
            return false;
        }

        //int accum_frames = lFrameInfo.AccumulatedFrames;
        //if (accum_frames > 1) {// && current_frame != 1) {
        //                     // TOO MANY OF THESE is the problem
        //                     // especially after having to wait >17ms in AcquireNextFrame()
        //}

        // QI for ID3D11Texture2D
        ComPtr<ID3D11Texture2D> _lAcquiredDesktopImage;
        //hr = lDesktopResource.As(&_lAcquiredDesktopImage);
        hr = lDesktopResource->QueryInterface(IID_PPV_ARGS(&_lAcquiredDesktopImage));
        // Copy image into a newly created CPU access texture
        hr = _lDevice->CreateTexture2D(&_desc, NULL, &currTexture);
        if (FAILED(hr))
        {
            hr = _lDeskDupl->ReleaseFrame();
            std::cout << "error 3" << std::endl;
            return false;
        }
        if (!currTexture)
        {
            hr = _lDeskDupl->ReleaseFrame();
            std::cout << "error 4" << std::endl;
            return false;
        }

        _lImmediateContext->CopyResource(currTexture, _lAcquiredDesktopImage.Get());
        UINT subresource = D3D11CalcSubresource(0, 0, 0);
        _lImmediateContext->Map(currTexture, subresource, D3D11_MAP_READ, 0, &_resource);
        //_lImmediateContext->Map(_lAcquiredDesktopImage.Get(), subresource, D3D11_MAP_READ, 0, &_resource);
        _lImmediateContext->Unmap(currTexture, 0);
        currTexture->Release();
        hr = _lDeskDupl->ReleaseFrame();

        //output = _result;
        //output.data = (uchar*)_resource.pData;
        output = cv::Mat(_desc.Height, _desc.Width, CV_8UC4, _resource.pData);
        //std::cout << "finish" << std::endl;
        return true;
    }
};

//ID3D11Texture2D* pSurface;
//HRESULT hr = m_swapChain->GetBuffer(0, __uuidof(ID3D11Texture2D), reinterpret_cast<void**>(&pSurface));
//if (pSurface)
//{
//    const int width = static_cast<int>(m_window->Bounds.Width * m_dpi / 96.0f);
//    const int height = static_cast<int>(m_window->Bounds.Height * m_dpi / 96.0f);
//    unsigned int size = width * height;
//    if (m_captureData)
//    {
//        freeFramebufferData(m_captureData);
//    }
//    m_captureData = new unsigned char[width * height * 4];
//
//    ID3D11Texture2D* pNewTexture = NULL;
//
//    D3D11_TEXTURE2D_DESC description;
//    pSurface->GetDesc(&description);
//    description.BindFlags = 0;
//    description.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
//    description.Usage = D3D11_USAGE_STAGING;
//
//    HRESULT hr = m_d3dDevice->CreateTexture2D(&description, NULL, &pNewTexture);
//    if (pNewTexture)
//    {
//        m_d3dContext->CopyResource(pNewTexture, pSurface);
//        D3D11_MAPPED_SUBRESOURCE resource;
//        unsigned int subresource = D3D11CalcSubresource(0, 0, 0);
//        HRESULT hr = m_d3dContext->Map(pNewTexture, subresource, D3D11_MAP_READ_WRITE, 0, &resource);
//        //resource.pData; // TEXTURE DATA IS HERE
//
//        const int pitch = width << 2;
//        const unsigned char* source = static_cast<const unsigned char*>(resource.pData);
//        unsigned char* dest = m_captureData;
//        for (int i = 0; i < height; ++i)
//        {
//            memcpy(dest, source, width * 4);
//            source += resource.RowPitch; // <------
//            dest += pitch;
//        }
//
//        m_captureSize = size;
//        m_captureWidth = width;
//        m_captureHeight = height;
//
//        return;
//    }
//
//    freeFramebufferData(m_captureData);
//}
//static void GetAdapters(std::vector<CComPtr<IDXGIAdapter1>>& a)
//{
//    CComPtr<IDXGIFactory1> df;
//    CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)&df);
//    a.clear();
//    if (!df)
//        return;
//    int L = 0;
//    for (;;)
//    {
//        CComPtr<IDXGIAdapter1> lDxgiAdapter;
//        df->EnumAdapters1(L, &lDxgiAdapter);
//        if (!lDxgiAdapter)
//            break;
//        L++;
//        a.push_back(lDxgiAdapter);
//    }
//}
//
//HRESULT CreateDirect3DDevice(IDXGIAdapter1* g)
//{
//    HRESULT hr = S_OK;
//
//    // Driver types supported
//    D3D_DRIVER_TYPE DriverTypes[] =
//    {
//        D3D_DRIVER_TYPE_HARDWARE,
//        D3D_DRIVER_TYPE_WARP,
//        D3D_DRIVER_TYPE_REFERENCE,
//    };
//    UINT NumDriverTypes = ARRAYSIZE(DriverTypes);
//
//    // Feature levels supported
//    D3D_FEATURE_LEVEL FeatureLevels[] =
//    {
//        D3D_FEATURE_LEVEL_11_0,
//        D3D_FEATURE_LEVEL_10_1,
//        D3D_FEATURE_LEVEL_10_0,
//        D3D_FEATURE_LEVEL_9_3,
//        D3D_FEATURE_LEVEL_9_2,
//        D3D_FEATURE_LEVEL_9_1
//    };
//    UINT NumFeatureLevels = ARRAYSIZE(FeatureLevels);
//
//    D3D_FEATURE_LEVEL FeatureLevel;
//
//    // Create device
//    for (UINT DriverTypeIndex = 0; DriverTypeIndex < NumDriverTypes; ++DriverTypeIndex)
//    {
//        hr = D3D11CreateDevice(g, DriverTypes[DriverTypeIndex],
//            nullptr, D3D11_CREATE_DEVICE_VIDEO_SUPPORT, FeatureLevels, NumFeatureLevels,
//            D3D11_SDK_VERSION, &device, &FeatureLevel, &context);
//        if (SUCCEEDED(hr))
//        {
//            // Device creation success, no need to loop anymore
//            break;
//        }
//    }
//    if (FAILED(hr))
//        return hr;
//
//    return S_OK;
//}
//
//bool Prepare(UINT Output = 0)
//{
//    // Get DXGI device
//    CComPtr<IDXGIDevice> lDxgiDevice;
//    lDxgiDevice = device;
//    if (!lDxgiDevice)
//        return 0;
//
//    // Get DXGI adapter
//    CComPtr<IDXGIAdapter> lDxgiAdapter;
//    auto hr = lDxgiDevice->GetParent(
//        __uuidof(IDXGIAdapter),
//        reinterpret_cast<void**>(&lDxgiAdapter));
//
//    if (FAILED(hr))
//        return 0;
//
//    lDxgiDevice = 0;
//
//    // Get output
//    CComPtr<IDXGIOutput> lDxgiOutput;
//    hr = lDxgiAdapter->EnumOutputs(Output, &lDxgiOutput);
//    if (FAILED(hr))
//        return 0;
//
//    lDxgiAdapter = 0;
//
//    DXGI_OUTPUT_DESC lOutputDesc;
//    hr = lDxgiOutput->GetDesc(&lOutputDesc);
//
//    // QI for Output 1
//    CComPtr<IDXGIOutput1> lDxgiOutput1;
//    lDxgiOutput1 = lDxgiOutput;
//    if (!lDxgiOutput1)
//        return 0;
//
//    lDxgiOutput = 0;
//
//    // Create desktop duplication
//    hr = lDxgiOutput1->DuplicateOutput(
//        device,
//        &lDeskDupl);
//
//    if (FAILED(hr))
//        return 0;
//
//    lDxgiOutput1 = 0;
//
//    // Create GUI drawing texture
//    lDeskDupl->GetDesc(&lOutputDuplDesc);
//    D3D11_TEXTURE2D_DESC desc = {};
//    desc.Width = lOutputDuplDesc.ModeDesc.Width;
//    desc.Height = lOutputDuplDesc.ModeDesc.Height;
//    desc.Format = lOutputDuplDesc.ModeDesc.Format;
//    desc.ArraySize = 1;
//    desc.BindFlags = D3D11_BIND_FLAG::D3D11_BIND_RENDER_TARGET;
//    desc.MiscFlags = D3D11_RESOURCE_MISC_GDI_COMPATIBLE;
//    desc.SampleDesc.Count = 1;
//    desc.SampleDesc.Quality = 0;
//    desc.MipLevels = 1;
//    desc.CPUAccessFlags = 0;
//    desc.Usage = D3D11_USAGE_DEFAULT;
//    hr = device->CreateTexture2D(&desc, NULL, &lGDIImage);
//    if (FAILED(hr))
//        return 0;
//
//    if (lGDIImage == nullptr)
//        return 0;
//
//    // Create CPU access texture
//    desc.Width = lOutputDuplDesc.ModeDesc.Width;
//    desc.Height = lOutputDuplDesc.ModeDesc.Height;
//    desc.Format = lOutputDuplDesc.ModeDesc.Format;
//    desc.ArraySize = 1;
//    desc.BindFlags = 0;
//    desc.MiscFlags = 0;
//    desc.SampleDesc.Count = 1;
//    desc.SampleDesc.Quality = 0;
//    desc.MipLevels = 1;
//    desc.CPUAccessFlags = D3D11_CPU_ACCESS_READ | D3D11_CPU_ACCESS_WRITE;
//    desc.Usage = D3D11_USAGE_STAGING;
//    hr = device->CreateTexture2D(&desc, NULL, &lDestImage);
//    if (FAILED(hr))
//        return 0;
//
//    if (lDestImage == nullptr)
//        return 0;
//
//    return 1;
//}
//
//bool Get(IDXGIResource* lDesktopResource, bool Curs, RECT* rcx = 0)
//{
//    // QI for ID3D11Texture2D
//    CComPtr<ID3D11Texture2D> lAcquiredDesktopImage;
//    if (!lDesktopResource)
//        return 0;
//    auto hr = lDesktopResource->QueryInterface(IID_PPV_ARGS(&lAcquiredDesktopImage));
//    if (!lAcquiredDesktopImage)
//        return 0;
//    lDesktopResource = 0;
//
//    // Copy image into GDI drawing texture
//    context->CopyResource(lGDIImage, lAcquiredDesktopImage);
//
//    // Draw cursor image into GDI drawing texture
//    CComPtr<IDXGISurface1> lIDXGISurface1;
//
//    lIDXGISurface1 = lGDIImage;
//
//    if (!lIDXGISurface1)
//        return 0;
//
//    CURSORINFO lCursorInfo = { 0 };
//    lCursorInfo.cbSize = sizeof(lCursorInfo);
//    auto lBoolres = GetCursorInfo(&lCursorInfo);
//    if (lBoolres == TRUE)
//    {
//        if (lCursorInfo.flags == CURSOR_SHOWING && Curs)
//        {
//            auto lCursorPosition = lCursorInfo.ptScreenPos;
//            //                auto lCursorSize = lCursorInfo.cbSize;
//            HDC  lHDC;
//            lIDXGISurface1->GetDC(FALSE, &lHDC);
//            DrawIconEx(
//                lHDC,
//                lCursorPosition.x,
//                lCursorPosition.y,
//                lCursorInfo.hCursor,
//                0,
//                0,
//                0,
//                0,
//                DI_NORMAL | DI_DEFAULTSIZE);
//            lIDXGISurface1->ReleaseDC(nullptr);
//        }
//    }
//
//    // Copy image into CPU access texture
//    context->CopyResource(lDestImage, lGDIImage);
//
//    // Copy from CPU access texture to bitmap buffer
//    D3D11_MAPPED_SUBRESOURCE resource;
//    UINT subresource = D3D11CalcSubresource(0, 0, 0);
//    hr = context->Map(lDestImage, subresource, D3D11_MAP_READ_WRITE, 0, &resource);
//    if (FAILED(hr))
//        return 0;
//
//    auto sz = lOutputDuplDesc.ModeDesc.Width
//        * lOutputDuplDesc.ModeDesc.Height * 4;
//    auto sz2 = sz;
//    buf.resize(sz);
//    if (rcx)
//    {
//        sz2 = (rcx->right - rcx->left) * (rcx->bottom - rcx->top) * 4;
//        buf.resize(sz2);
//        sz = sz2;
//    }
//
//    UINT lBmpRowPitch = lOutputDuplDesc.ModeDesc.Width * 4;
//    if (rcx)
//        lBmpRowPitch = (rcx->right - rcx->left) * 4;
//    UINT lRowPitch = std::min<UINT>(lBmpRowPitch, resource.RowPitch);
//
//    BYTE* sptr = reinterpret_cast<BYTE*>(resource.pData);
//    BYTE* dptr = buf.data() + sz - lBmpRowPitch;
//    if (rcx)
//        sptr += rcx->left * 4;
//    for (size_t h = 0; h < lOutputDuplDesc.ModeDesc.Height; ++h)
//    {
//        if (rcx && h < (size_t)rcx->top)
//        {
//            sptr += resource.RowPitch;
//            continue;
//        }
//        if (rcx && h >= (size_t)rcx->bottom)
//            break;
//        memcpy_s(dptr, lBmpRowPitch, sptr, lRowPitch);
//        sptr += resource.RowPitch;
//        dptr -= lBmpRowPitch;
//    }
//    context->Unmap(lDestImage, subresource);
//    return 1;
//}