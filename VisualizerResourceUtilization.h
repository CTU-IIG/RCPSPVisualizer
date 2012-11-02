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

