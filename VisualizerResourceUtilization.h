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
#ifndef VISUALIZER_RESOURCE_UTILIZATION_H
#define VISUALIZER_RESOURCE_UTILIZATION_H

#include <string>
#include <vector>
#include <stdint.h>
#include "Visualizer.h"

class VisualizerResourceUtilization : public Visualizer {
	public:
	  
		VisualizerResourceUtilization(const std::string& file) : Visualizer(file) { };
		void writeResourceUtilization(const std::string& file)	const;
		void writeResourceUtilizationILP(const std::string& file)	const;
		
	private:

		std::vector<std::string> getResourceUtilizationLatexBody1() const;
		std::vector<std::string> getResourceUtilizationLatexBody2() const;
		
		std::vector<uint32_t> getAxisShiftY() const;
		std::vector<std::string> getResourceAxisLatexSource(const uint32_t& resourceId, const uint32_t& axisOriginY)  const;
};

#endif

