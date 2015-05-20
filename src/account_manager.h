#ifndef _ACCOUNT_MANAGER_H_
#define _ACCOUNT_MANAGER_H_

#include <iostream>
#include <fstream>
#include <cstdio>
#include <cstring>
#include <map>
#include <algorithm>

typedef std::map<std::string, std::string> AccountTable;
typedef std::pair<std::string, std::string> AccountRecord;

class AccountManager {
public:

	enum Exception{
		ERR_DUPID,
		ERR_IO,
		ERR_NULL,
		ERR_LOADDATA
	};

	AccountManager(const char *);
	AccountManager();
	~AccountManager();

	bool isExist(const char *);
	bool isExist(std::string);
	bool isCorrect(std::string id, std::string pwd);
	
	/*
	 * insert(id, pwd):
     * @throw ERR_IO, ERR_DUP
	 *
	 */
	void insert(const char*, const char*);
	void insert(std::string, std::string);
	void remove(const char *);
private:
	std::string account_filename;
	AccountTable account_table;

	void update();
	void update(const char *, const char *);
	void update(std::string, std::string);
	bool load(const char *);
	bool store(const char *);
	bool store(const char *, const char *, const char *);
};

#endif