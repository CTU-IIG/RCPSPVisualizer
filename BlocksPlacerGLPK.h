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
#ifndef HLIDAC_PES_BLOCKS_PLACER_GLPK
#define HLIDAC_PES_BLOCKS_PLACER_GLPK

#include <iostream>
#include <string>
#include <vector>
#include <utility>
#include <stdint.h>
#include <glpk.h>

class BlocksPlacerGLPK {
	public:
		BlocksPlacerGLPK(uint32_t numberOfActivities, uint32_t resourceId,  uint32_t *startTimesById, uint32_t* activitiesDuration,
				uint32_t *capacityOfResources, uint32_t **requiredCap);
		std::vector<std::pair<uint32_t, uint32_t> > solve(bool verbose = false);
		~BlocksPlacerGLPK();
	private:

		std::string getGlpkError(int code) const;

		glp_prob *mip;
		glp_iocp settings;
		std::vector<uint32_t> activities;
};

#endif
