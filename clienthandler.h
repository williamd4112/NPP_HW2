/**
	Clienthandler: used to handle client instruction in finite state machine form

	Instance Variables:
		struct sockaddr_in addr;
		string id;
		string pwd;
		enum State state;
**/

#ifndef _CLIENTHANDLER_H_
#define _CLIENTHANDLER_H_

#include "network.h"
#include "udpserver.h"
#include "bbs.h"
#include "article.h"
#include <string>
#include <map>
#include <stack>

#define EID_DISPLAYARTICLEERROR 0x1
#define EID_RESPONSEERROR 0x2

extern long long MAXARTICLEID;

class Clienthandler;
typedef std::map<SIN, Clienthandler>::iterator ClihanlderIterator;

class Clienthandler {
public:
	Clienthandler(Udpserver&, std::map<SIN, Clienthandler>&, SIN, std::string&, std::string&);
	~Clienthandler();
	std::string getId();
	std::string getPwd();
	port_t getPort();
	std::string getAddr_p();
	SIN& getAddr();
	Position getPos();
	State getState();

	void enter_article(std::string& aid){
		cur_aid = aid;
	}

	void exit_article(){
		cur_aid = "-1";
	}

	void moveTo(Position new_pos);
	void back();
	void translate(State new_state);
	void statehandler_recv_text(std::vector<std::string>);
	void statehandler_recv_article(void *buff, size_t buffsize);
	void func_display_mainmenu();
	void func_display_users();
	void func_display_article_list();
	void func_display_article(std::string& aid);
	void func_add_article(std::string& aid);
	void func_response(std::string& response);
	void func_add_blacklist(std::string& bid);
	void func_del_blacklist(std::string& bid);
	void func_yell(std::string& msg);
	void func_tell(std::string& id, std::string& msg);
	void func_logout();

	bool operator ==(Clienthandler);
private:
	Article* article_ptr;
	Udpserver& server; // Udpserver ref, transmission interface
	std::map<SIN, Clienthandler>& clihandlers_map;
	std::stack<Position> pos_st;
	Position pos;
	State state;
	std::string cur_aid;
	std::string id;
	std::string pwd;
	SIN src_addr;

	long getArticleID();
};

#endif