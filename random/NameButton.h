﻿#pragma once
#include "Button.h"
class NameButton :
    public Button
{
	HBITMAP hBitmap = nullptr;
public:
	void paint() const;
    void setHBITMAP(HBITMAP newValue);
	bool IsOK() const;
};

