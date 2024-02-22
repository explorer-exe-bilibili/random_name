#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include FT_GLYPH_H
#include FT_TRUETYPE_TABLES_H
#include"mywindows.h"

FT_Face face_text,face_icon;
FT_Library library_text, library_icon;

void inittype() {
    FT_Init_FreeType(&library_text);
    FT_Init_FreeType(&library_icon);
    FT_New_Face(library_text, ".\\files\\ttfs\\text.ttf", 0, &face_text);
    FT_New_Face(library_icon, ".\\files\\ttfs\\icon.ttf", 0, &face_icon);
    FT_Select_Charmap(face_text, FT_ENCODING_UNICODE);
}

void ShowText(wchar_t text, HDC hdcScreen,int x,int y,int size) {
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
    bitmap_glyph = (FT_BitmapGlyph)face_text->glyph;

    // 获取字形的位图数据
    FT_Bitmap bitmap = bitmap_glyph->bitmap;

    // 创建位图
    BITMAPINFO bitmapInfo = { 0 };
    bitmapInfo.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    bitmapInfo.bmiHeader.biWidth = face_text->glyph->bitmap.width;
    bitmapInfo.bmiHeader.biHeight = face_text->glyph->bitmap.rows;
    bitmapInfo.bmiHeader.biPlanes = 1;
    bitmapInfo.bmiHeader.biBitCount = 1;
    bitmapInfo.bmiHeader.biCompression = BI_RGB;
    HBITMAP hBitmap = CreateBitmap(bitmapInfo.bmiHeader.biWidth, bitmapInfo.bmiHeader.biHeight, 1, 1, NULL);
    HDC hdcBitmap = CreateCompatibleDC(hdcScreen);
    SelectObject(hdcBitmap, hBitmap);
    BitBlt(hdcBitmap, x, y, bitmap.width, bitmap.rows, hdcScreen, 0, 0, SRCCOPY);
    UpdateWindow(hWnd);

    DeleteObject(hBitmap);
    DeleteDC(hdcBitmap);
}
void relase_ferrtype() {
    // 清理资源
    FT_Done_Face(face_text);
    FT_Done_Face(face_icon);
    FT_Done_FreeType(library_text);
    FT_Done_FreeType(library_icon);
}