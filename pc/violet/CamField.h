#pragma once

#include <opencv/cv.hpp>  // Gaussian Blur
#include <opencv/highgui.h>  // OpenCV window I/O


class CamField
{
public:
	CamField(void);
	~CamField(void);

	CvCapture* g_Capture;
	float pixPerDeg;

	void release();
	void displayCam(int width, int height);
};

