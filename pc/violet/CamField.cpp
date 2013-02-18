#include "CamField.h"
#include <GL/glut.h>


CamField::CamField(void)
{
	//g_Capture = cvCreateCameraCapture(CV_CAP_ANY);
	//assert(g_Capture);
}

CamField::~CamField(void)
{
}

void CamField::displayCam(int width, int height)
{
	//IplImage *image = cvQueryFrame(g_Capture);

	//cvCvtColor(image, image, CV_BGR2RGB);
	//gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->imageData);
	glColor3f(0.2, 0.2, 0.2);
	float camWidth = 800;
	float camHeight = 600;
	float camAngleOfView = 50;

	float camAspectRatio = camWidth/camHeight;
	float windowAspectRatio = (float)width/(float)height;

	float fieldWidth;
	float fieldHeight;
	float temp;
	(camAspectRatio > windowAspectRatio)? temp = camWidth/(float)width : temp = camHeight/(float)height;

	fieldHeight = camHeight/temp;
	fieldWidth = camWidth/temp;

	pixPerDeg = (((fieldWidth) < (fieldHeight)) ? (fieldWidth) : (fieldHeight))/camAngleOfView;
	//pixPerDeg = (((fieldWidth) > (fieldHeight)) ? (fieldWidth) : (fieldHeight))/camAngleOfView;

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

void CamField::release()
{
	//cvReleaseCapture(&g_Capture);
}