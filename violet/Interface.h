#pragma once
#include "comport.h"

class Interface
{
public:
	Interface(void);
	~Interface(void);

	void read();
	int timeout;

	char hello[6];
	float pitch;
	float yaw;
	float roll;
	float mhead;
	float AN[8];
	float AN_OFFSET[8];
	float G_Dt;
	float DCM_Matrix[3][3];
	float Accel_vector[3];

	COMPort *com;
};

