#include "Statistics.h"
#include <stdio.h>
#include "Engine.h"
#include "PlatformDefinitions.h"


Statistics::Statistics(Engine& eng)
	: engine(eng)
{
	TimeCount = 0;
	FrameCount = 0;
	reRenderedChunks = 0;
}

Statistics::~Statistics(void)
{
}

void Statistics::ComputeFPS( double FrameInterval )
{
	FrameCount++;
	TimeCount += FrameInterval;
}

void Statistics::PrintStat(void)
{
	static char cFPS[20] = "";
	static char pos[1024] = "";
	static void *font = GLUT_BITMAP_HELVETICA_12;
	static int h, m, s;

	if (TimeCount > 100) {
		b_sprintf(cFPS, "FPS: %0.1f\n", 1000.0*FrameCount/TimeCount);
		TimeCount = 0.0;
		FrameCount = 0;
	}

	RenderString(50, engine.height - 50, font, cFPS);

	b_sprintf(pos, "Pitch: %f\n", engine.inter.quadroPitch);
	RenderString(50, engine.height - 70, font, pos);
	b_sprintf(pos, "Roll: %f\n", engine.inter.quadroRoll);
	RenderString(50, engine.height - 90, font, pos);
	b_sprintf(pos, "Yaw: %f\n", engine.inter.quadroYaw);
	RenderString(50, engine.height - 110, font, pos);
	b_sprintf(pos, "Magnet heading: %f\n", engine.inter.quadroMagHead);
	RenderString(50, engine.height - 130, font, pos);

	/*
	b_sprintf(pos, "Analog readings:\n%f, %f, %f, %f\t%f\n%f, %f, %f\n", 
		engine.inter.quadroAnalogReads[0], 
		engine.inter.quadroAnalogReads[1], 
		engine.inter.quadroAnalogReads[2], 
		engine.inter.quadroAnalogReads[3], 
		engine.inter.quadroAnalogReads[4], 
		engine.inter.quadroAnalogReads[5], 
		engine.inter.quadroAnalogReads[6], 
		engine.inter.quadroAnalogReads[7]);
	RenderString(250, engine.height - 50, font, pos);
	b_sprintf(pos, "Analog readings:\n%f, %f, %f, %f\t%f\n%f, %f, %f\n", 
		engine.inter.quadroAnalogOffsets[0], 
		engine.inter.quadroAnalogOffsets[1], 
		engine.inter.quadroAnalogOffsets[2], 
		engine.inter.quadroAnalogOffsets[3], 
		engine.inter.quadroAnalogOffsets[4], 
		engine.inter.quadroAnalogOffsets[5], 
		engine.inter.quadroAnalogOffsets[6], 
		engine.inter.quadroAnalogOffsets[7]);
	RenderString(250, engine.height - 100, font, pos);
	b_sprintf(pos, "Dt: %f\n", engine.inter.quadroDeltaTime);
	RenderString(250, engine.height - 150, font, pos);


	b_sprintf(pos, "Accel vector: %f, %f, %f\n", engine.inter.quadroAccelVector[0], engine.inter.quadroAccelVector[1], engine.inter.quadroAccelVector[2]);
	RenderString(250, engine.height - 170, font, pos);
	*/
	//com->read(&DCM_Matrix[0], 4*3);
	//com->read(&DCM_Matrix[1], 4*3);
	//com->read(&DCM_Matrix[2], 4*3);

}

void Statistics::RenderString(int x, int y, void *font, const char string[])
{
	glRasterPos2i(x, y);
	glutBitmapString(font, (const unsigned char*) string);
}
