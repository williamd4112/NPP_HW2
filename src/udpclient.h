/**
	Udpclient: a finite state machine, can send message to server and do some response when receive server message
	
**/

#ifndef _UDPCLIENT_H_
#define _UDPCLIENT_H_

#include "network.h"

typedef void (*handle_func) (int, struct sockaddr_in, void *, size_t);

class Udpclient{
public:
	Udpclient(char *addr_p, port_t port);
	Udpclient();
	~Udpclient();
	bool receive_msg(void*, size_t, handle_func);
	bool send_msg(const char*, size_t);
	bool send_msg(std::string& msg);
	bool send_msg(const char* msg);
	bool send_ack();
	void send_file(const char *filename);
	sockfd_t getSockfd();
	void shutdown();
private:
	char sendbuff[MAXBUFF];
	sockfd_t sockfd;
	port_t serv_port;
	SIN servaddr;
	socklen_t servlen;
};

#endif