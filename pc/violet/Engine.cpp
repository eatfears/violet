#include "Engine.h"
#include <ctime>
#define _USE_MATH_DEFINES
#include <math.h>
#include "PlatformDefinitions.h"

GLfloat DayFogColor[4] = {0.50f, 0.67f, 1.00f, 1.00f};
GLfloat NightFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};
GLfloat WaterFogColor[4] = {0.00f, 0.00f, 0.00f, 1.00f};

Engine::Engine()
	:stat(*this)
{
	bMousing = false;
	fullscreen = false;

	width = 0;
	height = 0;
	FrameInterval = 0.0;
}

Engine::~Engine()
{
}

int Engine::InitGL()
{
//	wWorld.MaterialLib.AllocGLTextures();
	//wWorld.MaterialLib.LoadGLTextures();

//	wWorld.player.dPositionY = 100*BLOCK_SIZE+00.0;
//	wWorld.player.dSpinY = -90 - 45;
	glutSetCursor(GLUT_CURSOR_NONE);							//Выставляем на НЕТ КУРСОР


	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0f);											// Разрешить очистку буфера глубины

	glEnable(GL_DEPTH_TEST);									// Разрешить тест глубины

	glDepthFunc(GL_LEQUAL);										// Тип теста глубины
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// Улучшение в вычислении перспективы

	//рассчет текстур
	glEnable(GL_TEXTURE_2D);

	//автоматическое приведение нормалей к единичной длине
	glEnable(GL_NORMALIZE);

	return true;
}

void Engine::Reshape(int width, int height)
{
	if(height == 0)											// Предотвращение деления на ноль
		height = 1;

	this->width = width;
	this->height = height;

	OpenGL3d();
	//glutPostRedisplay();
}

void Engine::Display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// Очистить экран и буфер глубины
	glLoadIdentity();											// Сбросить текущую матрицу
	OpenGL3d();

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);


	//Set position
	//glRotated(-player.dSpinX, 1.0, 0.0, 0.0);
	//glRotated(-player.dSpinY, 0.0, 1.0, 0.0);
	//glTranslated(-player.dPositionX, -player.dPositionY, -player.dPositionZ);


	glEnable(GL_FOG);
	glFogi(GL_FOG_MODE,  GL_LINEAR);
	glHint(GL_FOG_HINT, GL_DONT_CARE);
	/*
	if(player.UnderWater)
	{
		glFogfv(GL_FOG_COLOR, WaterFogColor);
		glFogf(GL_FOG_DENSITY, 20.0);
		glFogf(GL_FOG_START, BLOCK_SIZE*10);
		glFogf(GL_FOG_END, BLOCK_SIZE*30);
	}*/

	glColor3d(1.0, 1.0, 1.0);

//	glBindTexture(GL_TEXTURE_2D, tex[TERRAIN]);
	int render;

	/*if(player.bKeyboard['Z'])
	{
		player.bKeyboard['Z'] = 0;
		wWorld.SoftLight = !wWorld.SoftLight;
		wWorld.LightToRefresh = true;
	}
	*/
	stat.reRenderedChunks = 0;


	glEnable(GL_ALPHA_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glDisable(GL_ALPHA_TEST);
	glDisable(GL_BLEND);

}

void Engine::Keyboard(unsigned char button, int x, int y, bool KeyDown)
{
	switch(button)
	{
	case KEY_ESCAPE: glutExit();
		break;
	default:
#ifdef _WIN32
		button = VkKeyScan(button);
#endif
		//wWorld.player.bKeyboard[button] = KeyDown;
		break;
	}
}

void Engine::MouseMotion(int x, int y)
{
	if(bMousing)
	{
/*		Character &player = wWorld.player;
		bMousing = false;

		player.dSpinY -= (x - width/2)/MOUSE_SENSIVITY;
		player.dSpinX -= (y - height/2)/MOUSE_SENSIVITY;

		while(player.dSpinY >= 360.0)
			player.dSpinY -= 360.0;

		while(player.dSpinY < 0.0)
			player.dSpinY += 360.0;

		if(player.dSpinX < -90.0) player.dSpinX = -90.0;
		if(player.dSpinX > 90.0) player.dSpinX = 90.0;

		glutWarpPointer(width/2,height/2);
*/	}
	else
		bMousing = true;
}

void Engine::MouseButton(int button, int state, int x, int y)
{
// 	switch(button)
// 	{
// 	case GLUT_LEFT_BUTTON:
// 		if (state==GLUT_DOWN) glutIdleFunc(spinDisplay); break;
//
// 	case GLUT_RIGHT_BUTTON:
// 		if (state==GLUT_DOWN) glutIdleFunc(NULL); break;
// 	}
}

void Engine::InitGame()
{
#ifdef _WIN32
	SetThreadPriority(GetCurrentThread(), THREAD_PRIORITY_ABOVE_NORMAL);

	HWND console = GetConsoleWindow();
	ShowWindow(console, SW_HIDE);
#endif // _WIN32

	unsigned int seed = std::time(0);
}

void Engine::DrawInterface()
{
	//glPushMatrix();
	//glPopMatrix();
	int WidthBy2  = width/2;
	int HeightBy2 = height/2;

	//glLoadIdentity();
	OpenGL2d();

/*	{
		GLfloat Brightness;
		Light::GetLight(*player.chunk, player.index, Brightness);

		GLdouble TextureRotation = player.dSpinY/90;

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		glBindTexture(GL_TEXTURE_2D, wWorld.MaterialLib.texture[UNDERWATER]);
		glColor4f(Brightness, Brightness, Brightness, 0.9f);

		glBegin(GL_QUADS);
		glTexCoord2d(0.0 - TextureRotation, 0.0);
		glVertex2i(WidthBy2- 3*HeightBy2, 0);
		glTexCoord2d(0.0 - TextureRotation, 1.0);
		glVertex2i(WidthBy2 - 3*HeightBy2, height);
		glTexCoord2d(3.0 - TextureRotation, 1.0);
		glVertex2i(WidthBy2 + 3*HeightBy2, height);
		glTexCoord2d(3.0 - TextureRotation, 0.0);
		glVertex2i(WidthBy2 + 3*HeightBy2, 0);
		glEnd();

		glDisable(GL_BLEND);
	}
	*/
	//Vignette
	glEnable(GL_BLEND);
	glBlendFunc(GL_ZERO, GL_ONE_MINUS_SRC_COLOR);

//	glBindTexture(GL_TEXTURE_2D, wWorld.MaterialLib.texture[VIGNETTE]);

	glColor3f(0.4f, 0.4f, 0.4f);

	glBegin(GL_QUADS);
	glTexCoord2d(0.0, 0.0);
	glVertex2i(0, 0);
	glTexCoord2d(0.0, 1.0);
	glVertex2i(0, height);
	glTexCoord2d(1.0, 1.0);
	glVertex2i(width, height);
	glTexCoord2d(1.0, 0.0);
	glVertex2i(width, 0);
	glEnd();

	glDisable(GL_BLEND);

	//Cross
	glEnable(GL_BLEND);
	glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

	glBindTexture(GL_TEXTURE_2D, 0);
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth (2.0);

	glBegin(GL_LINES);
	glVertex2i(WidthBy2, -9 + HeightBy2);
	glVertex2i(WidthBy2,  9 + HeightBy2);

	glVertex2i(-9 + WidthBy2, HeightBy2);
	glVertex2i(-1 + WidthBy2, HeightBy2);
	glVertex2i( 1 + WidthBy2, HeightBy2);
	glVertex2i( 9 + WidthBy2, HeightBy2);
	glEnd();

	//Statistics
	stat.PrintStat();

	glDisable(GL_BLEND);
}

void Engine::Loop()
{
	Display();

	//player.Control(FrameInterval);
	inter.read();
	DrawInterface();

	GetFrameTime();

	glutSwapBuffers();
	glFinish();				//may be bad!!!!!!!
}

void Engine::GetFrameTime()
{
	static double koef = 0.0005;
	static double max_FPS = 200;
	static int sleep_time;

	double currentTime = GetMillisecTime();

	static double frameTime = currentTime;  // Время последнего кадра

	//Интервал времени, прошедшего с прошлого кадра
	FrameInterval = currentTime - frameTime;
	sleep_time = (int) (1000.0/max_FPS - FrameInterval);
	if(sleep_time > 0)
	{
		Sleep(sleep_time);
		currentTime = GetMillisecTime();
		FrameInterval = currentTime - frameTime;
	}
	frameTime = currentTime;
	stat.ComputeFPS(FrameInterval);

	FrameInterval *= koef;

	static double day_koef = 10.0;
}

void Engine::Special(int button, int x, int y, bool KeyDown)
{
	switch(button)
	{
	case GLUT_KEY_F1:
		if(KeyDown)
		{
			if(!fullscreen) glutFullScreenToggle();
			else glutLeaveFullScreen();
			fullscreen = !fullscreen;
		}
		break;
//	case GLUT_KEY_F2: 	glutLeaveGameMode();
//		break;
//	default:
		//wWorld.player.bSpecial[button] = KeyDown;
	}
}

void Engine::OpenGL2d()
{
	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(0, width, 0, height, -1.0, 1.0);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_DEPTH_TEST);
}

void Engine::OpenGL3d()
{
	float fovy;
	fovy = 70.0f;

	glViewport(0, 0, width, height);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(fovy, (GLfloat)width/(GLfloat)height, 0.2f, 100);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glEnable(GL_DEPTH_TEST);
}
