#include "CamField.h"
#include <GL/glut.h>

void _StartCapture(CvCapture *capture, IplImage **image);

CamField::CamField(void)
{
	g_Capture = cvCreateCameraCapture(CV_CAP_ANY);
	//assert(g_Capture);

	if(g_Capture) {
		_camThread = boost::thread (_StartCapture, g_Capture, &image);
	}
}

CamField::~CamField(void)
{
	if(g_Capture) {
		cvReleaseCapture(&g_Capture);
	}
}
boost::mutex m;
bool ready = 0;

void CamField::Display(int width, int height)
{
	static float camWidth;
	static float camHeight;
	float camFovx = 50;

	if(g_Capture) {
		if(ready) {

			boost::unique_lock<boost::mutex> lk(m);
			camWidth = image->width;
			camHeight = image->height;
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, image->imageData);
			ready = 0;
			lk.unlock();
		}
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

void _StartCapture(CvCapture *capture, IplImage **image)
{
	IplImage *tempImg;
	try{
		while(1) {
			tempImg = cvQueryFrame(capture);
			cvCvtColor(tempImg, tempImg, CV_BGR2RGB);

			boost::unique_lock<boost::mutex> lk(m);
			*image = tempImg;
			ready = 1;
			lk.unlock();
		}
	}
	catch (const std::runtime_error &e) {
		std::cout << e.what() << std::endl;
	}
}

