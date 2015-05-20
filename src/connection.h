#ifndef _CONNECTION_H_
#define _CONNECTION_H_

#include <map>
#include <algorithm>
#include <list>
#include <time.h>
#include "network.h"
#include "udpserver.h"

#define TIMEOUT 10

class Udpserver;
class ConnectionManager;

class Connection {
friend class BBSServer;
friend class ConnectionManager;
public:
	enum State{
		RECV_ACK,
		RECV_TEXT
	};

	enum Exception{

	};

	Connection(Udpserver& server, ConnectionManager& manager, SIN addr);
	~Connection();
	
	std::string getAddr_p();
	bool isTimeout();
	bool isLogin();
	void translate(Connection::State s);
	void save_state();
	void reset_timer();
	void send_msg(const char *msg);
	void send_buff();
	void retransmit();

	void setLogin(std::string _id){
		login_state = true;
		id = _id;
	}

	bool operator==(Connection conn){
		return (src_addr == conn.src_addr);
	}
private:
	Udpserver& parent;
	ConnectionManager& manager;
	char sendbuff[MAXBUFF];
	size_t buffsize;
	bool login_state;
	
	// Enable: transfer text, Disable: transfer file
	bool rdt_enable;

	Connection::State state, last_state;
	SIN src_addr;
	std::string id;

	time_t last_time;
};

typedef std::pair<SIN, Connection> ConnectionRecord;
class ConnectionManager {
friend class BBSServer;
public:
	enum Exception{
		KEY_NOFOUND,
		CONN_DUP
	};

	ConnectionManager(Udpserver& server);
	~ConnectionManager();

	bool isExist(SIN addr);

	/*
		getConnection():
		@param: addr as key
		@throw EID_NOTFOUND: no such key
	*/
	Connection& getConnection(SIN& addr);
	void addConnection(SIN& addr);
	void removeConnection(SIN& addr);
	void addWaiting(Connection& conn);
	void removeWaiting(Connection& conn);
	void pollWaiting();

private:
	Udpserver& parent;
	std::map<SIN, Connection> conn_map;
	std::list<Connection> waitting_conn;
};

#endif