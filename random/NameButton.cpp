#include "NameButton.h"

#include "configitem.h"
#include "Gp.h"
#include "mywindows.h"

void NameButton::paint() const
{
	static bool debug = config::getint(DEBUG);
	if (Disable)return;
	if ((xE < 0 AND x < 0)
		OR(yE < 0 AND y < 0)
		OR(x > mywindows::WW AND xE > mywindows::WW)
		OR(y > mywindows::WH AND yE > mywindows::WH))
		return;
	if (!p)return;
	p->Paint(x, y, xE - x, yE - y, hBitmap);
	if (debug)
	{
		p->DrawSquare(x, y, xE, yE, ARGB(255,255,0,0), false);
	}
}

void NameButton::setHBITMAP(HBITMAP newValue)
{
	if (hBitmap)DeleteObject(hBitmap);
	hBitmap = newValue;
}

bool NameButton::IsOK() const
{
	if (hBitmap)return true;
	return false;
}
