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
#include <iostream>
#include <set>
#include <stdexcept>
#include "BlocksPlacerGLPK.h"
#include "DefaultParameters.h"

using namespace std;

BlocksPlacerGLPK::BlocksPlacerGLPK(uint32_t numberOfActivities, uint32_t resourceId, uint32_t *startTimesById, uint32_t* activitiesDuration,
		uint32_t *capacityOfResources, uint32_t **requiredCap)	{

	/* FIND BLOCKS IN THE PROJECT */
	uint32_t resourceCapacity = capacityOfResources[resourceId];
	for (uint32_t id = 0; id < numberOfActivities; ++id)	{
		if (activitiesDuration[id] > 0 && requiredCap[id][resourceId] > 0)
			activities.push_back(id);
	}

	/* INITIALIZATION AND SETTING */
	mip = glp_create_prob();
	glp_set_prob_name(mip, "Resource utilization");
	glp_set_obj_dir(mip, GLP_MIN);

	glp_init_iocp(&settings);
	settings.presolve = GLP_ON;
	settings.fp_heur = GLP_ON;
	settings.msg_lev = ConfigureILPSolver::messageLevel;
	settings.br_tech = ConfigureILPSolver::branchingTechnique;
	settings.bt_tech = ConfigureILPSolver::backtrackingTechnique;

	settings.tm_lim = ConfigureILPSolver::timeLimit;
	settings.mip_gap = ConfigureILPSolver::mipGap;

	/* ADD EQUATIONS */
	uint32_t line = 1;
	set<uint64_t> equationsAdded;
	glp_add_cols(mip, activities.size());
	uint32_t binaryVariableIndex = activities.size()+1;

	for (uint32_t i = 0; i < activities.size(); ++i)	{

		uint32_t activityIdI = activities[i];
		uint32_t startI = startTimesById[activityIdI];
		uint32_t durationI = activitiesDuration[activityIdI];
		uint32_t requirementI = requiredCap[activityIdI][resourceId];

		for (uint32_t j = 0; j < activities.size(); ++j)	{

			uint32_t activityIdJ = activities[j];
			uint32_t startJ = startTimesById[activityIdJ];
			uint32_t durationJ = activitiesDuration[activityIdJ];
			uint32_t requirementJ = requiredCap[activityIdJ][resourceId];

			if (startI+durationI > startJ && startJ+durationJ > startI && i != j)	{
				uint64_t key = (activityIdI < activityIdJ ? activityIdI : activityIdJ);
				key <<= 32;
				key |= (activityIdI < activityIdJ ? activityIdJ : activityIdI);

				if (equationsAdded.find(key) != equationsAdded.end())
					continue;

				int32_t indices[4];
				double values[4];

				glp_add_rows(mip, 2);
				glp_add_cols(mip, 1);

				// y_i - y_x + UB*b_{i,x} >= r_{x,k}
				indices[1] = i+1;
				indices[2] = j+1;
				indices[3] = binaryVariableIndex;

				values[1] = 1;
				values[2] = -1;
				values[3] = resourceCapacity;

				glp_set_mat_row(mip, line, 3, indices, values);
				glp_set_row_bnds(mip, line++, GLP_LO, requirementJ, 0);

				// y_x - y_i - UB*b_{i,x} >= r_{i,k} - UB
				indices[1] = j+1;
				indices[2] = i+1;
				indices[3] = binaryVariableIndex;

				values[1] = 1;
				values[2] = -1;
				values[3] = -((int32_t) resourceCapacity);

				glp_set_mat_row(mip, line, 3, indices, values);
				glp_set_row_bnds(mip, line++, GLP_LO, ((int32_t) requirementI)-((int32_t) resourceCapacity), 0);
				
				equationsAdded.insert(key);
				++binaryVariableIndex;
			}
		}
	}

	/* SET VARIABLE TYPES AND BOUNDS */
	for (uint32_t i = 0; i < activities.size(); ++i)	{
		glp_set_col_kind(mip, i+1, GLP_IV);
		glp_set_obj_coef(mip, i+1, requiredCap[activities[i]][resourceId]*activitiesDuration[activities[i]]);
		// y_i <= R_k-r_{i,k}
		glp_set_col_bnds(mip, i+1, GLP_DB, 0, resourceCapacity-requiredCap[activities[i]][resourceId]);
	}

	for (uint32_t i = activities.size()+1; i < binaryVariableIndex; ++i)
		glp_set_col_kind(mip, i, GLP_BV);
}

vector<pair<uint32_t, uint32_t> > BlocksPlacerGLPK::solve(bool verbose)	{

	glp_long startTime = glp_time(), stopTime;
	vector<pair<uint32_t, uint32_t> > solution;

	if (verbose)	{
		clog<<string(40,'*')<<endl;
		uint32_t numberOfVariables = glp_get_num_cols(mip);
		uint32_t numberOfEquations = glp_get_num_rows(mip);
		clog<<"Number of binary variables: "<<numberOfVariables-activities.size()<<endl;
		clog<<"Number of integer variables: "<<activities.size()<<endl;
		clog<<"Total number of variables: "<<numberOfVariables<<endl;
		clog<<"Number of equations: "<<numberOfEquations<<endl;
	}

	/* SOLVE ILP */
	int retCode = glp_intopt(mip,&settings);
	if (retCode != 0 && retCode != GLP_ETMLIM && retCode != GLP_EMIPGAP)	{
		throw runtime_error("GLPKSolver::solve: "+getGlpkError(retCode));
	} else {
		int status = glp_mip_status(mip);
		if (status == GLP_OPT || status == GLP_FEAS)	{
			int criterion = glp_mip_obj_val(mip);
			for (uint32_t i = 0; i < activities.size(); ++i)	{
				uint32_t y = glp_mip_col_val(mip,i+1);
				solution.push_back(pair<uint32_t, uint32_t>(activities[i], y));
			}
			if (verbose)	{
				clog<<(status == GLP_OPT ? "Optimal " : "Feasible ")<<"solution was found!"<<endl;
				clog<<"Criterion: "<<criterion<<endl;
			}
		} else {
			string method = "GLPKSolver::solve: ";
			switch (status)	{
				case GLP_UNDEF:
					throw runtime_error(method+"MIP solution is undefined!");
				case GLP_NOFEAS:
					throw runtime_error(method+"Problem has no integer feasible solution.");
				default:
					throw runtime_error(method+"Unknow status code!");
			}
		}
	}

	stopTime = glp_time();
	double runTime = glp_difftime(stopTime,startTime);

	glp_long memUsage;
	glp_mem_usage(NULL,NULL,NULL,&memUsage);

	if (verbose)	{
		clog<<"ILP total time: "<<runTime<<" sec"<<endl;
		clog<<"Total memory usage: "<<memUsage.lo<<" bytes"<<endl;
		clog<<string(40,'#')<<endl;
	}

	return solution;
}

string BlocksPlacerGLPK::getGlpkError(int code) const	{
	string errorMessage;
	switch (code)	{
		case 0:
			errorMessage = "No errors.";
			break;
		case GLP_EBOUND:
			errorMessage = "Incorrect bounds of some integer variables!";
		       	break;
		case GLP_EROOT:
			errorMessage = "Cannot start to solve mip instance, because optimal LP solution wasn't provided!\n";
			errorMessage += "You should turn presolve option on...";
			break;
		case GLP_ENOPFS:
			errorMessage = "No feasible primal LP solution.";
			break;
		case GLP_ENODFS:
			errorMessage = "No feasible dual LP solution.";
			break;
		case GLP_EFAIL:
			errorMessage = "Solver failed to solve this instance. (prematurely terminated)";
			break;
		case GLP_ESTOP:
			errorMessage = "Solver was prematurely stopped by user!";
			break;
		default:
			errorMessage = "Unhandled error code ";
			errorMessage += code;
			errorMessage += " of glp_intopt function!";
			break;
	}

	return errorMessage;
}

BlocksPlacerGLPK::~BlocksPlacerGLPK()	{
	glp_delete_prob(mip);
	glp_free_env();
}

