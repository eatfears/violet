#pragma once
#include "comport.h"
#include <boost/thread.hpp>

class Interface
{
public:
	Interface(void);
	~Interface(void);

	void Connect();
	void Disconnect();

	bool connected;

	float quadroPitch;
	float quadroYaw;
	float quadroRoll;
	float quadroMagHead;
	float quadroAnalogReads[8];
	float quadroAnalogOffsets[8];
	float quadroDeltaTime;
	float quadroDCMMatrix[3][3];
	float quadroAccelVector[3];

private:
	boost::thread _commThread;
	int _timeout;
	char _syncMessage[6];
	COMPort *_comPort;
};

