#pragma once

#include <string>
#include <list>
#include <numeric>
#include <algorithm>
#include <sstream>

#include "Defs.h"
#include "MatcherManager.h"

typedef std::list<std::string> results_t;

class SimpleCross
{
public:
	results_t action(const std::string& line) {
		return trading::MatcherManager::Instance().action(line);
	}

public:
	~SimpleCross() {
		std::stringstream s;
		s.precision(2);		

		s << "Add/Cancel performance stats: " << std::endl;
		std::vector<double> performance = trading::MatcherManager::Instance()._performance;

		auto begin = performance.begin();
		auto end = performance.end();
		
		s << std::fixed << "count=" << performance.size() << std::endl;
		if (!performance.empty()) {
			auto result = std::minmax_element(begin, end);
			s << "min=" << *(result.first) << " microsecs" << std::endl;
			s << "max=" << *(result.second) << " microsecs" << std::endl;
			s << "mean=" << std::accumulate(begin, end, 0.0) / performance.size() << " microsecs" << std::endl;

			++begin;
			if (begin != end) {
				result = std::minmax_element(begin, end);
				s << "min (w/o first measurement)=" << *(result.first) << " microsecs" << std::endl;
				s << "max (w/o first measurement)=" << *(result.second) << " microsecs" << std::endl;
				s << "mean (w/o first measurement)=" << std::accumulate(begin, end, 0.0) / performance.size() << " microsecs" << std::endl;
			}

#ifdef __SimpleCrossPrintPerformanceVector__
			s << "+++++++ all measurements (comment out \n#define __SimpleCrossPrintPerformanceVector__  \nin Defs.h to disable print out): +++++++" << std::endl;
			for (auto i : performance)
				s << i << " ";			
#endif
		}

		std::cout << s.str() << std::endl;

		trading::MatcherManager::Instance().clear();
	}
};