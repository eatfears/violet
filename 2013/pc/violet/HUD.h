#pragma once
#include "Interface.h"


class HUD
{
public:
	HUD(Interface *intrfc);
	~HUD(void);

	void Display(int width, int height, float Deg);

private:
	void DisplayPitch();
	void DisplayYaw();
	void DisplayCompass();

	int width;
	int height;
	float pixPerDeg;
	void _circle(float r, float s);
	
	Interface *inter;
};

