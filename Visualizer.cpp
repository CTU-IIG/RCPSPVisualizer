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
#include <iostream>
#include <fstream>
#include <iterator>
#include <stdexcept>
#include <sstream>
#include <vector>
#include "DefaultParameters.h"
#include "Visualizer.h"

using namespace std;

const char* Visualizer::colors[NUMBER_OF_COLORS] = {
	"White", "White", "red", "green", "blue", "cyan", "magenta", "yellow", "Brown",
	"Apricot", "Orange", "Violet", "JungleGreen", "Aquamarine", "LimeGreen", "CarnationPink", "Purple"
};

Visualizer::Visualizer(const string& fileName)	: numberOfActivities(0), numberOfResources(0) {
	ifstream input(fileName.c_str(), ios::in | ios::binary);
	if (input.good())	{
	       	int32_t phase;
		bool error = false;

		if (input.read((char*) &numberOfActivities, sizeof(uint32_t)).fail() == true || numberOfActivities <= 2)	{
			error = true;
			phase = 0;
		}

		if (!error && (input.read((char*) &numberOfResources, sizeof(uint32_t)).fail() == true || numberOfResources < 1))	{
			error = true;
			phase = 1;
		}

		if (!error) activitiesDuration = new uint32_t[numberOfActivities];
		if (!error && input.read((char*) activitiesDuration, numberOfActivities*sizeof(uint32_t)).fail() == true)	{
			error = true;
			phase = 2;
		}

		if (!error) resourceCapacities = new uint32_t[numberOfResources];
		if (!error && input.read((char*) resourceCapacities, numberOfResources*sizeof(uint32_t)).fail() == true)	{
			error = true;
			phase = 3;
		}

		if (!error)	{
			requiredResources = new uint32_t*[numberOfActivities];
			for (uint32_t i = 0; i < numberOfActivities; ++i)	{
				requiredResources[i] = new uint32_t[numberOfResources];
				if (!error && input.read((char*) requiredResources[i], numberOfResources*sizeof(uint32_t)).fail() == true) {
					error = true;
					phase = 4;
				}
			}
		}

		if (!error) numberOfSuccessors = new uint32_t[numberOfActivities];
		if (!error && input.read((char*) numberOfSuccessors, numberOfActivities*sizeof(uint32_t)).fail() == true)	{
			error = true;
			phase = 5;
		}

		if (!error)	{
			successors = new uint32_t*[numberOfActivities];
			for (uint32_t i = 0; i < numberOfActivities; ++i)	{
				successors[i] = new uint32_t[numberOfSuccessors[i]];
				if (!error && input.read((char*) successors[i], numberOfSuccessors[i]*sizeof(uint32_t)).fail() == true)	{
					error = true;
					phase = 6;
				}
			}
		}

		if (!error) numberOfPredecessors = new uint32_t[numberOfActivities];
		if (!error && input.read((char*) numberOfPredecessors, numberOfActivities*sizeof(uint32_t)).fail() == true)	{
			error = true;
			phase = 7;
		}

		if (!error)	{
			predecessors = new uint32_t*[numberOfActivities];
			for (uint32_t i = 0; i < numberOfActivities; ++i)	{
				predecessors[i] = new uint32_t[numberOfPredecessors[i]];
				if (!error && input.read((char*) predecessors[i], numberOfPredecessors[i]*sizeof(uint32_t)).fail() == true)	{
					error = true;
					phase = 8;
				}
			}
		}

		if (!error && input.read((char*) &projectMakespan, sizeof(uint32_t)).fail() == true)	{
			error = true;
			phase = 9;
		}

		if (!error) scheduleOrder = new uint32_t[numberOfActivities];
		if (!error && input.read((char*) scheduleOrder, numberOfActivities*sizeof(uint32_t)).fail() == true)	{
			error = true;
			phase = 10;
		}

		if (!error) startTimesById = new uint32_t[numberOfActivities];
		if (!error && input.read((char*) startTimesById, numberOfActivities*sizeof(uint32_t)).fail() == true)	{
			error = true;
			phase = 11;
		}

		input.close();

		if (error)	{
			string errorMessage = freeAllocatedResources(phase);
			throw runtime_error("Visualizer::Visualizer(const string&): "+errorMessage);
		}

		// It computes maxtrix of maximal distance (each-to-each). Floyd algorithm.
		distanceMatrix = new int32_t*[numberOfActivities];
		for (uint32_t i = 0; i < numberOfActivities; ++i)	{
			distanceMatrix[i] = new int32_t[numberOfActivities];
			fill(distanceMatrix[i], distanceMatrix[i]+numberOfActivities, -1);

			distanceMatrix[i][i] = 0;
			for (uint32_t j = 0; j < numberOfSuccessors[i]; ++j)	{
				distanceMatrix[i][successors[i][j]] = activitiesDuration[i];
			}
		}

		for (uint32_t k = 0; k < numberOfActivities; ++k)	{
			for (uint32_t i = 0; i < numberOfActivities; ++i)	{
				for (uint32_t j = 0; j < numberOfActivities; ++j)	{
					if (distanceMatrix[i][k] != -1 && distanceMatrix[k][j] != -1)	{
						if (distanceMatrix[i][j] != -1)	{
							if (distanceMatrix[i][k]+distanceMatrix[k][j] > distanceMatrix[i][j])
								distanceMatrix[i][j] = distanceMatrix[i][k]+distanceMatrix[k][j];
						} else	{
							distanceMatrix[i][j] = distanceMatrix[i][k]+distanceMatrix[k][j];
						}
					}
				}
			}
		}
	} else {
		throw invalid_argument("Visualizer::Visualizer(const string&): Cannot open the input file!");
	}
}

Visualizer::~Visualizer()	{
	freeAllocatedResources();
}

vector<string> Visualizer::getLatexTikzHeader() {
	vector<string> header;
	header.push_back("\\documentclass{article}");
	header.push_back("");
	header.push_back("\\usepackage[usenames,dvipsnames]{xcolor}");
	header.push_back("");
	header.push_back("\\usepackage{tikz}");
	header.push_back("\\usetikzlibrary{arrows,shapes,calc}");
	header.push_back("\\usepackage[active,tightpage]{preview}");
	header.push_back("\\PreviewEnvironment{tikzpicture}");
	header.push_back("");
	return header;
}

vector<string> Visualizer::getTikzStyle() {
	vector<string> style;
	style.push_back("\\tikzstyle{axisline} = [-triangle 45,thick,color=black]");
	style.push_back("\\tikzstyle{plainblockstyle} = [thick]");
	style.push_back(string("\\tikzstyle{blockstyle} = [fill="+string(ConfigureResourceUtilization::blockColor)+"!20, thick]"));
	style.push_back("");
	return style;
}

vector<string> Visualizer::envelopLatexBody(const vector<string>& body, double scale) {
	vector<string> doc;
	doc.push_back("\\begin{document}");
	stringstream line(stringstream::in | stringstream::out);
	line<<"\t\\begin{tikzpicture}[scale="<<scale<<"]";
	doc.push_back(extractLine(line));
	doc.insert(doc.end(), body.begin(), body.end());
	doc.push_back("\t\\end{tikzpicture}");
	doc.push_back("\\end{document}");
	return doc;
}

vector<string> Visualizer::composeCompleteLatexDocument(const vector<string>& header, const vector<string>& style, const vector<string>& body, double scale) {
	vector<string> document;
	document.insert(document.end(), header.begin(), header.end());
	document.insert(document.end(), style.begin(), style.end());
	vector<string> envelopedBody = envelopLatexBody(body, scale);
	document.insert(document.end(), envelopedBody.begin(), envelopedBody.end());
	return document;
}

string Visualizer::extractLine(stringstream& ss) {
	string ret = ss.str(); ss.str("");
	return ret;
}

void Visualizer::writeLinesToFile(const string& file, const vector<string>& doc) {
	ofstream output(file.c_str(), ios::out | ios::trunc);
	if (output.good())	{
		ostream_iterator<string> fit(output, "\n");
		copy(doc.begin(), doc.end(), fit);
		output.close();
	} else {
		throw invalid_argument("void Visualizer::writeLinesToFile(const string&, const vector<string>&): Cannot open the input file!");
	}
}

string Visualizer::freeAllocatedResources(const int32_t& phase)	{
	switch (phase)	{
		case -1:
			for (uint32_t i = 0; i < numberOfActivities; ++i)
				delete[] distanceMatrix[i];
			delete[] distanceMatrix;
		case 11:
			delete[] startTimesById;
		case 10: 
			delete[] scheduleOrder;
		case 9:
		case 8:
			for (uint32_t i = 0; i < numberOfActivities; ++i)
				delete[] predecessors[i];
			delete[] predecessors;
		case 7:
			delete[] numberOfPredecessors;
		case 6:
			for (uint32_t i = 0; i < numberOfActivities; ++i)
				delete[] successors[i];
			delete[] successors;
		case 5:
			delete[] numberOfSuccessors;
		case 4:
			for (uint32_t i = 0; i < numberOfActivities; ++i)
				delete[] requiredResources[i];
			delete[] requiredResources;
		case 3:
			delete[] resourceCapacities;
		case 2:
			delete[] activitiesDuration;
	}

	string errorMessage;
	switch (phase)	{
		case 0: errorMessage = "Cannot read variable 'numberOfActivities'!"; break;
		case 1: errorMessage = "Error while reading number of resources!"; break;
		case 2: errorMessage = "Unexpected error while reading array of activity durations!"; break;
		case 3: errorMessage = "The resource capacities is impossible to read from the input file!"; break;
		case 4: errorMessage = "Unexpected error while reading two-dimensional array of activities resource requirements!"; break;
		case 5: errorMessage = "Error while reading number of successors for each activity!"; break;
		case 6: errorMessage = "Cannot retrieve successors from the input file!"; break;
		case 7: errorMessage = "Error while reading number of predecessors for each activity!"; break;
		case 8: errorMessage = "Cannot retrieve predecessors from the input file!"; break;
		case 9: errorMessage = "Cannot read the project makespan!"; break;
		case 10: errorMessage = "Unexpected error while reading schedule order!"; break;
		case 11: errorMessage = "Cannot read the array of scheduled start time values!"; break;
	
		default: errorMessage = "No errors.";
	}

	return errorMessage;
}




