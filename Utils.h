#pragma once

#include <utility>

namespace trading {	
	typedef std::pair<char*, char*> TStrTokResult;
	static void specializedStrtok(char* str, const char delim, TStrTokResult& result) {
		result.first = str;
		result.second = nullptr;

		if (nullptr != str) {
			for (; *str; ++str) {
				if (delim == *str) {
					*str = '\0';
					result.second = str;
					break;
				}
			}
		}
	}
}
