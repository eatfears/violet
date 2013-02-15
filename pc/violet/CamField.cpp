#include "CamField.h"
#include <GL/glut.h>


CamField::CamField(void)
{
	g_Capture = cvCreateCameraCapture(CV_CAP_ANY);
	assert(g_Capture);
}

CamField::~CamField(void)
{
}

void CamField::displayCam(int width, int height)
{
	IplImage *image = cvQueryFrame(g_Capture);

	cvCvtColor(image, image, CV_BGR2RGB);
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, image->width, image->height, GL_RGB, GL_UNSIGNED_BYTE, image->imageData);

	glEnable(GL_TEXTURE_2D);

	glLoadIdentity();

	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();

	glBegin(GL_QUADS);
	glTexCoord2f(0.0f, 1.0f); 
	glVertex2f(0.0f, 0.0f);
	glTexCoord2f(1.0f, 1.0f); 
	glVertex2f(width, 0.0f);
	glTexCoord2f(1.0f, 0.0f); 
	glVertex2f(width, height);
	glTexCoord2f(0.0f, 0.0f); 
	glVertex2f(0.0f, height);
	glEnd();
	glDisable(GL_TEXTURE_2D);
}

void CamField::release()
{
	cvReleaseCapture(&g_Capture);
}