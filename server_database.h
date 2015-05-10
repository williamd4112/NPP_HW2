#ifndef _SERVER_DATABASE_H_
#define _SERVER_DATABASE_H_

#include <map>
#include "network.h"
#include "bbs.h"

class ServerDatabase{
public:
		ServerDatabase(std::string filename);
		~ServerDatabase();
		void insert(string&, string&);
private:
};

#endif