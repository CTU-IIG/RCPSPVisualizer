#include <algorithm>
#include <string>
#include <sstream>
#include <stdexcept>
#include <vector>
#include <utility>
#include <stdint.h>
#include "BlocksPlacerGLPK.h"
#include "DefaultParameters.h"
#include "VisualizerResourceUtilization.h"

#ifdef __GNUC__
#include <signal.h>
#include <unistd.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#endif

using namespace std;

void VisualizerResourceUtilization::writeResourceUtilization(const string& file)  const	{
	vector<string> doc = composeCompleteLatexDocument(getLatexTikzHeader(), getTikzStyle(), getResourceUtilizationLatexBody1(), 0.9);
	writeLinesToFile(file, doc);
}

void VisualizerResourceUtilization::writeResourceUtilizationILP(const string& file)  const	{
	/* TRY TO GET LATEX SOURCE */
	vector<string> doc;
	try {
		doc = composeCompleteLatexDocument(getLatexTikzHeader(), getTikzStyle(), getResourceUtilizationLatexBody2(), 0.8);
	} catch (exception& e)	{
		// Probably ILP isn't able to solve blocks positions.
		throw runtime_error(e.what());
	}
	writeLinesToFile(file, doc);
}

vector<string> VisualizerResourceUtilization::getResourceUtilizationLatexBody1() const	{

	vector<string> latexBody;
	vector<uint32_t> axisOrigins = getAxisShiftY();
	stringstream line(stringstream::in | stringstream::out);

	for (uint32_t r = 0; r < numberOfResources; ++r)	{
		uint32_t axisOriginY = axisOrigins[r];

		vector<string> axisSource = getResourceAxisLatexSource(r, axisOriginY);
		latexBody.insert(latexBody.end(), axisSource.begin(), axisSource.end());

		uint32_t * resourceUtilization = new uint32_t[projectMakespan];
		fill(resourceUtilization, resourceUtilization+projectMakespan, 0);
		for (uint32_t t = 0; t < projectMakespan; ++t)	{
			vector<uint32_t> activeActivities;
			for (uint32_t i = 0; i < numberOfActivities; ++i)	{
				if (startTimesById[i] <= t && t < startTimesById[i]+activitiesDuration[i] && requiredResources[i][r] > 0)
					activeActivities.push_back(i);
			}
			
			if (!activeActivities.empty())	{
				double x1 = t, x2 = t+1.;
				double mx = (x2-x1)/2.+t;
				for (vector<uint32_t>::const_iterator it = activeActivities.begin(); it != activeActivities.end(); ++it)	{
					uint32_t activityId = *it;
					uint32_t activityRequirement = requiredResources[activityId][r];
					double y1 = resourceUtilization[t], y2 = resourceUtilization[t]+activityRequirement;
					line<<"\t\t\\draw [fill="<<colors[(activityId % (NUMBER_OF_COLORS-2))+2]<<"!30]";
				        line<<" ("<<x1<<","<<y1+axisOriginY<<") rectangle ("<<x2<<","<<y2+axisOriginY<<");";
					latexBody.push_back(extractLine(line));
					double my = (y2-y1)/2.+resourceUtilization[t];
					line<<"\t\t\\draw ("<<mx<<","<<my+axisOriginY<<") node {\\bf\\large "<<activityId<<"};";
					latexBody.push_back(extractLine(line));

					resourceUtilization[t] += activityRequirement;
				}
			}
		}

		delete[] resourceUtilization;
	}

	return latexBody;
}

#ifdef __GNUC__
volatile bool unableToSolveILP;
void processError(int signalNumber)	{
	unableToSolveILP = true;
}
#endif

vector<string> VisualizerResourceUtilization::getResourceUtilizationLatexBody2() const	{

	vector<uint32_t> axisOrigins = getAxisShiftY();
	stringstream line(stringstream::in | stringstream::out);

	vector<vector<pair<uint32_t, uint32_t> > > blockPositions;
	#ifdef __GNUC__
	vector<pid_t> childs;
	unableToSolveILP = false;
	pid_t parentPid = getpid();
	// User handler - receive info about ILP error.
	signal(SIGUSR1, processError);
	// Allocate inter-process shared memory.
	uint32_t *sharedMemory = (uint32_t*) mmap(NULL, (2*numberOfActivities*numberOfResources+numberOfResources)*sizeof(uint32_t),
			PROT_READ | PROT_WRITE, MAP_SHARED | MAP_ANONYMOUS, -1, 0);
	for (uint32_t r = 0; r < numberOfResources; ++r)	{
		pid_t pid = fork();
		if (pid == 0)	{
			// Child process.
			try {
				vector<pair<uint32_t,uint32_t> > yOffsets;

				// Solve ILP problem.
				BlocksPlacerGLPK solver(numberOfActivities, r, startTimesById, activitiesDuration,
						resourceCapacities, requiredResources);
				yOffsets = solver.solve();

				// Write result to inter-process shared memory.
				uint32_t idx = 2*r*numberOfActivities;
				for (uint32_t i = 0; i < yOffsets.size(); ++i)	{
					sharedMemory[idx++] = yOffsets[i].first;
					sharedMemory[idx++] = yOffsets[i].second;
				}
				sharedMemory[2*numberOfActivities*numberOfResources+r] = 2*yOffsets.size();

				_exit(EXIT_SUCCESS);
			} catch (exception& e)	{
				// Send error signal to parent.
				cerr<<"VisualizerResourceUtilization::getResourceUtilizationLatexBody2: "<<e.what()<<endl;
				kill(parentPid, SIGUSR1);
				_exit(EXIT_FAILURE);
			}
		} else if (pid > 0)	{
			// Parent process.
			childs.push_back(pid);
		} else {
			// New child cannot be created.
			for (vector<pid_t>::const_iterator it = childs.begin(); it != childs.end(); ++it)
				kill(*it, SIGKILL);
			munmap(sharedMemory, (2*numberOfActivities*numberOfResources+numberOfResources)*sizeof(uint32_t));
			throw runtime_error("VisualizerResourceUtilization::getResourceUtilizationLatexBody2: Cannot create new process!");
		}
	}

	for (uint32_t i = 0; i < childs.size(); ++i)	{
		pid_t ret;
		int childStatus;
		do {
			usleep(10000);
			ret = waitpid(childs[i], &childStatus, WNOHANG);
		} while (ret <= 0 && !unableToSolveILP);

		if (unableToSolveILP)	{
			for (uint32_t k = (ret > 0 ? i+1 : i); k < childs.size(); ++k)
				kill(childs[k], SIGQUIT);
			munmap(sharedMemory, (2*numberOfActivities*numberOfResources+numberOfResources)*sizeof(uint32_t));
			throw runtime_error("VisualizerResourceUtilization::getResourceUtilizationLatexBody2: Invalid to solve ILP problem!");
		}
	}

	for (uint32_t r = 0; r < numberOfResources; ++r)	{
		vector<pair<uint32_t,uint32_t> > yOffsets;
		uint32_t offset = 2*r*numberOfActivities;
		uint32_t numberOfElements = sharedMemory[2*numberOfActivities*numberOfResources+r];
		for (uint32_t i = offset; i < offset+numberOfElements; i += 2)
			yOffsets.push_back(pair<uint32_t, uint32_t>(sharedMemory[i], sharedMemory[i+1]));
		blockPositions.push_back(yOffsets);
	}

	// Free allocated shared memory.
	munmap(sharedMemory, (2*numberOfActivities*numberOfResources+numberOfResources)*sizeof(uint32_t));
	#else
	for (uint32_t r = 0; r < numberOfResources; ++r)	{
		BlocksPlacerGLPK solver(numberOfActivities, r, startTimesById, activitiesDuration,
			       	resourceCapacities, requiredResources);
		blockPositions.push_back(solver.solve());
	}
	#endif

	vector<string> latexBody;
	for (uint32_t r = 0; r < numberOfResources; ++r)	{
		uint32_t axisOriginY = axisOrigins[r];
		vector<string> axisSource = getResourceAxisLatexSource(r, axisOriginY);
		latexBody.insert(latexBody.end(), axisSource.begin(), axisSource.end());

		/* PLACE ACTIVITIES TO SCHEDULE */
		latexBody.push_back("\t\t%% Draw activities blocks.");
		for (uint32_t i = 0; i < blockPositions[r].size(); ++i)	{
			uint32_t id = blockPositions[r][i].first;
			uint32_t x1 = startTimesById[id];
			uint32_t y1 = blockPositions[r][i].second+axisOriginY;
			uint32_t x2 = x1+activitiesDuration[id];
			uint32_t y2 = y1+requiredResources[id][r];
			double mx = ((double) x1+x2)/2.;
			double my = ((double) y1+y2)/2.;

			line<<"\t\t\\draw[blockstyle] ("<<x1<<","<<y1<<") rectangle ("<<x2<<","<<y2<<");";
			latexBody.push_back(extractLine(line));
			line<<"\t\t\\draw ("<<mx<<","<<my<<") node {\\bf\\large "<<id<<"};";
			latexBody.push_back(extractLine(line));
		}

		latexBody.push_back("");
	}

	return latexBody;
}

vector<uint32_t> VisualizerResourceUtilization::getAxisShiftY() const	{
	double spaceBetweenResources = ConfigureResourceUtilization::spaceBetweenResources;

	uint32_t y = 0;
	vector<uint32_t> axisOrigins;
	for (uint32_t r = 0; r < numberOfResources; ++r)	{
		axisOrigins.push_back(y);
		y += spaceBetweenResources+resourceCapacities[r];
	}
	return axisOrigins;
}

vector<string> VisualizerResourceUtilization::getResourceAxisLatexSource(const uint32_t& resourceId, const uint32_t& axisOriginY)	const	{
	double axisElongationX = ConfigureResourceUtilization::axisElongationX;
	double axisElongationY = ConfigureResourceUtilization::axisElongationY;
	double textDist = ConfigureResourceUtilization::textDist;
	double ticsLength = ConfigureResourceUtilization::ticsLength;

	vector<string> axisLatexSource;
	stringstream line(stringstream::in | stringstream::out);
	uint32_t resourceCapacity = resourceCapacities[resourceId];

	/* LATEX SOURCE - START INFO */
	line<<"\t\t%% RESOURCE "<<resourceId<<" LATEX CODE."<<endl;
	axisLatexSource.push_back(extractLine(line));

	/* AXIS X */
	axisLatexSource.push_back("\t\t%% Draw X axis.");
	line<<"\t\t\\draw[axisline] (0,"<<axisOriginY<<") -- ";
	line<<"("<<projectMakespan*axisElongationX<<","<<axisOriginY<<")";
	line<<" node [below="<<textDist<<"] {$t\\,[s]$};"<<endl;
	axisLatexSource.push_back(extractLine(line));

	line<<"\t\t\\foreach \\x in {0,...,"<<projectMakespan<<"}";
	axisLatexSource.push_back(extractLine(line));
	line<<"\t\t\t\\draw (\\x,"<<axisOriginY+ticsLength<<") -- ";
	line<<"(\\x,"<<axisOriginY-ticsLength<<") node [below] {\\x};"<<endl;
	axisLatexSource.push_back(extractLine(line));

	/* AXIS Y */
	axisLatexSource.push_back("\t\t%% Draw Y axis.");
	line<<"\t\t\\draw[axisline] (0,"<<axisOriginY<<") -- ";
	line<<"(0,"<<axisOriginY+axisElongationY*resourceCapacity<<") ";
	line<<"node [left="<<textDist<<"] {$R_"<<resourceId<<"$};"<<endl;
	axisLatexSource.push_back(extractLine(line));

	line<<"\t\t\\foreach \\y in {0,...,"<<resourceCapacity<<"}";
	axisLatexSource.push_back(extractLine(line));
	line<<"\t\t\t\\draw ("<<ticsLength<<",\\y+"<<axisOriginY<<") -- ("<<-ticsLength<<",\\y+"<<axisOriginY<<") ";
	line<<"node [left] {\\y};"<<endl;
	axisLatexSource.push_back(extractLine(line));

	/* MAX CAPACITY LINE */
	axisLatexSource.push_back("\t\t%% Draw max capacity label.");
	line<<"\t\t\\draw[dashed,color=red] (0,"<<axisOriginY+resourceCapacity<<") -- ";
	line<<"("<<projectMakespan<<","<<axisOriginY+resourceCapacity<<");";
	axisLatexSource.push_back(extractLine(line));
	line<<"\t\t\\draw ("<<projectMakespan<<","<<axisOriginY+resourceCapacity<<") ";
	line<<"node [above] {$\\textrm{Maximal capacity}$};"<<endl;
	axisLatexSource.push_back(extractLine(line));

	return axisLatexSource;
} 

