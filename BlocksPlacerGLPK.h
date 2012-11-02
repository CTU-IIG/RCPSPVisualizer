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
