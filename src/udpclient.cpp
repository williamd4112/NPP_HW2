#include "udpclient.h"

Udpclient::Udpclient(char *addr_p, port_t port){    
    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    inet_pton(AF_INET, addr_p, &servaddr.sin_addr);
    
    sockfd = socket(AF_INET, SOCK_DGRAM, 0);
}

Udpclient::Udpclient(){

}

Udpclient::~Udpclient(){

}

sockfd_t Udpclient::getSockfd(){
	return sockfd;
}

void Udpclient::shutdown(){
	close(sockfd);
}

bool Udpclient::receive_msg(void *buff, size_t buffsize, handle_func handler){
	int n = recvfrom(sockfd, buff, buffsize, 0, NULL, NULL);
	if(n < 0)
		return false;
	handler(sockfd, servaddr, buff, n);
	return true;
}

bool Udpclient::send_msg(const char *buff, size_t buffsize){
	int wn = sendto(sockfd, buff, buffsize, 0, (SA*)&servaddr, sizeof(servaddr));
	if(wn < 0){
		perror("udpclient::send_msg():\n");
		return false;
	}

	return true;
}

void Udpclient::send_file(const char *filename){
	printf("Send file %s\n",filename );
	long size = getFilesize(filename);
	int rn, wn;

	int fd = open(filename, O_RDONLY);
	if(fd < 0) 
		throw "Error: open error.";

	while((rn = read(fd, sendbuff, MAXBUFF)) > 0){
		printf("Read %d byte\n",rn);
		wn = sendto(sockfd, sendbuff, rn, 0, (SA*)&servaddr, sizeof(servaddr));
		printf("Write %d byte\n",wn);
		if(wn < 0) 
			throw "Error: write error";
	}
}

bool Udpclient::send_msg(std::string& msg){
	return send_msg(msg.c_str(), msg.size());
}

bool Udpclient::send_msg(const char* msg){
	return send_msg(msg, strlen(msg));
}

bool Udpclient::send_ack(){
	return send_msg(&ACK, sizeof(ACK));
}