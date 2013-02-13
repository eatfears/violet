#include "HUD.h"
#include <stdio.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "Engine.h"
#include "PlatformDefinitions.h"

HUD::HUD(Interface *intrfc)
{
	inter = intrfc;
}


HUD::~HUD(void)
{
}

void HUD::Display(int width, int height, float Deg)
{
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth (1.);
	//HUD

	//DisplayPitch(width, height, Deg);
	DisplayYaw(width, height, Deg);
}

void HUD::DisplayYaw(int width, int height, float Deg)
{
	glLoadIdentity();
	float pixPerDeg = (((width) < (height)) ? (width) : (height))/Deg;

	static char temp_text[16] = "";
	//	350		0		010
	//	|		|		|
	//	|	|	|	|	|
	//			/\

	int arrowSize = 5;
	int arrowPos = 100;
	glTranslated(width/2,height - arrowPos,0);
	glBegin(GL_LINES);    
	glVertex2i(-1, 0);
	glVertex2i(arrowSize-1, -arrowSize*2);
	glVertex2i(0, 0);
	glVertex2i(-arrowSize, -arrowSize*2);
	glEnd();

	int size1 = 20;
	int size2 = 5;
	glTranslated(0,2,0);
	glBegin(GL_LINES);    
	for(int deg = -360; deg <= 360; deg += 5) {
		int h = deg*pixPerDeg;
		glVertex2i(h, 0);
		if(deg%10) glVertex2i(h, size2);
		else glVertex2i(h, size1);
	}
	glEnd();
}

void HUD::DisplayPitch(int width, int height, float Deg)
{
	glLoadIdentity();
	float pixPerDeg = (((width) < (height)) ? (width) : (height))/Deg;
	//pixPerDeg = (((width) > (height)) ? (width) : (height))/Deg;

	float s = 8.;
	float r = 5.;

	int sizline = 120;
	int sizline2 = 70;
	int sizline3 = 20;
	int sizline4 = 10;
	static char temp_text[16] = "";

	glTranslated(width/2,height/2,0);

	s = (M_PI * 2 / s);
	glBegin(GL_LINE_LOOP);    
	for(float i = M_PI; i >= -M_PI; i -= s)
		glVertex2f(cos(i) * r, sin(i) * r);
	glEnd();

	glBegin(GL_LINES);    
	glVertex2f(r, 0);
	glVertex2f(sizline3, 0);
	glVertex2f(-r, 0);
	glVertex2f(-sizline3, 0);
	glVertex2f(0, r);
	glVertex2f(0, sizline3);
	glVertex2f(0, -r);
	glVertex2f(0, -sizline3);

	glEnd();


	glRotatef(TODEG(inter->quadroRoll),0,0,1);
	glTranslatef(0,-TODEG(inter->quadroPitch)*pixPerDeg,0);


	int siz = (sizline2-sizline3)/5;

	glPushMatrix();

	for(int deg = -180; deg <= 180; deg += 5)
	{
		glPushMatrix();
		if(deg >= -90 && deg <= 90) b_sprintf(temp_text, "%d", deg);
		if(deg > 90) b_sprintf(temp_text, "%d", 180 - deg);
		if(deg < -90) b_sprintf(temp_text, "%d", -180 - deg);

		glPushMatrix();
		if(deg == 0 || deg == 90 || deg == -90) glTranslated(sizline*1.1,deg*pixPerDeg,0);
		else glTranslated(sizline2*1.1,deg*pixPerDeg,0);
		//if(deg >= 0) 
		glTranslated(0,-sizline4/2,0);

		glScalef(0.1, 0.1, 0.1);
		glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*) temp_text);
		glPopMatrix();
		if(deg == 0 || deg == 90 || deg == -90) glTranslated(-sizline*1.1,deg*pixPerDeg,0);
		else glTranslated(-sizline2*1.1,deg*pixPerDeg,0);
		glTranslated(-(float)strlen(temp_text)*10.0,0,0);
		//if(deg >= 0) 
		glTranslated(0,-sizline4/2,0);


		glScalef(0.1, 0.1, 0.1);
		glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*) temp_text);
		glPopMatrix();
	}

	glPopMatrix();

	glBegin(GL_LINES);

	for (int deg = -180; deg <= 180; deg += 5)
	{
		int h = deg*pixPerDeg;

		if(deg == 0 || deg == 90 || deg == -90) {
			glVertex2f(-sizline, h);
			glVertex2f(-sizline3, h);
			glVertex2f(sizline, h);
			glVertex2f(sizline3, h);
			if(deg == 0)
			{
				glVertex2f(-sizline, h);
				glVertex2f(-sizline, h-sizline4);
				glVertex2f(sizline, h);
				glVertex2f(sizline, h-sizline4);
			}
		} else if (deg > 0){
			glVertex2f(-sizline2, h);
			glVertex2f(-sizline3, h);
			glVertex2f(sizline2, h);
			glVertex2f(sizline3, h);

			if(deg < 90) {
				glVertex2f(-sizline2, h);
				glVertex2f(-sizline2, h-sizline4);
				glVertex2f(sizline2, h);
				glVertex2f(sizline2, h-sizline4);
			} else {
				glVertex2f(-sizline2, h);
				glVertex2f(-sizline2, h+sizline4);
				glVertex2f(sizline2, h);
				glVertex2f(sizline2, h+sizline4);
			}


		} else if(deg < 0) {
			glVertex2f(-sizline2, h);
			glVertex2f(-sizline2 + siz, h);
			glVertex2f(-sizline2 + siz*2, h);
			glVertex2f(-sizline2 + siz*3, h);
			glVertex2f(-sizline2 + siz*4, h);
			glVertex2f(-sizline2 + siz*5, h);
			glVertex2f(sizline2, h);
			glVertex2f(sizline2 - siz, h);
			glVertex2f(sizline2 - siz*2, h);
			glVertex2f(sizline2 - siz*3, h);
			glVertex2f(sizline2 - siz*4, h);
			glVertex2f(sizline2 - siz*5, h);

			if(deg < -90) {
				glVertex2f(-sizline2, h);
				glVertex2f(-sizline2, h-sizline4);
				glVertex2f(sizline2, h);
				glVertex2f(sizline2, h-sizline4);
			} else {
				glVertex2f(-sizline2, h);
				glVertex2f(-sizline2, h+sizline4);
				glVertex2f(sizline2, h);
				glVertex2f(sizline2, h+sizline4);
			}
		}
	}

	glEnd();
}