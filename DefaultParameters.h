#ifndef DEFAULT_PARAMETERS_H
#define DEFAULT_PARAMETERS_H

#include <stdint.h>

namespace ConfigureGraph {
	extern const char* standardNodeColor;
	extern const char* criticalNodeColor;
	extern const char* startNodeColor;
	extern const char* finishNodeColor;
}

namespace ConfigureResourceUtilization	{
	extern const char* blockColor;
	extern double spaceBetweenResources;
	extern double axisElongationX;
	extern double axisElongationY;
	extern double textDist;
	extern double ticsLength;
}

namespace ConfigureILPSolver	{
	extern int32_t messageLevel;
	extern int32_t branchingTechnique;
	extern int32_t backtrackingTechnique;
	extern int32_t timeLimit;
	extern double mipGap;
}

namespace ConfigureScheduleAnalysis	{
	extern uint32_t boxSize;
	extern uint32_t averageWindow;
}

#endif

