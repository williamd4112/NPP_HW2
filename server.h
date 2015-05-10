#ifndef _SERVER_H_
#define _SERVER_H_

#include "network.h"
#include "udpserver.h"
#include "bbs.h"
#include "article.h"
#include "clienthandler.h"
#include <dirent.h> 
#include <sstream>
#include <map>

extern long long MAXARTICLEID;

typedef std::pair<std::string, std::string> AccountRecord;

// C style ver
void fload_accounts(const char *filename);
void setMaxArticleID();

void mainloop();
void handler_echo_ser(int, struct sockaddr_in, void *buff, size_t buffsize);
void handler_entry(int, struct sockaddr_in, void *buff, size_t buffsize);

void func_guest_mux(int, struct sockaddr_in, void *buff, size_t buffsize);
void func_user_mux(int, struct sockaddr_in, void *buff, size_t buffsize);;
void func_login(struct sockaddr_in, std::string&, std::string&);
void func_register(std::string&, std::string&);
void func_display_entry(SIN *addr);

bool isLogined(SIN addr);
void setMaxartileID();

#endif