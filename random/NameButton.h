#pragma once
#include "Button.h"
#include "EasyGL/Bitmap.h"

class NameButton :
    public Button
{
	Bitmap hBitmap;
public:
	void paint() const;
    void setHBITMAP(Bitmap newValue);
	bool IsOK() const;
};

