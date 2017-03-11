#pragma once

#include <string>
#include <list>

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
		trading::MatcherManager::Instance().clear();
	}
};