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
#ifndef VISUALIZER_GRAPH_H
#define VISUALIZER_GRAPH_H

#include <string>
#include <vector>
#include "Visualizer.h"

class VisualizerGraph : public Visualizer {
	public:

		VisualizerGraph(const std::string& file) : Visualizer(file) { };	  
		void writeInstanceGraph(const std::string& file) const;
		
	private:
	  
		std::vector<std::string> getGraphAtDotFormat() const;
};

#endif

