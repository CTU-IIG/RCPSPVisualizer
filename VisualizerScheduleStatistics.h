#ifndef VISUALIZER_SCHEDULE_STATISTICS_H
#define VISUALIZER_SCHEDULE_STATISTICS_H

#include <map>
#include <string>
#include <vector>
#include <stdint.h>
#include "Visualizer.h"

#define NUMBER_OF_PARAMETERS 17

enum Parameter {
	ACTIVITY_ID = 0, ACTIVITY_START_TIME = 1, ACTIVITY_DURATION = 2, NUMBER_OF_DIRECT_SUCCESSORS = 3,
       	NUMBER_OF_DIRECT_PREDECESSORS = 4, NUMBER_OF_ALL_SUCCESSORS = 5, NUMBER_OF_ALL_PREDECESSORS = 6,
       	SUM_OF_DIRECT_SUCCESSORS_FLOWS = 7, SUM_OF_SUCCESSORS_FLOWS = 8, NUMBER_OF_USED_RESOURCES = 9,
       	SUM_OF_REQUIREMENTS = 10, SUM_OF_WEIGHTED_REQUIREMENTS = 11, SUM_OF_DIRECT_SUCCESSORS_REQUIREMENTS = 12, 
	SUM_OF_ALL_SUCCESSORS_REQUIREMENTS = 13, WEIGHTED_SUM_OF_DIRECT_SUCCESSORS_REQUIREMENTS = 14,
	WEIGHTED_SUM_OF_ALL_SUCCESSORS_REQUIREMENTS = 15, ACTIVITY_FREEDOM = 16
};

struct ParameterResult {
	std::string description;
	std::vector<double> parameterValues;
}; 

struct InstanceResults	{
	uint32_t numberOfColumns;
	std::map<Parameter, ParameterResult> results;
};

class VisualizerScheduleStatistics : public Visualizer {
	 public:

		VisualizerScheduleStatistics(const std::string& file) : Visualizer(file) { };
		void writeScheduleStatistics(const std::string& file);
		InstanceResults getScheduleAnalysisData() const { return savedAnalysis; }
		static void writeGroupStatistics(const std::string& file, const std::vector<InstanceResults>& results);
		
	private:

		std::vector<std::string> getScheduleAnalysisLatexBody(std::string instanceTitle);
		std::vector<std::string> getScheduleAnalysisLatexBodyPart(const Parameter& parameter, uint32_t yShift);
		static std::vector<std::string> getGroupAnalysisLatexBody(const std::string& title, std::vector<InstanceResults> results);

		template <class T>
		static std::vector<T> getValuesInOrder(const std::vector<T>& values, const uint32_t * const& order);
		template <class T>
		static std::vector<std::string> getScheduleAnalysisLatexBodyElement(const T& values, const char* color, uint32_t yShift);
		template <class T>
		static std::vector<double> normalizeValues(const std::vector<T>& values);

		std::vector<uint32_t> getNumberOfAllSuccessors() const;
		std::vector<uint32_t> getNumberOfAllPredecessors() const;
		std::vector<uint32_t> getAllRelatedActivities(uint32_t activityId, uint32_t *numberOfRelated, uint32_t **related) const;
		uint32_t getNumberOfAllRelatedActivities(uint32_t activityId, uint32_t *numberOfRelated, uint32_t **related) const;
		std::vector<uint32_t> getSumOfSuccessorsFlowsDirect() const;
		std::vector<uint32_t> getSumOfSuccessorsFlowsAll() const;
		std::vector<uint32_t> getNumberOfUsedResources() const;
		std::vector<uint32_t> getSumOfActivitiesRequirements() const;
		std::vector<uint32_t> getSumOfWeightedActivitiesRequirements() const;
		std::vector<uint32_t> getSumOfDirectSuccessorsActivitiesRequirements(bool weighted = false) const;
		std::vector<uint32_t> getSumOfAllSuccessorsActivitiesRequirements(bool weighted = false) const;
		std::vector<double> getActivitiesFreedom() const;
		
		InstanceResults savedAnalysis;
};

#endif
