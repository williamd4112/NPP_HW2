#include "network.h"
#include "udpserver.h"
#include "BBSServer.h"

int
main (int argc, char *argv[]){
	if(argc != 2){
		std::cout << "Usage: server <port>" << std::endl;
		return 0;
	}

	try{
		unsigned short port = atoi(argv[1]);
		BBSServer server(htonl(INADDR_ANY), port);

		fd_set rset;
		FD_ZERO(&rset);
		sockfd_t maxfdpl = server.getSockfd() + 1;

		struct timeval timeout={0,0};
		int nready;
		for(;;){
			FD_SET(server.getSockfd(), &rset);
			if((nready = select(maxfdpl, &rset, NULL, NULL, &timeout)) < 0){
				if(errno == EINTR)
					continue;
				else{
					perror("main(): Select error\n");
					break;
				}
			}

			server.pollWaiting();
			if(FD_ISSET(server.getSockfd(), &rset)){
				server.recv();
			}
		}
		
	}
	catch(...){
		std::cerr << "BBSServer(): failed to create bbs server." << std::endl;
	}

	return 0;
}