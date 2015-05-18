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
#ifndef DEFAULT_PARAMETERS_H
#define DEFAULT_PARAMETERS_H

#include <stdint.h>

namespace ConfigureGraph {
	extern const char* standardNodeColor;
	extern const char* criticalNodeColor;
	extern const char* startNodeColor;
	extern const char* finishNodeColor;
	extern const char* graphOrientation;
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

