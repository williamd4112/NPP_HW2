#ifndef _NETWORK_H_
#define _NETWORK_H_

#include <iostream>
#include <cstdio>
#include <cstring>
#include <cstdlib>
#include <vector>
#include <sstream>
#include <iterator>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/poll.h>
#include <netinet/in.h>
#include <netdb.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include <signal.h>
#include <dirent.h> 
#include <arpa/inet.h>
#include <termios.h>

#define MAXBUFF 65535
#define MAXLINE 1024
#define MAXCONTEXTSIZE 3000
#define IPV4_ADDRLEN 16

const char ACK = 0x1;
const char NAK = 0x0;

typedef unsigned short port_t;
typedef in_addr_t sin_addr;
typedef int sockfd_t;
typedef struct sockaddr SA;
typedef struct sockaddr_in SIN;
typedef void (*handle_func) (int, struct sockaddr_in, void *, size_t);

bool operator < (SIN a, SIN b);
char getch();

#endif