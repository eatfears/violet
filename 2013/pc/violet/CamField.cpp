#include "CamField.h"
#include <GL/glut.h>

#define DISPLAY_METHOD	1

void _StartCapture(CvCapture **capture, IplImage **image);
boost::mutex imageMutex, captureMutex;
bool ready = false;

CamField::CamField(void)
{
	_image = NULL;
	_gCapture = cvCreateCameraCapture(CV_CAP_ANY);
	//assert(_gCapture);

	if(_gCapture) {
		_camThread = boost::thread (_StartCapture, &_gCapture, &_image);
	}
}

CamField::~CamField(void)
{
	if(_gCapture) {
		boost::unique_lock<boost::mutex> capLock(captureMutex);
		cvReleaseCapture(&_gCapture);
		capLock.unlock();
		_camThread.join();
		cvReleaseImage(&_image);
	}
}

void CamField::Display(int width, int height)
{
	if(_gCapture) {
		boost::unique_lock<boost::mutex> imageLock(imageMutex);
		_ComputePixPerDeg(width, height, 1);
		imageLock.unlock();
	} else {
		_ComputePixPerDeg(width, height, 0);
		glColor3f(0.2, 0.2, 0.2);
	}
}

void CamField::_ComputePixPerDeg( float width, float height, bool display )
{
	float camWidth, camHeight;
	float camFovx = 40.0;
	
	if(display && _image) {
		camWidth = _image->width;
		camHeight = _image->height;
	} else {
		camWidth = 800;
		camHeight = 600;
	}

	float camAspectRatio = camWidth/camHeight;
	float windowAspectRatio = (float)width/(float)height;
	float fieldWidth, fieldHeight;
	float temp;

	(camAspectRatio > windowAspectRatio) ? temp = (float)width/camWidth : temp = (float)height/camHeight;

	fieldWidth = camWidth*temp;
	fieldHeight = camHeight*temp;

	pixPerDeg = fieldHeight/camFovx;

	if(display && _image) {
#if DISPLAY_METHOD == 1
		glRasterPos2i((width-fieldWidth)/2., (height+fieldHeight)/2.-1);
		glPixelZoom(temp, -temp);
		glDrawPixels(camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, _image->imageData);  
		glPixelZoom(1, 1);
#elif DISPLAY_METHOD == 2
		if(ready) {
			gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, camWidth, camHeight, GL_RGB, GL_UNSIGNED_BYTE, _image->imageData);
		}
#elif DISPLAY_METHOD == 3
		if(ready) {
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
			gluScaleImage(GL_RGB, camWidth, camHeight, GL_UNSIGNED_BYTE, _image->imageData, 512, 512, GL_UNSIGNED_BYTE, _image->imageData);
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, 512, 512, 0, GL_RGB, GL_UNSIGNED_BYTE, _image->imageData);
			ready = 0;
		}
#endif

#if (DISPLAY_METHOD == 2) || (DISPLAY_METHOD == 3)
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
#endif
	}
}

void _StartCapture(CvCapture **capture, IplImage **image)
{
	IplImage *tempImage;
	try {
		while(1) {
			boost::unique_lock<boost::mutex> captureLock(captureMutex);
			if(*capture) tempImage = cvQueryFrame(*capture);
			else {
				cvReleaseImage(&tempImage);
				return;
			}
			captureLock.unlock();

			cvCvtColor(tempImage, tempImage, CV_BGR2RGB);
			boost::unique_lock<boost::mutex> imageLock(imageMutex);
			cvReleaseImage(image);
			*image = cvCloneImage(tempImage);
			ready = 1;
			imageLock.unlock();
		}
	} catch (const std::exception &e) {
		std::cout << e.what() << std::endl;
	}
}

