#include "Interface.h"


Interface::Interface(void)
{
	com = new COMPort("Com2");

	com->setBitRate(COMPort::br115200);
}


Interface::~Interface(void)
{
	delete com;
}

void Interface::read()
{
	while(1)
	{
		if(com->read() == 'h')
		if(com->read() == 'e')
		if(com->read() == 'l')
		if(com->read() == 'l')
		if(com->read() == 'o')
			break;
	}

	com->read(&pitch, 4);
	com->read(&roll, 4);
	com->read(&yaw, 4);
	com->read(&mhead, 4);

	com->read(&AN, 4*8);
	com->read(&AN_OFFSET, 4*8);
	com->read(&G_Dt, 4);

	com->read(&DCM_Matrix[0], 4*3);
	com->read(&DCM_Matrix[1], 4*3);
	com->read(&DCM_Matrix[2], 4*3);

	com->read(&Accel_vector, 4*3);
}

