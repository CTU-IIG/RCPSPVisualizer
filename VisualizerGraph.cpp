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
	string graphOrientation = ConfigureGraph::graphOrientation;
	string standardNodeColor = ConfigureGraph::standardNodeColor;
	string criticalNodeColor = ConfigureGraph::criticalNodeColor;
	string startNodeColor = ConfigureGraph::startNodeColor, finishNodeColor = ConfigureGraph::finishNodeColor;
	int32_t criticalPathLength = distanceMatrix[0][numberOfActivities-1];

	stringstream line(stringstream::in | stringstream::out);

	dot.push_back("digraph G {");
	line<<"\tgraph[rankdir="<<graphOrientation<<"];"<<endl;
	dot.push_back(extractLine(line));
	dot.push_back("\tnode[style=filled, width=\"0.8\", height=\"0.8\", fixedsize=true, margin=\"0,0\"];");
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
