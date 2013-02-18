#include "CamField.h"
#include <GL/glut.h>


CamField::CamField(void)
{
	g_Capture = cvCreateCameraCapture(CV_CAP_ANY);
	//assert(g_Capture);
}

CamField::~CamField(void)
{
	if(g_Capture) {
		cvReleaseCapture(&g_Capture);
	}
}

void CamField::Display(int width, int height)
{
	float camWidth;
	float camHeight;
	float camFovx = 50;

	if(g_Capture) {
		IplImage *image = cvQueryFrame(g_Capture);

		cvCvtColor(image, image, CV_BGR2RGB);
		camWidth = image->width;
		camHeight = image->height;
		gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->imageData);
	}
	else {
		camWidth = 800;
		camHeight = 600;
		glColor3f(0.2, 0.2, 0.2);
	}

	float camAspectRatio = camWidth/camHeight;
	float windowAspectRatio = (float)width/(float)height;

	float fieldWidth;
	float fieldHeight;
	float temp;
	(camAspectRatio > windowAspectRatio)? temp = camWidth/(float)width : temp = camHeight/(float)height;

	fieldHeight = camHeight/temp;
	fieldWidth = camWidth/temp;

	pixPerDeg = fieldHeight/camFovx;
	//pixPerDeg = fieldWidth/camFovy;

	glEnable(GL_TEXTURE_2D);
	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glTranslated(width/2, height/2, 0);

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); 
	glVertex2f(-fieldWidth/2, -fieldHeight/2);
	glTexCoord2f(1.0f, 1.0f); 
	glVertex2f(fieldWidth/2, -fieldHeight/2);
	glTexCoord2f(1.0f, 0.0f); 
	glVertex2f(fieldWidth/2, fieldHeight/2);
	glTexCoord2f(0.0f, 0.0f); 
	glVertex2f(-fieldWidth/2, fieldHeight/2);
	glEnd();

	glDisable(GL_TEXTURE_2D);
}
