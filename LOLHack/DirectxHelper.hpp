//#pragma once
//#include <d3d11.h>
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
//            source += pitch;
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