#pragma once
#include "Definitions.h"
#include <GL/freeglut.h>

#include "Interface.h"
#include "Statistics.h"
#include "HUD.h"
#include "CamField.h"

class Engine
{
public:
	Engine();
	~Engine();

	int InitGL();
	void Reshape(GLsizei width, GLsizei height);
	void Display();
	void Keyboard(unsigned char button, int x, int y, bool KeyDown);
	void MouseMotion(int x, int y);
	void MouseButton(int button, int state, int x, int y);
	void Special(int button, int x, int y, bool KeyDown);
	void Loop();

	void InitGame();

private:
	int width, height;

	bool	warp;
	bool	fullscreen;
	bool	bMousing;

	void DrawInterface();

	void OpenGL2d();
	void OpenGL3d();

	void GetFrameTime();
	double FrameInterval;
	friend class Statistics;
	Interface inter;
	Statistics stat;
	CamField *cam;
	HUD *hud;
};
