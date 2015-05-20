#ifndef _UTIL_H_
#define _UTIL_H_

#include <cstring>
#include <iostream>
#include <sstream>
#include <vector>

namespace Util{
	enum Exception{
		ERR_PARSE_SIZE
	};

	std::vector<std::string> parse(char *buff, char sep);
	std::vector<std::string> parse(char *buff);
}


#endif