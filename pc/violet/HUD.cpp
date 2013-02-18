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
	this->width = width;
	this->height = height;
	this->pixPerDeg = Deg;

	glColor3f(1.0, 1.0, 1.0);
	glLineWidth (1.3);
	//HUD
	glEnable( GL_LINE_SMOOTH );
	glEnable( GL_POLYGON_SMOOTH );
	glHint( GL_LINE_SMOOTH_HINT, GL_NICEST );
	glHint( GL_POLYGON_SMOOTH_HINT, GL_NICEST );
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	//glBlendFunc(GL_ONE_MINUS_DST_COLOR, GL_ZERO);

	DisplayPitch();
	DisplayYaw();
	//DisplayCompass();
}

void HUD::DisplayYaw()
{
	glLoadIdentity();

	static char temp_text[16] = "";
	//	350		0		010
	//	|		|		|
	//	|	|	|	|	|
	//			/\		

	int scissSize = pixPerDeg*30;
	glScissor  (width/2-scissSize, height - 100, scissSize*2, height);
	glEnable(GL_SCISSOR_TEST);
	/*
	glColor3f(0.1,0.1,0.1);
	glBegin(GL_QUADS);
	glVertex2i(0, 0);
	glVertex2i(width, 0);
	glVertex2i(width, height);
	glVertex2i(0, height);
	glEnd();
	glColor3f(1,1,1);
	*/

	int arrowSize = 5;
	int yPos = 70;
	glTranslated(width/2, height - yPos,0);
	glBegin(GL_LINES);    
	glVertex2i(-1, 0);
	glVertex2i(arrowSize-1, -arrowSize*2);
	glVertex2i(0, 0);
	glVertex2i(-arrowSize, -arrowSize*2);
	glEnd();
	int tempyaw = TODEG(inter->quadroYaw);
	while(tempyaw >= 360) tempyaw -= 360;
	while(tempyaw < 0) tempyaw += 360;
	b_sprintf(temp_text, "%3.3d", tempyaw);

	glPushMatrix();
	glTranslatef(0,-25,0);
	glBegin(GL_LINE_LOOP);    //border
	glVertex2i(-15, -5);
	glVertex2i(15, -5);
	glVertex2i(15, 15);
	glVertex2i(-15, 15);
	glEnd();

	glTranslatef(-12,0,0);
	glScalef(0.1, 0.1, 0.1);
	glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*) temp_text);	//heading
	glPopMatrix();

	
	glTranslatef(-TODEG(inter->quadroYaw)*pixPerDeg,0,0);

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

	glTranslated(-12,size1+2,0);
	for(int deg = -360; deg <= 360; deg += 10) {
		glPushMatrix();
		tempyaw = deg;
		while(tempyaw >= 360) tempyaw -= 360;
		while(tempyaw < 0) tempyaw += 360;
		b_sprintf(temp_text, "%3.3d", tempyaw);
		//if(deg >= -90 && deg <= 90) b_sprintf(temp_text, "%d", deg);
		//if(deg > 90) b_sprintf(temp_text, "%d", 180 - deg);
		//if(deg < -90) b_sprintf(temp_text, "%d", -180 - deg);

		glTranslated(deg*pixPerDeg,0,0);

		glScalef(0.1, 0.1, 0.1);
		glutStrokeString(GLUT_STROKE_ROMAN, (unsigned char*) temp_text);
		glPopMatrix();

	}

	glDisable(GL_SCISSOR_TEST);

}

void HUD::DisplayPitch()
{
	glLoadIdentity();

	int scissSize = pixPerDeg*20;
	int scissSize2 = 100;
	glScissor  (width/2-scissSize, scissSize2, scissSize*2, height - 100 - scissSize2);
	glEnable(GL_SCISSOR_TEST);
	/*
	glColor3f(0.1,0.1,0.1);
	glBegin(GL_QUADS);
	glVertex2i(0, 0);
	glVertex2i(width, 0);
	glVertex2i(width, height);
	glVertex2i(0, height);
	glEnd();
	glColor3f(1,1,1);
	
	*/

	float r = 5.;

	int sizline = 120;
	int sizline2 = 70;
	int sizline3 = 20;
	int sizline4 = 10;
	static char temp_text[16] = "";

	glTranslated(width/2,height/2,0);

	_circle(r, 8);
	glBegin(GL_LINES);    
	glVertex2f(r, 0);
	glVertex2f(sizline3, 0);
	glVertex2f(-r, 0);
	glVertex2f(-sizline3, 0);
	glVertex2f(0, r);
	glVertex2f(0, sizline3/2);
	glVertex2f(0, -r);
	glVertex2f(0, -sizline3/2);
	glEnd();

	glRotatef(TODEG(inter->quadroRoll),0,0,1);
	glTranslatef(0,-TODEG(inter->quadroPitch)*pixPerDeg,0);

	int siz = (sizline2-sizline3)/5;

	glPushMatrix();

	for(int deg = -180; deg <= 180; deg += 5) {
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

	for (int deg = -180; deg <= 180; deg += 5) {
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

void HUD::_circle( float r, float s )
{
	s = (M_PI * 2 / s);
	glBegin(GL_LINE_LOOP);    
	for(float i = M_PI; i >= -M_PI; i -= s)
		glVertex2f(cos(i) * r, sin(i) * r);
	glEnd();
}

void HUD::DisplayCompass()
{
	glLoadIdentity();

	static char temp_text[16] = "";

	int innerRad = 30;
	int size = 5;
	int yPos = 40;

	glTranslated(width/2, yPos,0);


	//glTranslatef(-TODEG(inter->quadroYaw)*pixPerDeg,0,0);
	//glRotatef(TODEG(inter->quadroRoll),0,0,1);

	for(int deg = 0; deg < 360; deg += 10) {
		glBegin(GL_LINES);    
		glVertex2i(0, innerRad);
		glVertex2i(0, innerRad+size);
		glEnd();
		glRotatef(10,0,0,1);
	}

}
