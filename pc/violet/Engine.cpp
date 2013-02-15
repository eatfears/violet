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
	warp = false;

	width = 0;
	height = 0;
	FrameInterval = 0.0;

	hud = new HUD(&inter);
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


	glShadeModel(GL_SMOOTH);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glClearDepth(1.0f);											// ��������� ������� ������ �������

	glEnable(GL_DEPTH_TEST);									// ��������� ���� �������

	glDepthFunc(GL_LEQUAL);										// ��� ����� �������
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);			// ��������� � ���������� �����������

	//������� �������
	glEnable(GL_TEXTURE_2D);

	//�������������� ���������� �������� � ��������� �����
	glEnable(GL_NORMALIZE);

	return true;
}

void Engine::Reshape(int width, int height)
{
	if(height == 0)											// �������������� ������� �� ����
		height = 1;

	this->width = width;
	this->height = height;

	//OpenGL3d();
	//glutPostRedisplay();
}

void Engine::Display()
{
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);		// �������� ����� � ����� �������
	glLoadIdentity();											// �������� ������� �������
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	//OpenGL3d();



	//glColor3f(0.4f, 0.4f, 0.4f);
		/*
	glBegin(GL_TRIANGLES);
	glVertex3f(-1.0f, -1.0f, -2.0f);
	glVertex3f( 0.0f,  1.0f, -2.0f);
	glVertex3f( 1.0f, -1.0f, -2.0f);
	glEnd();*/

	//GL_PROJECTION
	//GL_MODELVIEW
	// 
	/*
	 *quadro
	glTranslatef(0, 0, -5);
	glMatrixMode(GL_MODELVIEW);
	static float s = 0;
	s+= 0.09;
	glRotatef(s,1,0,0);
	glPushMatrix();
	glScalef(20,1,1);
	glutSolidCube(0.1);
	glPopMatrix();
	glPushMatrix();
	glScalef(1,20,1);
	glutSolidCube(0.1);
	glPopMatrix();
	
	glTranslatef(2, 0, 0);
	glutSolidCylinder(1,0.1,50,1);
	glTranslatef(-4, 0, 0);
	glutSolidCylinder(1,0.1,50,1);
	glTranslatef(2, 2, 0);
	glutSolidCylinder(1,0.1,50,1);
	glTranslatef(0, -4, 0);
	glutSolidCylinder(1,0.1,50,1);
	*/

	//Set position
	//glRotated(-player.dSpinX, 1.0, 0.0, 0.0);
	//glRotated(-player.dSpinY, 0.0, 1.0, 0.0);
	//glTranslated(-player.dPositionX, -player.dPositionY, -player.dPositionZ);


	//glEnable(GL_FOG);
	//glFogi(GL_FOG_MODE,  GL_LINEAR);
	//glHint(GL_FOG_HINT, GL_DONT_CARE);
	/*
	if(player.UnderWater)
	{
		glFogfv(GL_FOG_COLOR, WaterFogColor);
		glFogf(GL_FOG_DENSITY, 20.0);
		glFogf(GL_FOG_START, BLOCK_SIZE*10);
		glFogf(GL_FOG_END, BLOCK_SIZE*30);
	}*/



	//glColor3d(1.0, 1.0, 1.0);

//	glBindTexture(GL_TEXTURE_2D, tex[TERRAIN]);
	//int render;

	/*if(player.bKeyboard['Z'])
	{
		player.bKeyboard['Z'] = 0;
		wWorld.SoftLight = !wWorld.SoftLight;
		wWorld.LightToRefresh = true;
	}
	*/


	//glEnable(GL_ALPHA_TEST);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	//glDisable(GL_ALPHA_TEST);
	//glDisable(GL_BLEND);

}

void Engine::Keyboard(unsigned char button, int x, int y, bool KeyDown)
{
#ifdef _WIN32
	button = VkKeyScan(button);
#endif

	switch(button)
	{
	case KEY_ESCAPE: 
		cam.release();
		glutExit();
		break;

	case 'S':
		if(KeyDown) {
			if(inter.connected) {
				inter.Disconnect();
				std::cout << "Disconnected" << std::endl;
			} else {
				inter.Connect();
				std::cout << "Connected" << std::endl;
			}
		}
		break;	
	
	case ' ':
		if(KeyDown) {
			warp = !warp;
			if(warp) glutSetCursor(GLUT_CURSOR_NONE);
			else glutSetCursor(GLUT_CURSOR_INHERIT);
		}
		break;

	default:
		//wWorld.player.bKeyboard[button] = KeyDown;
		break;
	}
}

void Engine::MouseMotion(int x, int y)
{
	if(bMousing) {
		bMousing = false;

		if(warp)
			glutWarpPointer(width/2, height/2);
	} else {
		bMousing = true;
	}
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
#endif // _WIN32

	unsigned int seed = std::time(0);
}

void Engine::DrawInterface()
{
	//glPushMatrix();
	//glPopMatrix();
	//glLoadIdentity();
	OpenGL2d();
	
	hud->Display(width, height, 50);

	glLoadIdentity();
	//Statistics
	stat.PrintStat();

	glDisable(GL_BLEND);
}

void Engine::Loop()
{
	Display();

	//player.Control(FrameInterval);
	//inter.Read();
	cam.displayCam(width, height);
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

	static double frameTime = currentTime;  // ����� ���������� �����

	//�������� �������, ���������� � �������� �����
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
