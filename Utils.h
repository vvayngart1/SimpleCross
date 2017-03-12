#pragma once

#include <utility>

namespace trading {
	/*	
		implements strtok C function semantics and provides information
		to the modified delim character to enable it to be switched back
		from '\0' to delim character
	*/

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
