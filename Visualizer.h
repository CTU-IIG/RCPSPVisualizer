#ifndef HLIDAC_PES_VISUALIZER_H
#define HLIDAC_PES_VISUALIZER_H

#include <string>
#include <sstream>
#include <vector>
#include <stdint.h>

#define NUMBER_OF_COLORS 17

class Visualizer {

	public:

		Visualizer(const std::string& fileName);
		~Visualizer();

	protected:

		static std::vector<std::string> getLatexTikzHeader();
		static std::vector<std::string> getTikzStyle();
		static std::vector<std::string> envelopLatexBody(const std::vector<std::string>& body, double scale);
		static std::vector<std::string> composeCompleteLatexDocument(const std::vector<std::string>& header,
				const std::vector<std::string>& style, const std::vector<std::string>& body, double scale);

		static std::string extractLine(std::stringstream& ss);
		static void writeLinesToFile(const std::string& file, const std::vector<std::string>& doc);
		
		std::string freeAllocatedResources(const int32_t& phase = -1);


		uint32_t numberOfActivities;
		uint32_t numberOfResources;
		uint32_t *activitiesDuration;
		uint32_t *resourceCapacities;
		uint32_t **requiredResources;
		uint32_t *numberOfSuccessors;
		uint32_t *numberOfPredecessors;
		uint32_t **successors;
		uint32_t **predecessors;

		uint32_t projectMakespan;
		uint32_t *scheduleOrder;
		uint32_t *startTimesById;

		int32_t ** distanceMatrix;

		static const char* colors[];
};

#endif

