#include "Interface.h"


Interface::Interface(void)
{
	quadroPitch = quadroRoll = quadroYaw = quadroMagHead = quadroDeltaTime = 0;
	for(int i = 0; i < 8; i++) {
		quadroAnalogReads[i] = quadroAnalogOffsets[i] = 0;
	}
	for(int i = 0; i < 3; i++) {
		for(int j = 0; j < 3; j++) {
			quadroDCMMatrix[i][j] = 0;
		}
		quadroAccelVector[i] = 0;
	}
}


Interface::~Interface(void)
{
}

void _StartReading(Interface *ifc, COMPort *_comPort);

void Interface::Connect()
{
	_comPort = new COMPort("Com13");
	_comPort->setBitRate(COMPort::br115200);
	connected = true;

	_commThread = boost::thread (_StartReading, this, _comPort);
}

void Interface::Disconnect()
{
	delete _comPort;
	connected = false;
}

void _StartReading(Interface *ifc, COMPort *_comPort)
{
	while(1) {

		while(1) {
			if(_comPort->read() == 'h')
				if(_comPort->read() == 'e')
					if(_comPort->read() == 'l')
						if(_comPort->read() == 'l')
							if(_comPort->read() == 'o')
								break;
		}

		_comPort->read(&ifc->quadroPitch, 4);
		_comPort->read(&ifc->quadroRoll, 4);
		_comPort->read(&ifc->quadroYaw, 4);
		_comPort->read(&ifc->quadroMagHead, 4);

		_comPort->read(&ifc->quadroAnalogReads, 4*8);
		_comPort->read(&ifc->quadroAnalogOffsets, 4*8);
		_comPort->read(&ifc->quadroDeltaTime, 4);

		_comPort->read(&ifc->quadroDCMMatrix[0], 4*3);
		_comPort->read(&ifc->quadroDCMMatrix[1], 4*3);
		_comPort->read(&ifc->quadroDCMMatrix[2], 4*3);

		_comPort->read(&ifc->quadroAccelVector, 4*3);
	}
}

