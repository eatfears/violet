#pragma once

#include <opencv/cv.hpp>  // Gaussian Blur
#include <opencv/highgui.h>  // OpenCV window I/O
#include <boost/thread.hpp>

class CamField
{
public:
	CamField(void);
	~CamField(void);

	float pixPerDeg;

	void Display(int width, int height);

private:
	boost::thread _camThread;
	CvCapture* _gCapture;
	IplImage *_image;
	void _ComputePixPerDeg(float width, float height, bool display);
};

