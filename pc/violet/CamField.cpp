#include "CamField.h"
#include <GL/glut.h>

void _StartCapture(CvCapture *capture, IplImage **image);
boost::mutex m;
GLuint texture;

CamField::CamField(void)
{
	image = NULL;
	g_Capture = cvCreateCameraCapture(CV_CAP_ANY);
	//assert(g_Capture);

	if(g_Capture) {
		_camThread = boost::thread (_StartCapture, g_Capture, &image);
	}
}

CamField::~CamField(void)
{
	if(g_Capture) {
		boost::unique_lock<boost::mutex> lk(m);
		cvReleaseCapture(&g_Capture);
		lk.unlock();
	}
}

void CamField::Display(int width, int height)
{
	if(g_Capture) {
		boost::unique_lock<boost::mutex> lk(m);
		ComputePixPerDeg(width, height, 1);
		lk.unlock();
	} else {
		ComputePixPerDeg(width, height, 0);
		glColor3f(0.2, 0.2, 0.2);
	}
}

void CamField::ComputePixPerDeg( float width, float height, bool display )
{
	float camWidth, camHeight;
	float camFovx = 50.0;
	
	if(display && image) {
		camWidth = image->width;
		camHeight = image->height;
	} else {
		camWidth = 800;
		camHeight = 600;
	}

	float camAspectRatio = camWidth/camHeight;
	float windowAspectRatio = (float)width/(float)height;
	float fieldWidth, fieldHeight;
	float temp;

	(camAspectRatio > windowAspectRatio)? temp = (float)width/camWidth : temp = (float)height/camHeight;

	fieldWidth = camWidth*temp;
	fieldHeight = camHeight*temp;

	pixPerDeg = fieldHeight/camFovx;

	if(display && image) {
		glRasterPos2i((width-fieldWidth)/2., (height+fieldHeight)/2.-1);
		glPixelZoom(temp, -temp);
		glDrawPixels(camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, image->imageData);    
	}
}

void _StartCapture(CvCapture *capture, IplImage **image)
{
	IplImage *tempImage;
	try {
		while(1) {
			tempImage = cvQueryFrame(capture);
			cvCvtColor(tempImage, tempImage, CV_BGR2RGB);
			boost::unique_lock<boost::mutex> lk(m);
			*image = cvCloneImage(tempImage);
			lk.unlock();
		}
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}

