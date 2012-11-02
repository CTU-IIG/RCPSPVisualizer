#include <string>
#include <sstream>
#include <vector>
#include "DefaultParameters.h"
#include "VisualizerGraph.h"

using namespace std;

void VisualizerGraph::writeInstanceGraph(const string& file) const	{
	vector<string> doc = getGraphAtDotFormat();
	writeLinesToFile(file, doc);
}

vector<string> VisualizerGraph::getGraphAtDotFormat() const	{
	vector<string> dot;
	string standardNodeColor = ConfigureGraph::standardNodeColor;
	string criticalNodeColor = ConfigureGraph::criticalNodeColor;
	string startNodeColor = ConfigureGraph::startNodeColor, finishNodeColor = ConfigureGraph::finishNodeColor;
	int32_t criticalPathLength = distanceMatrix[0][numberOfActivities-1];

	stringstream line(stringstream::in | stringstream::out);

	dot.push_back("digraph G {");
	dot.push_back("\tnode [style=filled, width=\"0.8\", height=\"0.8\"];");
	for (uint32_t i = 0; i < numberOfActivities; ++i)	{
		if (i == 0)	{
			line<<"\t"<<i<<" [fillcolor="<<startNodeColor<<", shape=doublecircle, label=\""<<i<<"\\n"<<activitiesDuration[i]<<"\"];";
		} else if (i+1 == numberOfActivities)	{
			line<<"\t"<<i<<" [fillcolor="<<finishNodeColor<<", shape=doublecircle, label=\""<<i<<"\\n"<<activitiesDuration[i]<<"\"];";
		} else	{
			string nodeColor;
		        if (distanceMatrix[0][i]+distanceMatrix[i][numberOfActivities-1] == criticalPathLength)
				nodeColor = criticalNodeColor;
			else
				nodeColor = standardNodeColor;
			line<<"\t"<<i<<" [fillcolor="<<nodeColor<<", shape=circle, label=\""<<i<<"\\n"<<activitiesDuration[i]<<"\"];";
		}
		dot.push_back(extractLine(line));
	}
	dot.push_back("");

	for (uint32_t i = 0; i < numberOfActivities; ++i)	{
		for (uint32_t j = 0; j < numberOfSuccessors[i]; ++j)	{
			line<<"\t"<<i<<" -> "<<successors[i][j]<<";";
			dot.push_back(extractLine(line));
		}
	}
	dot.push_back("}");

	return dot;
}
