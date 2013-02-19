#pragma once

#include <opencv/cv.hpp>  // Gaussian Blur
#include <opencv/highgui.h>  // OpenCV window I/O
#include <boost/thread.hpp>


class CamField
{
public:
	CamField(void);
	~CamField(void);

	CvCapture* g_Capture;
	float pixPerDeg;

	void release();
	void Display(int width, int height);

private:
	boost::thread _camThread;
	IplImage *image;
};

