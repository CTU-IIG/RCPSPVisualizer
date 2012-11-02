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

