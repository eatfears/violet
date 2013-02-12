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

	if (TimeCount > 100)
	{
		b_sprintf(cFPS, "FPS: %0.1f\n", 1000.0*FrameCount/TimeCount);
		TimeCount = 0.0;
		FrameCount = 0;
	}

	RenderString(50, engine.height - 50, font, cFPS);

	b_sprintf(pos, "Pitch: %f\n", engine.inter.pitch);
	RenderString(50, engine.height - 70, font, pos);
	b_sprintf(pos, "Roll: %f\n", engine.inter.roll);
	RenderString(50, engine.height - 90, font, pos);
	b_sprintf(pos, "Yaw: %f\n", engine.inter.yaw);
	RenderString(50, engine.height - 110, font, pos);
	b_sprintf(pos, "Magnet heading: %f\n", engine.inter.mhead);
	RenderString(50, engine.height - 130, font, pos);


	b_sprintf(pos, "Analog readings:\n%f, %f, %f, %f\t%f\n%f, %f, %f\n", engine.inter.AN[0], engine.inter.AN[1], engine.inter.AN[2], engine.inter.AN[3], engine.inter.AN[4], engine.inter.AN[5], engine.inter.AN[6], engine.inter.AN[7]);
	RenderString(250, engine.height - 50, font, pos);
	b_sprintf(pos, "Analog readings:\n%f, %f, %f, %f\t%f\n%f, %f, %f\n", engine.inter.AN_OFFSET[0], engine.inter.AN_OFFSET[1], engine.inter.AN_OFFSET[2], engine.inter.AN_OFFSET[3], engine.inter.AN_OFFSET[4], engine.inter.AN_OFFSET[5], engine.inter.AN_OFFSET[6], engine.inter.AN_OFFSET[7]);
	RenderString(250, engine.height - 100, font, pos);
	b_sprintf(pos, "Dt: %f\n", engine.inter.G_Dt);
	RenderString(250, engine.height - 150, font, pos);


	b_sprintf(pos, "Accel vector: %f, %f, %f\n", engine.inter.Accel_vector[0], engine.inter.Accel_vector[1], engine.inter.Accel_vector[2]);
	RenderString(250, engine.height - 170, font, pos);

	//com->read(&DCM_Matrix[0], 4*3);
	//com->read(&DCM_Matrix[1], 4*3);
	//com->read(&DCM_Matrix[2], 4*3);

}

void Statistics::RenderString(int x, int y, void *font, const char string[])
{
	glRasterPos2i(x, y);
	glutBitmapString(font, (const unsigned char*) string);
}
