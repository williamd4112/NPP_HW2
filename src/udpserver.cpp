#include "udpserver.h"

Udpserver::Udpserver(in_addr_t addr, port_t port){
	sockfd = socket(AF_INET, SOCK_DGRAM, 0);
	
	bzero(&servaddr, sizeof(servaddr));
	servaddr.sin_family = AF_INET;
	servaddr.sin_addr.s_addr = addr;
	servaddr.sin_port = htons(port);

	if(bind(sockfd, (SA*)&servaddr, sizeof(servaddr)) < 0)
		throw "Udpserver: failed to bind.";
#ifdef LOG 
	std::cout << "BBS Server online at " << port << std::endl;
#endif
}

Udpserver::Udpserver(){

}

Udpserver::~Udpserver(){

}

sockfd_t Udpserver::getSockfd(){
	return sockfd; 
}

bool Udpserver::send_msg(const char *buff, size_t buffsize, SIN* cliaddr){
	int wn = sendto(sockfd, buff, buffsize, 0, (SA*)cliaddr, sizeof(*cliaddr));
	if(wn < 0) {
		perror("Udpserver::send_msg(): send message error\n");
		return false;
	}
	return true;
}

bool Udpserver::send_msg(const char *buff, SIN* cliaddr){
	int wn = sendto(sockfd, buff, strlen(buff), 0, (SA*)cliaddr, sizeof(*cliaddr));
	if(wn < 0) {
		perror("Udpserver::send_msg(): send message error\n");
		return false;
	}
	return true;
}

bool Udpserver::send_msg(const std::string& str, SIN* cliaddr){
	return send_msg(str.c_str(), str.size(), cliaddr);
}

bool Udpserver::send_ack(SIN* cliaddr){
	return send_msg(&ACK, sizeof(ACK), cliaddr);
}

bool Udpserver::send_nak(SIN* cliaddr){
	return send_msg(&NAK, sizeof(NAK), cliaddr);
}

// void Udpserver::send_header(Fileheader header, SIN* cliaddr){
// 	int wn = sendto(sockfd, &header, sizeof(header), 0, (SA*)cliaddr, sizeof(*cliaddr));
// 	if(wn < 0) {
// 		perror("Udpserver::send_header(): send header error\n");
// 		return;
// 	}
// }

void Udpserver::shutdown(){
	close(sockfd);
#ifdef LOG 
	std::cout << "BBS Server offline" << std::endl;
#endif
}