#pragma once
#include "Button.h"
class NameButton :
    public Button
{
	HBITMAP hBitmap = nullptr;
public:
	void paint();
    void setHBITMAP(HBITMAP newValue);
	bool IsOK() const;
};

