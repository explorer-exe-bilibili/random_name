#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_TABLES_H
#include"mywindows.h"

FT_Face face_text,face_icon;
FT_Library library_text, library_icon;

void inittype();
void ShowText(wchar_t text, HDC hdcScreen, int x, int y, int size);
void relase_ferrtype();

void inittype() {
    FT_Init_FreeType(&library_text);
    FT_Init_FreeType(&library_icon);
    FT_New_Face(library_text, ".\\files\\ttfs\\text.ttf", 0, &face_text);
    FT_New_Face(library_icon, ".\\files\\ttfs\\icon.ttf", 0, &face_icon);
    FT_Select_Charmap(face_text, FT_ENCODING_UNICODE);
    log("Init FreeType Successfully");
}

void ShowText(wchar_t text, HDC hdcScreen,int x,int y,int size) {
    log("showtext begin");
    // 设置字符编码为Unicode
    FT_Select_Charmap(face_text, FT_ENCODING_UNICODE);
    // 获取字符的索引
    FT_UInt glyph_index = FT_Get_Char_Index(face_text, text);
    // 加载字形
    FT_Load_Glyph(face_text, glyph_index, FT_LOAD_RENDER);
    // 获取字形的位图
    FT_BitmapGlyph bitmap_glyph;
    FT_Glyph glyph;
    FT_Get_Glyph(face_text->glyph, &glyph);
    FT_Glyph_To_Bitmap(&glyph, ft_render_mode_normal, NULL, 1);
    bitmap_glyph = (FT_BitmapGlyph)glyph;
    // 创建位图
    BITMAPINFO bitmapInfo = { 0 };
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth =bitmap_glyph->bitmap.width;
    bitmapInfo.bmiHeader.biHeight = bitmap_glyph->bitmap.rows;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 1;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    bitmapInfo.bmiHeader.biSizeImage = (bitmap_glyph->bitmap.width + 7) / 8 * bitmap_glyph->bitmap.rows;
    HBITMAP hBitmap = CreateBitmap(bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, 1, 1,NULL);
    if (!hBitmap) {errlog("fail to create bitmap (showtext)");return;}
    HDC hdcBitmap = CreateCompatibleDC(hdcScreen);
    if (!hdcBitmap) {errlog("fail to create memory DC (showtext)");DeleteObject(hBitmap);return;}
    // 选择位图到内存设备上下文
    HBITMAP hOldBitmap = (HBITMAP)SelectObject(hdcBitmap, hBitmap);
    if (!hOldBitmap) {errlog("fail to select bitmap (showtext)");DeleteObject(hBitmap); DeleteDC(hdcBitmap);return; }    
    // 复制 FreeType 位图数据到 GDI 位图
    BYTE* pBits = (BYTE*)malloc(bitmapInfo.bmiHeader.biSizeImage);
    if (!pBits) {
        log("fail to allocate memory for bits (showtext)");
        SelectObject(hdcBitmap, hOldBitmap);
        DeleteObject(hBitmap);
        DeleteDC(hdcBitmap);
        return;
    }
    memcpy(pBits, bitmap_glyph->bitmap.buffer, bitmapInfo.bmiHeader.biSizeImage);

    // 设置位图信息的位数据指针
    bitmapInfo.bmiHeader.biClrImportant = 0;
    bitmapInfo.bmiHeader.biClrUsed = 0;
    SetBitmapBits(hBitmap, bitmapInfo.bmiHeader.biSizeImage, pBits);
    // 使用 StretchBlt 渲染位图
    StretchBlt(hdcScreen, x, y, size, size, hdcBitmap, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, SRCCOPY);
    //// 使用 StretchBlt 渲染位图
    //StretchBlt(hdcScreen, x, y, size, size, hdcBitmap, 0, 0, bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, SRCCOPY);
    //SelectObject(hdcBitmap, hBitmap);
    //StretchBlt(hdcBitmap, x, y,size,size,hdcScreen,0,0, bitmap.width, bitmap.rows,SRCCOPY);
    //UpdateWindow(hWnd);

    SelectObject(hdcBitmap, hOldBitmap);
    DeleteObject(hBitmap);
    DeleteDC(hdcBitmap);
    free(pBits);
    log("(FreeType)ShowText,in%d,%d",x, y);
}
void relase_ferrtype() {
    // 清理资源
    FT_Done_Face(face_text);
    FT_Done_Face(face_icon);
    FT_Done_FreeType(library_text);
    FT_Done_FreeType(library_icon);
    log("Relase FreeType successfully");
}