#include "server.h"

port_t port;

char buff[MAXBUFF];

Udpserver server;

// <ID, PWD> map
std::map<std::string, std::string> accounts_map;

// <ADDR, HANDLER> map
std::map<SIN, Clienthandler> clihandlers_map;
 
int
main(int argc, char *argv[]){
	if(argc != 2){
		std::cout << "Usage: server [Port]" << std::endl;
		return 0;
	}

	port = atoi(argv[1]);

	try{
		fload_accounts(ACCOUNTS_FILENAME);
		setMaxartileID();
		server = Udpserver(htonl(INADDR_ANY), port);
		mainloop();
		server.shutdown();
	}catch(const char *err_msg){
		std::cerr << err_msg << std::endl;
	}

	return 0;
}

void 
mainloop(){
	fd_set rset;
	FD_ZERO(&rset);
	sockfd_t maxfdpl = server.getSockfd() + 1;

	int nready;
	for(;;){
		FD_SET(server.getSockfd(), &rset);
		if((nready = select(maxfdpl, &rset, NULL, NULL, NULL)) < 0){
			if(errno == EINTR)
				continue;
			else{
				perror("mainloop(): Select error\n");
				return;
			}
		}

		if(FD_ISSET(server.getSockfd(), &rset)){
			server.recv(buff, MAXBUFF - 1, handler_entry);
		}
	}
}

void 
handler_echo_ser(int sockfd, struct sockaddr_in addr, void *buff, size_t buffsize){
	char *ch_buff = (char*)buff;
	ch_buff[buffsize] = '\0';
	printf(":%s\n",ch_buff);
	
	sendto(sockfd, buff, buffsize, 0, (SA*)&addr, sizeof(addr));
}

void 
handler_entry(int sockfd, struct sockaddr_in addr, void *buff, size_t buffsize){
	if(isLogined(addr))
		func_user_mux(sockfd, addr, buff, buffsize);
	else
		func_guest_mux(sockfd, addr, buff, buffsize);
}

void func_user_mux(int sockfd, struct sockaddr_in addr, void *buff, size_t buffsize){
	ClihanlderIterator clienthandler_it;
	if((clienthandler_it = clihandlers_map.find(addr)) == clihandlers_map.end()){
		std::cout << "func_user_mux(): cannot find hanlder" << std::endl;
		return;
	}

	Clienthandler& client_ref = clienthandler_it->second;
	switch(client_ref.getState()){
		case STATE_RECV_TEXT:
		{
			char *instr = (char*)buff;
			instr[buffsize] = '\0';

			std::string instr_str(instr);
			std::vector<std::string> args = split(instr_str);

			if(args.size() < 1) return; // Arguments parsing error
			client_ref.statehandler_recv_text(args);
		}
		break;
		case STATE_RECV_ARTICLE:
			client_ref.statehandler_recv_article(buff, buffsize);
			break;
		default:
			break;
	}
}

void func_guest_mux(int sockfd, struct sockaddr_in addr, void *buff, size_t buffsize){
	char *instr = (char*)buff;
	instr[buffsize] = '\0';

	std::string instr_str(instr);
	std::vector<std::string> args = split(instr_str);

	if(args.size() < 1) return; // Arguments parsing error

	char opcode = decode_op(args[0]);
	try{
		switch(opcode){
			case OP_RETURN:
				std::cout << "OP_RETURN" << std::endl;
				func_display_entry(&addr);
				break;
			case OP_VISIT:
				std::cout << "OP_VISIT" << std::endl;
				func_display_entry(&addr);
				break;
			case OP_REGISTER:
				if(args.size() != 3) 
					throw EID_INVALIDARGS;
				std::cout << "OP_REGISTER" << std::endl;
				func_register(args[1], args[2]);
				server.send_msg(MSG_REG_SUCCESS, &addr);
				break;
			case OP_LOGIN:
				if(args.size() != 3) 
					throw EID_INVALIDARGS;
				std::cout << "OP_LOGIN" << std::endl;
				func_login(addr, args[1], args[2]);
				server.send_msg(MSG_LOGIN_SUCCESS, &addr);
				break;
			default:
				std::cout << "UNKNWON OP" << std::endl;
				break;
		}
	} catch(int eid){
		switch(eid){
			case EID_INVALIDARGS:
				std::cout << "INVALID ARGUMENTS" << std::endl;
				server.send_msg(MSG_REG_INVALIDARGS, &addr);
				break;
			case EID_UIDDUPLICATED:
				std::cout << "DUPLICATED UID" << std::endl;
				server.send_msg(MSG_REG_DUP, &addr);
				break;
			case EID_UIDNOTEXIST:
				std::cout << "UID NOT EXIST" << std::endl;
				server.send_msg(MSG_LOGIN_UID_ERROR, &addr);
				break;
			case EID_PWDERROR:
				std::cout << "PWD ERROR" << std::endl;
				server.send_msg(MSG_LOGIN_PWD_ERROR, &addr);
				break;
			default:
				break;
		}
	}
}

void 
func_login(struct sockaddr_in addr, std::string& id, std::string& pwd){
	std::map<std::string, std::string>::iterator it;
	if((it = accounts_map.find(id)) != accounts_map.end()){
		if(pwd != it->second)
			throw EID_PWDERROR;
		Clienthandler handler(server, clihandlers_map, addr, id, pwd);
		clihandlers_map.insert(std::pair<SIN, Clienthandler>(addr, handler));
#ifdef LOG
		std::cout << "Login succeess" << std::endl;
#endif
	}
	else throw EID_UIDNOTEXIST; 
}

void
func_register(std::string& key, std::string &value){
	if(accounts_map.find(key) == accounts_map.end()){
		accounts_map.insert(std::pair<std::string, std::string>(key, value));
#ifdef LOG
		std::cout << "Register succeess" << std::endl;
#endif
	}
	else throw EID_UIDDUPLICATED; 
}

void 
func_display_entry(SIN *addr){
	server.send_msg(ENTRY_MENU, addr);
}

bool 
isLogined(SIN addr){
	return (clihandlers_map.find(addr) != clihandlers_map.end());
}

void 
fload_accounts(const char *filename){
	FILE *fp = fopen(filename, "r");
	if(fp == NULL){
		std::cerr << "fload_accounts(): failed to open the file" << std::endl;
		return;
	}

	char uid[MAXLINE], pwd[MAXLINE];
	while(fscanf(fp, "%s%s",uid ,pwd) == 2){
		std::string s_uid(uid);
		std::string s_pwd(pwd);
		accounts_map.insert(AccountRecord(s_uid, s_pwd));
	}
}

void
setMaxartileID(){
	FILE *fp = fopen("articles.cfg", "r");
	if(fp == NULL){
		throw "setMaxartileID(): failed to open";
	}
	fscanf(fp, "%lld", &MAXARTICLEID);
	fclose(fp);
}