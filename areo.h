#pragma once
#include <Windows.h>
#include <d2d1.h>
#include <d2d1_1.h>
#include <vector>

// 链接Direct2D库
#pragma comment(lib, "d2d1.lib")

// 全局变量
ID2D1Factory1* pFactory = nullptr;
ID2D1Device* pDevice = nullptr;
ID2D1DeviceContext* pContext = nullptr;

// 截取窗口并应用高斯模糊
void CaptureAndBlurWindow(HWND hwnd) {
    // 初始化Direct2D
    D2D1_FACTORY_OPTIONS options;
    options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
    D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, &pFactory);
    pFactory->CreateDevice(&pDevice);
    pDevice->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &pContext);

    // 截取窗口
    HBITMAP hBitmap = NULL;
    if (PrintWindow(hwnd, hBitmap, NULL)) {
        // 从HBITMAP创建Direct2D位图
        ID2D1Bitmap* pD2DBitmap = nullptr;
        pContext->CreateBitmapFromHbitmap(hBitmap, &D2D1::SizeU(0, 0), nullptr, &pD2DBitmap);

        // 创建高斯模糊效果
        ID2D1Effect* pBlurEffect = nullptr;
        pContext->CreateEffect(CLSID_D2D1GaussianBlur, &pBlurEffect);
        pBlurEffect->SetInput(0, pD2DBitmap);
        pBlurEffect->SetValue(D2D1_GAUSSIANBLUR_PROP_STANDARD_DEVIATION, 3.0f);

        // 绘制模糊效果
        pContext->BeginDraw();
        pContext->DrawImage(pBlurEffect);
        pContext->EndDraw();

        // 释放资源
        pD2DBitmap->Release();
        pBlurEffect->Release();
        DeleteObject(hBitmap);
    }
}

// 清理Direct2D资源
void CleanupDirect2D() {
    if (pContext) pContext->Release();
    if (pDevice) pDevice->Release();
    if (pFactory) pFactory->Release();
}