#include <stdint.h>
#include <glpk.h>

namespace ConfigureGraph {
	const char* standardNodeColor = "lightblue";
	const char* criticalNodeColor = "orange";
	const char* startNodeColor = "red";
	const char* finishNodeColor = "green";
}

namespace ConfigureResourceUtilization	{
	const char* blockColor = "blue";
	double spaceBetweenResources = 3;
	double axisElongationX = 1.02;
	double axisElongationY = 1.04;
	double textDist = 10;
	double ticsLength = 0.1;
}

namespace ConfigureILPSolver    {
	int32_t messageLevel = GLP_MSG_ERR;
	int32_t branchingTechnique = GLP_BR_LFV;
	int32_t backtrackingTechnique = GLP_BT_DFS;
	int32_t timeLimit = 10000;
	double mipGap = 0.0;
}

namespace ConfigureScheduleAnalysis     {
	uint32_t boxSize = 2;
	uint32_t averageWindow = 5;
}

