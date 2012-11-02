#include <algorithm>
#include <cctype>
#include <cstdlib>
#include <iostream>
#include <map>
#include <sstream>
#include <stdexcept>
#include <string>
#include <vector>

#include "VisualizerGraph.h"
#include "VisualizerResourceUtilization.h"
#include "VisualizerScheduleStatistics.h"

using namespace std;

bool fileSystemSeperator(const char& c)	{
	if (c == '/' || c == '\\')
		return true;
	else
		return false;
}

int main(int argc, char* argv[])	{
	string outputDirectory;
	map<uint32_t, vector<InstanceResults> > analysisResults;
	for (int i = 1; i < argc; ++i)	{
		string filename = argv[i], baseName;
		string::reverse_iterator dotIter = find(filename.rbegin(), filename.rend(), '.');
		if (dotIter == filename.rend())	{
			baseName = filename;
		} else	{
			baseName = string(dotIter+1, filename.rend());
			reverse(baseName.begin(), baseName.end());
		}
		string graphFile = baseName+"_GH.dot";
		string resourceUtilizationFile = baseName+"_RU.tex";
		string resourceUtilizationILPFile = baseName+"_RU_ILP.tex";
		string instanceStatisticsFile = baseName+"_ST.tex";

		uint32_t instanceParameter = 0;
		for (uint32_t s = 0; s < baseName.size(); ++s)	{
			if (isdigit(baseName[s]))	{
				instanceParameter *= 10;
				instanceParameter += baseName[s]-'0';
			} else if (instanceParameter > 0)	{
				break;
			}
		}

		string::iterator slashIter = find_if(filename.begin(), filename.end(), fileSystemSeperator);
		if (slashIter != filename.end())
			outputDirectory = string(filename.begin(), slashIter+1);

		try {
			VisualizerGraph graphVisualizer(filename);
			graphVisualizer.writeInstanceGraph(graphFile);
			clog<<graphFile<<": Instance graph was created!"<<endl;

			VisualizerResourceUtilization resourceUtilizationVisualizer(filename);
			resourceUtilizationVisualizer.writeResourceUtilization(resourceUtilizationFile);
			clog<<resourceUtilizationFile<<": Utilization of resources was computed and transformed to Latex source!"<<endl;
			try {
				resourceUtilizationVisualizer.writeResourceUtilizationILP(resourceUtilizationILPFile);
				clog<<resourceUtilizationILPFile<<": Blocks version of resources load was created!"<<endl;
			} catch (exception& e)	{
				cerr<<resourceUtilizationFile<<": "<<e.what()<<endl;
			}

			VisualizerScheduleStatistics scheduleStatisticsVisualizer(filename);
			scheduleStatisticsVisualizer.writeScheduleStatistics(instanceStatisticsFile);
			analysisResults[instanceParameter].push_back(scheduleStatisticsVisualizer.getScheduleAnalysisData());
			clog<<instanceStatisticsFile<<": Various parameters were analysed in the schedule order!"<<endl;

		} catch (exception& e)	{
			cerr<<e.what()<<endl;
		}
	}

	vector<InstanceResults> allInstancesResults;
	for (map<uint32_t, vector<InstanceResults> >::const_iterator mit = analysisResults.begin(); mit != analysisResults.end(); ++mit)	{
		allInstancesResults.insert(allInstancesResults.end(), mit->second.begin(), mit->second.end());
		try	{
			stringstream line(stringstream::in | stringstream::out);
			line<<outputDirectory<<"stat_"<<mit->first<<".tex";
			VisualizerScheduleStatistics::writeGroupStatistics(line.str(), mit->second);
			clog<<line.str()<<": Group statistics was written!"<<endl;
		} catch (exception& e)	{
			cerr<<e.what()<<endl;
		}
	}

	try {
		string file = outputDirectory+"complete_stat.tex";
		VisualizerScheduleStatistics::writeGroupStatistics(file, allInstancesResults);
		clog<<file<<": Complete statistics of all read instances was written!"<<endl;
	} catch (exception& e)	{
		cerr<<e.what()<<endl;
	}

	return 0;
}
