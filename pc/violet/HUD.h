#pragma once
#include "Interface.h"


class HUD
{
public:
	HUD(Interface *intrfc);
	~HUD(void);

	void Display(int width, int height, float Deg);

private:
	void DisplayPitch(int width, int height, float Deg);

	Interface *inter;
};

