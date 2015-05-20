#include "udpclient.h"
#include "network.h"
#include "util.h"

enum State {
	RECV_TEXT,
	RECV_ACK
};

Udpclient client;
char buff[MAXBUFF];
State state = RECV_TEXT;

void handler_recv_mux(int, struct sockaddr_in, void*, size_t);
void func_display_text(void *buff, size_t buffsize);
std::string format(std::string line);

void
func_display_text(void *buff, size_t buffsize){
	char *ch_buff = (char*)buff;
	ch_buff[buffsize] = '\0';

	std::cout << ch_buff;
}

void 
handler_recv_mux(int sockfd, struct sockaddr_in addr, void *buff, size_t buffsize){
	switch(state){
		case RECV_TEXT:
			func_display_text(buff, buffsize);
			client.send_ack();
			break;
		default:
			break;
	}
}

std::string format(std::string line){
	std::stringstream iss(line);
	std::stringstream oss;

	std::string token;
	while(iss >> token){
		oss << token << "\n";
	}

	return oss.str();
}

int
main(int argc, char *argv[]){
	if(argc < 3){
		std::cout << "Usage: cli [IP Address] [Port]" << std::endl;
		return 0;
	}

	unsigned short port = atoi(argv[2]);

	try{
		client = Udpclient(argv[1], port);

		struct pollfd pfds[2];
		pfds[0].fd = fileno(stdin); pfds[0].events = POLLRDNORM;
		pfds[1].fd = client.getSockfd(); pfds[1].events = POLLRDNORM;

		int nready = 2;
		client.send_msg("RT\n");
		for(;;){
			poll(pfds, nready, -1);

			// Incoming message from server
			if(pfds[1].revents & (POLLRDNORM | POLLERR)){
				client.receive_msg(buff, MAXBUFF, handler_recv_mux);
			}

			// User input
			if(pfds[0].revents & POLLRDNORM){
				try{
					std::string line;
					getline(std::cin, line);
					client.send_msg(format(line).c_str());

				}catch(const char *err_msg){
					std::cerr << err_msg << std::endl;
				}
			}
		}

		client.shutdown();
	}catch(const char *err_msg){
		std::cerr << err_msg << std::endl;
	}

	return 0;
}