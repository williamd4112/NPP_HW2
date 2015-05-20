#include "util.h"

std::vector<std::string> Util::parse(char *buff, char sep){
	std::stringstream ss(buff);
	std::string token;
	std::vector<std::string> tokens;

	while(std::getline(ss, token, sep)){
		tokens.push_back(token);
	}
	return tokens;
}

std::vector<std::string> Util::parse(char *buff){
	return parse(buff, '\n');
}