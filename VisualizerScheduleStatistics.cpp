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
#include <algorithm>
#include <cmath>
#include <map>
#include <numeric>
#include <iomanip>
#include <stdexcept>
#include <string>
#include <sstream>
#include <vector>
#include "DefaultParameters.h"
#include "VisualizerScheduleStatistics.h"

using namespace std;

void VisualizerScheduleStatistics::writeScheduleStatistics(const string& file)   {
	vector<string> doc = composeCompleteLatexDocument(getLatexTikzHeader(), getTikzStyle(), getScheduleAnalysisLatexBody(file), 0.6);
	writeLinesToFile(file, doc);
}

void VisualizerScheduleStatistics::writeGroupStatistics(const string& file, const vector<InstanceResults>& results)	{
	string title = file;
	replace(title.begin(), title.end(), '_', '-');

	vector<string> doc;
	vector<string> body = getGroupAnalysisLatexBody(title, results);
	doc = composeCompleteLatexDocument(getLatexTikzHeader(), getTikzStyle(), body, 0.8);
	writeLinesToFile(file, doc);
}

vector<string> VisualizerScheduleStatistics::getScheduleAnalysisLatexBody(string instanceTitle) {
	vector<string> latexBody;

	uint32_t boxSize = ConfigureScheduleAnalysis::boxSize;
	int32_t yShift = NUMBER_OF_PARAMETERS*boxSize; 
	int32_t pictureWidth = numberOfActivities*boxSize;

	stringstream line(stringstream::in | stringstream::out);
	replace(instanceTitle.begin(), instanceTitle.end(), '_', '-');
	line<<"\t\t\\draw ("<<pictureWidth/2.<<","<<yShift<<") node[above] {\\bf\\Large Visualized parameters - "<<instanceTitle<<"};";
	latexBody.push_back(extractLine(line));
	for (uint32_t p = 0; p < NUMBER_OF_PARAMETERS; ++p)	{
		yShift -= boxSize;
		vector<string> partOfBody = getScheduleAnalysisLatexBodyPart((Parameter) p, yShift);
		latexBody.insert(latexBody.end(), partOfBody.begin(), partOfBody.end());
		latexBody.push_back("");
	}
	savedAnalysis.numberOfColumns = numberOfActivities;
	
	return latexBody;
}

vector<string> VisualizerScheduleStatistics::getScheduleAnalysisLatexBodyPart(const Parameter& parameter, uint32_t yShift) {

	vector<string> partOfBody;
	string parameterDescription;
	ParameterResult parameterResults;
	uint32_t boxSize = ConfigureScheduleAnalysis::boxSize;
	stringstream line(stringstream::in | stringstream::out);

	switch (parameter)	{
		case ACTIVITY_ID:
			{
				vector<uint32_t> vectorActivitiesId(scheduleOrder, scheduleOrder+numberOfActivities);
				partOfBody = getScheduleAnalysisLatexBodyElement(vectorActivitiesId, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterDescription = "Order of activities";
			}
			break;
		case ACTIVITY_START_TIME:
			{
				vector<uint32_t> vectorStartTimeById(startTimesById, startTimesById+numberOfActivities);
				vectorStartTimeById = getValuesInOrder(vectorStartTimeById, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(vectorStartTimeById, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterDescription = "Activities start time";
			}
			break;
		case ACTIVITY_DURATION:
			{
				vector<uint32_t> vectorActivitiesDuration(activitiesDuration, activitiesDuration+numberOfActivities);
				vectorActivitiesDuration = getValuesInOrder(vectorActivitiesDuration, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(vectorActivitiesDuration, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(vectorActivitiesDuration);
				parameterDescription = "Activities duration";
			}
			break;
		case NUMBER_OF_DIRECT_SUCCESSORS:
			{
				vector<uint32_t> vectorNumberOfSuccessors(numberOfSuccessors, numberOfSuccessors+numberOfActivities);
				vectorNumberOfSuccessors = getValuesInOrder(vectorNumberOfSuccessors, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(vectorNumberOfSuccessors, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(vectorNumberOfSuccessors);
				parameterDescription = "Number of direct successors";
			}
			break;
		case NUMBER_OF_DIRECT_PREDECESSORS:
			{
				vector<uint32_t> vectorNumberOfPredecessors(numberOfPredecessors, numberOfPredecessors+numberOfActivities);
				vectorNumberOfPredecessors = getValuesInOrder(vectorNumberOfPredecessors, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(vectorNumberOfPredecessors, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(vectorNumberOfPredecessors);
				parameterDescription = "Number of direct predecessors";
			}
			break;
		case NUMBER_OF_ALL_SUCCESSORS:
			{
				vector<uint32_t> allSuccessors = getNumberOfAllSuccessors();
				allSuccessors = getValuesInOrder(allSuccessors, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(allSuccessors, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(allSuccessors);
				parameterDescription = "Number of successors";
			}
			break;
		case NUMBER_OF_ALL_PREDECESSORS:
			{
				vector<uint32_t> allPredecessors = getNumberOfAllPredecessors();
				allPredecessors = getValuesInOrder(allPredecessors, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(allPredecessors, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(allPredecessors);
				parameterDescription = "Number of predecessors";
			}
			break;
		case SUM_OF_DIRECT_SUCCESSORS_FLOWS:
			{
				vector<uint32_t> sumOfDirectSuccessorsFlows = getSumOfSuccessorsFlowsDirect();
				sumOfDirectSuccessorsFlows = getValuesInOrder(sumOfDirectSuccessorsFlows, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(sumOfDirectSuccessorsFlows, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(sumOfDirectSuccessorsFlows);
				parameterDescription = "Sum of direct successors flows";
			}
			break;
		case SUM_OF_SUCCESSORS_FLOWS:
			{
				vector<uint32_t> sumOfSuccessorsFlows = getSumOfSuccessorsFlowsAll();
				sumOfSuccessorsFlows = getValuesInOrder(sumOfSuccessorsFlows, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(sumOfSuccessorsFlows, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(sumOfSuccessorsFlows);
				parameterDescription = "Sum of successors flows";
			}
			break;
		case NUMBER_OF_USED_RESOURCES:
			{
				vector<uint32_t> numberOfUsedResources = getNumberOfUsedResources();
				numberOfUsedResources = getValuesInOrder(numberOfUsedResources, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(numberOfUsedResources, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(numberOfUsedResources);
				parameterDescription = "Number of used resources:";
			}
			break;
		case SUM_OF_REQUIREMENTS:
			{
				vector<uint32_t> sumOfRequirements = getSumOfActivitiesRequirements();
				sumOfRequirements = getValuesInOrder(sumOfRequirements, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(sumOfRequirements, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(sumOfRequirements);
				parameterDescription = "Sum of activity requirements";
			}
			break;
		case SUM_OF_WEIGHTED_REQUIREMENTS:
			{
				vector<uint32_t> sumOfWeightedRequirements = getSumOfWeightedActivitiesRequirements();
				sumOfWeightedRequirements = getValuesInOrder(sumOfWeightedRequirements, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(sumOfWeightedRequirements, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(sumOfWeightedRequirements);
				parameterDescription = "Sum of weighted activity requirements";
			}
			break;
		case SUM_OF_DIRECT_SUCCESSORS_REQUIREMENTS:
			{
				vector<uint32_t> sumOfDirectSucReq = getSumOfDirectSuccessorsActivitiesRequirements();
				sumOfDirectSucReq = getValuesInOrder(sumOfDirectSucReq, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(sumOfDirectSucReq, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(sumOfDirectSucReq);
				parameterDescription = "Sum of direct successors requirements";
			}
			break;
		case SUM_OF_ALL_SUCCESSORS_REQUIREMENTS:
			{
				vector<uint32_t> sumOfSucReq = getSumOfAllSuccessorsActivitiesRequirements();
				sumOfSucReq = getValuesInOrder(sumOfSucReq, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(sumOfSucReq, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(sumOfSucReq);
				parameterDescription = "Sum of successors requirements";
			}
			break;
		case WEIGHTED_SUM_OF_DIRECT_SUCCESSORS_REQUIREMENTS:
			{
				vector<uint32_t> weightedSumOfDirSucReq = getSumOfDirectSuccessorsActivitiesRequirements(true);
				weightedSumOfDirSucReq = getValuesInOrder(weightedSumOfDirSucReq, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(weightedSumOfDirSucReq, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(weightedSumOfDirSucReq);
				parameterDescription = "Weighted sum of direct successors requirements";
			}
			break;
		case WEIGHTED_SUM_OF_ALL_SUCCESSORS_REQUIREMENTS:
			{
				vector<uint32_t> weightedSumOfSucReq = getSumOfAllSuccessorsActivitiesRequirements(true);
				weightedSumOfSucReq = getValuesInOrder(weightedSumOfSucReq, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(weightedSumOfSucReq, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(weightedSumOfSucReq);
				parameterDescription = "Weighted sum of successors requirements";
			}
			break;
		case ACTIVITY_FREEDOM:
			{
				vector<double> activitiesFreedom = getActivitiesFreedom();
				activitiesFreedom = getValuesInOrder(activitiesFreedom, scheduleOrder);
				partOfBody = getScheduleAnalysisLatexBodyElement(activitiesFreedom, colors[parameter % NUMBER_OF_COLORS], yShift);
				parameterResults.parameterValues = normalizeValues(activitiesFreedom);
				parameterDescription = "Activities freedom";
			}
			break;
		default:
			throw runtime_error("getScheduleAnalysisLatexBodyPart(const Param&, uint32_t): Unknown parameter!");
	}

	if (parameter != ACTIVITY_ID && parameter != ACTIVITY_START_TIME)	{
		parameterResults.description = parameterDescription;
		savedAnalysis.results[parameter] = parameterResults;
	}
	line<<"\t\t\\draw (0,"<<boxSize/2.+yShift<<") node[left] {\\bf "<<parameterDescription<<": };";
	partOfBody.insert(partOfBody.begin(), extractLine(line));

	return partOfBody;
}

template <class T>
vector<T> VisualizerScheduleStatistics::getValuesInOrder(const vector<T>& values, const uint32_t * const& order)	{
	vector<T> orderValues;
	for (uint32_t i = 0; i < values.size(); ++i)	{
		uint32_t activityId = order[i];
		orderValues.push_back(values[activityId]);
	}
	return orderValues;
}

template <class T>
vector<string> VisualizerScheduleStatistics::getScheduleAnalysisLatexBodyElement(const T& values, const char* color, uint32_t yShift)  {
	uint32_t boxSize = ConfigureScheduleAnalysis::boxSize;
	vector<string> latexBody;
	typename T::value_type minValue = *min_element(values.begin(), values.end());
	typename T::value_type maxValue = *max_element(values.begin(), values.end());
	double unitPitch = (maxValue-minValue)/100.;

	uint32_t x = 0;
	for (uint32_t i = 0; i < values.size(); ++i)	{
		uint32_t transparency = floor((values[i]-minValue)/unitPitch);
		stringstream line(stringstream::in | stringstream::out);
		line<<"\t\t\\draw[plainblockstyle, fill="<<color<<"!"<<transparency<<"] ("<<x<<","<<yShift<<") rectangle ";
		line<<"("<<x+boxSize<<","<<yShift+boxSize<<");";
		latexBody.push_back(extractLine(line));
		line<<"\t\t\\draw ("<<x+boxSize/2.<<","<<yShift+boxSize/2.<<") node {\\bf "<<fixed<<setprecision(2)<<values[i]<<"};";
		latexBody.push_back(extractLine(line));
		x += boxSize;
	}

	return latexBody;
}

vector<string> VisualizerScheduleStatistics::getGroupAnalysisLatexBody(const string& title, vector<InstanceResults> results)	{

	uint32_t boxSize = ConfigureScheduleAnalysis::boxSize;
	uint32_t averageWindow = ConfigureScheduleAnalysis::averageWindow;
	int32_t yShift = (NUMBER_OF_PARAMETERS-2)*(5*boxSize);

	for (vector<InstanceResults>::iterator it = results.begin(); it != results.end(); ++it)	{
		uint32_t numberOfColumns = it->numberOfColumns;
		for (uint32_t p = 2; p < NUMBER_OF_PARAMETERS; ++p)	{
			map<Parameter, ParameterResult>::iterator mit = it->results.find((Parameter) p);
			if (mit != it->results.end())	{
				vector<double> newValues;
				vector<double> values = mit->second.parameterValues;
				double sumValue = 0;
				uint32_t counter = 0;
				for (uint32_t i = 1; i < numberOfColumns; ++i)	{
					if (counter < averageWindow)	{
						sumValue += values[i];
						++counter;
					} else {
						double averageValue = sumValue/((double) averageWindow);
						newValues.push_back(averageValue);
						sumValue = values[i]; counter = 1;
					}
				}
				mit->second.parameterValues = newValues;
			}
		}
		it->numberOfColumns = (numberOfColumns-2)/averageWindow;
	}

	vector<string> latexBody;
	if (results.size() >= 2)	{
		uint32_t numberOfColumns = results[0].numberOfColumns;
		stringstream line(stringstream::in | stringstream::out);
		line<<"\t\t\\draw ("<<numberOfColumns*boxSize/2.<<","<<yShift<<") node[above] {\\bf\\Huge "<<title<<"};";
		latexBody.push_back(extractLine(line));
		for (uint32_t p = 2; p < NUMBER_OF_PARAMETERS; ++p)	{
			string parameterDescription;
			vector<double> meanValues, medianValues, deviationValues, variationValues;
			for (uint32_t i = 0; i < numberOfColumns; ++i)	{
				double columnSum = 0;
				vector<double> columnValues;
				for (vector<InstanceResults>::const_iterator it = results.begin(); it != results.end(); ++it)	{
					map<Parameter, ParameterResult>::const_iterator mit;
					if ((mit = it->results.find((Parameter) p)) != it->results.end())	{
						double value = mit->second.parameterValues[i];
						columnSum += value;
						columnValues.push_back(value);
						if (parameterDescription.empty())
							parameterDescription = mit->second.description;
					}
				}

				uint32_t numberOfRows = results.size();
				double mean = columnSum/((double) numberOfRows);
				sort(columnValues.begin(), columnValues.end());
				double median = 0;
			        if ((numberOfRows % 2) == 0)
					median = (columnValues[numberOfRows/2]+columnValues[numberOfRows/2-1])/2.;
				else
					median = columnValues[numberOfRows/2];

				double standartDeviation = 0;
				for (vector<InstanceResults>::const_iterator it = results.begin(); it != results.end(); ++it)	{
					map<Parameter, ParameterResult>::const_iterator mit;
					if ((mit = it->results.find((Parameter) p)) != it->results.end())	{
						double value = mit->second.parameterValues[i];
						standartDeviation += (value-mean)*(value-mean);
					}
				}
				standartDeviation /= (double) numberOfRows-1;
				standartDeviation = sqrt(standartDeviation);
				double variationCoefficient = 0;
				if (mean > 0)
					variationCoefficient = (standartDeviation/mean)*100;

				meanValues.push_back(mean);
				medianValues.push_back(median);
				deviationValues.push_back(standartDeviation);
				variationValues.push_back(variationCoefficient);
			}

			line<<"\t\t\\draw ("<<0<<","<<yShift<<") node[above] {\\bf\\large "<<parameterDescription<<"};"<<endl;
			latexBody.push_back(extractLine(line)); yShift -= boxSize;
			line<<"\t\t\\draw ("<<0<<","<<yShift+boxSize/2.<<") node[left] {\\bf mean:};"<<endl;
			latexBody.push_back(extractLine(line)); 
			vector<string> latexLines = getScheduleAnalysisLatexBodyElement(meanValues, colors[p % NUMBER_OF_COLORS], yShift);
			latexBody.insert(latexBody.end(), latexLines.begin(), latexLines.end()); yShift -= boxSize;
			line<<"\t\t\\draw ("<<0<<","<<yShift+boxSize/2.<<") node[left] {\\bf median:};"<<endl;
			latexBody.push_back(extractLine(line)); 
			latexLines = getScheduleAnalysisLatexBodyElement(medianValues, colors[p % NUMBER_OF_COLORS], yShift);
			latexBody.insert(latexBody.end(), latexLines.begin(), latexLines.end()); yShift -= boxSize;
			line<<"\t\t\\draw ("<<0<<","<<yShift+boxSize/2.<<") node[left] {\\bf standart deviation:};"<<endl;
			latexBody.push_back(extractLine(line)); 
			latexLines = getScheduleAnalysisLatexBodyElement(deviationValues, colors[p % NUMBER_OF_COLORS], yShift);
			latexBody.insert(latexBody.end(), latexLines.begin(), latexLines.end()); yShift -= boxSize;
			line<<"\t\t\\draw ("<<0<<","<<yShift+boxSize/2.<<") node[left] {\\bf variation coefficient:};"<<endl;
			latexBody.push_back(extractLine(line)); 
			latexLines = getScheduleAnalysisLatexBodyElement(variationValues, colors[p % NUMBER_OF_COLORS], yShift);
			latexBody.insert(latexBody.end(), latexLines.begin(), latexLines.end()); yShift -= boxSize;
		}
	}	else	{
		throw runtime_error("VisualizerScheduleStatistics::writeGroupStatistics: Insufficient data of results structure!");
	}
	
	return latexBody;
}

template <class T>
vector<double> VisualizerScheduleStatistics::normalizeValues(const vector<T>& values)	{
	vector<double> normalized;
	double maxValue = *max_element(values.begin(), values.end());
	for (typename vector<T>::const_iterator it = values.begin(); it != values.end(); ++it)
		normalized.push_back(((double) *it)/maxValue);
	return normalized;
}

vector<uint32_t> VisualizerScheduleStatistics::getNumberOfAllSuccessors() const	{
	vector<uint32_t> numberOfAllSuccessors;
	for (uint32_t i = 0; i < numberOfActivities; ++i)
		numberOfAllSuccessors.push_back(getNumberOfAllRelatedActivities(i, numberOfSuccessors, successors));
	return numberOfAllSuccessors;
}

vector<uint32_t> VisualizerScheduleStatistics::getNumberOfAllPredecessors() const	{
	vector<uint32_t> numberOfAllPredecessors;
	for (uint32_t i = 0; i < numberOfActivities; ++i)
		numberOfAllPredecessors.push_back(getNumberOfAllRelatedActivities(i, numberOfPredecessors, predecessors));
	return numberOfAllPredecessors;
}

vector<uint32_t> VisualizerScheduleStatistics::getAllRelatedActivities(uint32_t activityId, uint32_t *numberOfRelated, uint32_t **related) const	{
	vector<uint32_t> relatedActivities;
	bool *activitiesSet = new bool[numberOfActivities];
	fill(activitiesSet, activitiesSet+numberOfActivities, false);

	for (uint32_t j = 0; j < numberOfRelated[activityId]; ++j)	{
		activitiesSet[related[activityId][j]] = true;
		vector<uint32_t> indirectRelated = getAllRelatedActivities(related[activityId][j], numberOfRelated, related);
		for (vector<uint32_t>::const_iterator it = indirectRelated.begin(); it != indirectRelated.end(); ++it)
			activitiesSet[*it] = true;
	}

	for (uint32_t id = 0; id < numberOfActivities; ++id)	{
		if (activitiesSet[id] == true)
			relatedActivities.push_back(id);
	}
	
	delete[] activitiesSet;
	return relatedActivities;
}

uint32_t VisualizerScheduleStatistics::getNumberOfAllRelatedActivities(uint32_t activityId, uint32_t *numberOfRelated, uint32_t **related) const	{
	return getAllRelatedActivities(activityId, numberOfRelated, related).size();
}

vector<uint32_t> VisualizerScheduleStatistics::getSumOfSuccessorsFlowsDirect() const	{
	vector<uint32_t> sumOfSuccessorsFlowsDirect;
	for (uint32_t i = 0; i < numberOfActivities; ++i)	{
		uint32_t sumOfFlows = 0;
		for (uint32_t j = 0; j < numberOfSuccessors[i]; ++j)
			sumOfFlows += activitiesDuration[successors[i][j]];
		sumOfSuccessorsFlowsDirect.push_back(sumOfFlows);
	}
	return sumOfSuccessorsFlowsDirect;
}

vector<uint32_t> VisualizerScheduleStatistics::getSumOfSuccessorsFlowsAll() const	{
	vector<uint32_t> sumOfSuccessorsFlowsAll;
	for (uint32_t i = 0; i < numberOfActivities; ++i)	{
		uint32_t sumOfFlows = 0;
		vector<uint32_t> allSuccessors = getAllRelatedActivities(i, numberOfSuccessors, successors);
		for (vector<uint32_t>::const_iterator it = allSuccessors.begin(); it != allSuccessors.end(); ++it)
			sumOfFlows += activitiesDuration[*it];
		sumOfSuccessorsFlowsAll.push_back(sumOfFlows);
	}
	return sumOfSuccessorsFlowsAll;
}

vector<uint32_t> VisualizerScheduleStatistics::getNumberOfUsedResources() const	{
	vector<uint32_t> numberOfUsedResources;
	for (uint32_t i = 0; i < numberOfActivities; ++i)	{
		uint32_t usedResourcesCounter = 0;
		for (uint32_t r = 0; r < numberOfResources; ++r)	{
			if (requiredResources[i][r] > 0)
				++usedResourcesCounter;
		}
		numberOfUsedResources.push_back(usedResourcesCounter);
	}
	return numberOfUsedResources;
}

vector<uint32_t> VisualizerScheduleStatistics::getSumOfActivitiesRequirements() const	{
	vector<uint32_t> sumOfActivitiesRequirements;
	for (uint32_t i = 0; i < numberOfActivities; ++i)	{
		uint32_t sumOfRequirements = 0;
		for (uint32_t r = 0; r < numberOfResources; ++r)
			sumOfRequirements += requiredResources[i][r];
		sumOfActivitiesRequirements.push_back(sumOfRequirements);
	}
	return sumOfActivitiesRequirements;
}

vector<uint32_t> VisualizerScheduleStatistics::getSumOfWeightedActivitiesRequirements() const	{
	vector<uint32_t> sumOfWeightedActivitiesRequirements = getSumOfActivitiesRequirements();
	for (uint32_t i = 0; i < numberOfActivities; ++i)
		sumOfWeightedActivitiesRequirements[i] *= activitiesDuration[i];
	return sumOfWeightedActivitiesRequirements;
}

vector<uint32_t> VisualizerScheduleStatistics::getSumOfDirectSuccessorsActivitiesRequirements(bool weighted) const	{
	vector<uint32_t> sumOfDirectSuccessorsActivitiesRequirements;
	for (uint32_t i = 0; i < numberOfActivities; ++i)	{
		uint32_t sumOfRequirements = 0;
		for (uint32_t j = 0; j < numberOfSuccessors[i]; ++j)	{
			uint32_t successorSumReq = accumulate(requiredResources[successors[i][j]],
					requiredResources[successors[i][j]]+numberOfResources, 0);
			sumOfRequirements += successorSumReq*(weighted == true ? activitiesDuration[successors[i][j]] : 1);
		}
		sumOfDirectSuccessorsActivitiesRequirements.push_back(sumOfRequirements);
	}
	return sumOfDirectSuccessorsActivitiesRequirements;
}

vector<uint32_t> VisualizerScheduleStatistics::getSumOfAllSuccessorsActivitiesRequirements(bool weighted) const	{
	vector<uint32_t> sumOfAllSuccessorsActivitiesRequirements;
	for (uint32_t i = 0; i < numberOfActivities; ++i)	{
		uint32_t sumOfRequirements = 0;
		vector<uint32_t> allSuccessors = getAllRelatedActivities(i, numberOfSuccessors, successors);
		for (vector<uint32_t>::const_iterator it = allSuccessors.begin(); it != allSuccessors.end(); ++it)	{
			uint32_t successorSumReq = accumulate(requiredResources[*it], requiredResources[*it]+numberOfResources, 0);
			sumOfRequirements += successorSumReq*(weighted == true ? activitiesDuration[*it] : 1);
		}
		sumOfAllSuccessorsActivitiesRequirements.push_back(sumOfRequirements);
	}
	return sumOfAllSuccessorsActivitiesRequirements;
}

vector<double> VisualizerScheduleStatistics::getActivitiesFreedom() const	{
	vector<double> activitiesFreedom;

	double cpl = distanceMatrix[0][numberOfActivities-1];
	for (uint32_t i = 0; i < numberOfActivities; ++i)	{
		double l1 = distanceMatrix[0][i];
		double l2 = distanceMatrix[i][numberOfActivities-1];
		double freedomCoefficient = (cpl-l1-l2)/cpl;
		activitiesFreedom.push_back(freedomCoefficient);
	}

	return activitiesFreedom;
}
