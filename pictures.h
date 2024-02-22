#pragma once
#include <windows.h>
#include <gdiplus.h>
#pragma comment(lib, "gdiplus.lib")


void StretchBltToFile(const char* inputImagePath, const char* outputImagePath, int newWidth, int newHeight);
LPCWSTR UTF8To16(const char* utf8String);

void StretchBltToFile(const char* inputImagePath, const char* outputImagePath, int newWidth, int newHeight)
{
    Gdiplus::GdiplusStartupInput gdiplusStartupInput;
    ULONG_PTR gdiplusToken;
    Gdiplus::GdiplusStartup(&gdiplusToken, &gdiplusStartupInput, NULL);

    Gdiplus::Bitmap sourceBitmap(UTF8To16(inputImagePath));
    Gdiplus::Bitmap stretchedBitmap(newWidth, newHeight);
    Gdiplus::Graphics graphics(&stretchedBitmap);

    graphics.DrawImage(&sourceBitmap, 0, 0, newWidth, newHeight);

    // 直接使用 Save 方法保存图像，无需指定编码器的 CLSID
    stretchedBitmap.Save(UTF8To16(outputImagePath), NULL, NULL);

    Gdiplus::GdiplusShutdown(gdiplusToken);
}

//
//LPCWSTR UTF8To16(const char* utf8String)
//{
//    int bufferSize = MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, nullptr, 0);
//    wchar_t* wideString = new wchar_t[bufferSize];
//
//    MultiByteToWideChar(CP_UTF8, 0, utf8String, -1, wideString, bufferSize);
//
//    return wideString;
//}