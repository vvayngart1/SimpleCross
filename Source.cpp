#include <string>
#include <fstream>
#include <iostream>
#include <list>

#include "SimpleCross.h"

typedef std::list<std::string> results_t;

class TestMove {
public:
	TestMove(uint32_t value) {
		_value = new uint32_t(value);
		std::cout << "TestMove(uint32_t value) constructor called" << std::endl;
	}

	TestMove(const TestMove& rhs) {
		_value = rhs._value;
		std::cout << "TestMove(const TestMove& rhs) constructor called" << std::endl;
	}

	TestMove(TestMove&& rhs) {
		_value = rhs._value;
		rhs._value = nullptr;
		std::cout << "TestMove(TestMove&& rhs) constructor called" << std::endl;
	}

	uint32_t* _value;
};


TestMove createTestMove(uint32_t value) {
	TestMove o = value;

	return o;
}


int main(int argc, char **argv)
{
	TestMove o = std::move(createTestMove(8));

	return 0;







	if (1 == argc) {
		std::cout << "USAGE: <actions file> [<actions_expected>]" << std::endl;
		return -1;
	}
	
	results_t allResultsExpected;	

	SimpleCross scross;
	std::string line;	

	if (3 == argc) {
		std::ifstream resultsExpected(argv[2], std::ios::in);
		if (!resultsExpected) {
			std::cout << "ERROR: can open file: " << argv[2] << std::endl;
			return -1;
		}
		while (std::getline(resultsExpected, line))
			allResultsExpected.push_back(line);
	}

	std::ifstream actions(argv[1], std::ios::in);	
	if (!actions) {
		std::cout << "ERROR: can open file: " << argv[1] << std::endl;
		return -1;
	}
	while (std::getline(actions, line))
	{
		results_t results = scross.action(line);
		for (results_t::const_iterator it = results.begin(); it != results.end(); ++it) {
			std::cout << *it;
			if (3 == argc) {
				if (allResultsExpected.empty()) {
					std::cout << " - FAILED expected: <empty>" << std::endl;
					return -1;
				}

				if (*it != allResultsExpected.front()) {
					std::cout << " - FAILED expected: " << allResultsExpected.front() << std::endl;
					return -1;
				}

				allResultsExpected.pop_front();

				std::cout << " - PASSED";
			}

			std::cout << std::endl;
		}
	}

	if (!allResultsExpected.empty()) {
		for (results_t::const_iterator it = allResultsExpected.begin(); it != allResultsExpected.end(); ++it) {
			std::cout << "actual: <empty> - FAILED expected: " << *it << std::endl;
		}

		std::cout << "FAILED" << std::endl;
		return -1;
	}

	if (3 == argc)
		std::cout << "PASSED" << std::endl;	

	return 0;
}


