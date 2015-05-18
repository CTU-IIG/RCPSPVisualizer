/*
	This file is part of the RCPSPVisualizer program.

	RCPSPVisualizer is free software: you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation, either version 3 of the License, or
	(at your option) any later version.

	RCPSPVisualizer is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with RCPSPVisualizer. If not, see <http://www.gnu.org/licenses/>.
*/
#include <stdint.h>
#include <glpk.h>

namespace ConfigureGraph {
	const char* standardNodeColor = "\"#ccccff\"";
	const char* criticalNodeColor = "orange";
	const char* startNodeColor = "red";
	const char* finishNodeColor = "green";
	const char* graphOrientation = "TD"; // LR ~ left to right, TD ~ top to down
}

namespace ConfigureResourceUtilization	{
	const char* blockColor = "blue";
	double spaceBetweenResources = 3;
	double axisElongationX = 1;
	double axisElongationY = 0.7;
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

