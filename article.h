/**
	Article: 
	store the article content, attachement file will be recored in the string list.
	when server want to send article back to client, 
	server first to read "info" file in the article folder
	In this step, server checked the access ID (a.k.a client id) if in the blacklist
	if access id in the blacklist, server will return an "Empty Article" to Client
	(NOTE: Not echo failed message to client directly, but an "Empty Article".
		   Because when client send "Enter Article" command, the client will translate into "STATE_RECV_ARTICLE"
		   ,so client expected to recive an article not plain text)
	
	Article Folder:
	-info
	-text
	-attachments
		-file
		-file
		-file
		-....

	info format
	- ArticleID(string)
	- Article Title
	- Publisher ID(string)
	- Publisher Date(string)
	- Blacklist Count(int)
	- Blacklist (string[])
	- Attached Files Count(int)
	- Attached Files List(string[])

	text format
	-Text content

	Article will include the following data
	- Article ID(string)
	- Publisher ID(string)
	- Publish Date(string)
	- Permission(short)
	- Text Content(string)
	- Attached Files List(string[])

	Client will present a article by the permission
	e.g. a normal user will not see the [AB]dd/[DB]el ID in blacklist [D]elete options
	
**/

#ifndef _ARTICLE_H_H
#define _ARTICLE_H_H

#include <vector>
#include <queue>
#include <list>
#include <cstring>
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <sys/stat.h>
#include "bbs.h"

#define EID_CREATEARTICLE 0x1
#define EID_LOADRESPONSE 0x2
#define EID_FILEOPEN 0x3

extern std::string MSG_PERMISSION_DENIED;

class Article;
class Articleheader {
friend class Article;
public:
	Articleheader();
	Articleheader(const char *);
	~Articleheader();
	void store(const char *article_dir);
	std::string toString(); // Fixed length

private:
	std::string aid; //Article ID
	std::string title; // Title
	std::string pid; // Publisher ID
	std::string date; // Date
	std::vector<std::string> blacklist;
	std::vector<std::string> fileslist;

	void loadBlacklist(const char *article_dir);

};

class Article{
friend class Clienthandler;
public:
	Article();
	Article(const char *);
	Article(std::string& aid);
	Article(std::string aid, std::string title, std::string pid, std::string date);
	~Article();
	void init(const char *);
	void store();
	void store(const char *article_dir);
	void createBlacklist();
	void appendBlacklist(std::string& bid);
	void removefromBlacklist(std::string& bid);
	std::string getResponse();
	std::string& getPid();
	std::string toString(std::string vid); // pass Viewer ID to check if black

private:
	std::string article_dir;
	Articleheader header;
	std::stringstream context;
};

#endif