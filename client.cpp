/**
	Data structure:
		bool isLogin
		enum State state

	Not-yet login: (before feteched user command, always send visit command to server)
		Send:
		if state is STATE_RECV_MENU:
			send visit
		
		get user command
		case(command)
			LOGIN:
				send ${ID, PWD}, and expected text response
				STATE_RECV_TEXT => STATE_RECV_LOGIN_ACK => STATE_RECV_MENU
			REGISTER:
				send ${ID, PWD}, and expected text response
				STATE_RECV_TEXT => STATE_RECV_LOGIN_ACK => STATE_RECV_MENU
		endcase

		Receive:
		case(state)
			STATE_RECV_LOGIN_ACK:
				if msg is ACK:
					isLogin = true
				else:
					isLogin = false
			STATE_RECV_TEXT:
				print msg
		endcase

	Login: (before feteched user command, always send visit command to server)
		Send:
		if $pos is MAINMENU:
			send visit
		get user command
		case (command)
			RETURN:
				DO NOT SEND ANYTHING !
				ANY_STATE => STATE_RECV_MENU
			SHOW USER:
				send SHOW USER command
				STATE_RECV_MENU => STATE_RECV_USERLIST
			SHOW ARTICLES:
				send SHOW USER command
				STATE_RECV_MENU => STATE_RECV_ARTICLELIST
			ADD ARTICLE:
				let user edit an article
				send ADD ARTICLE command
			ENTER ARTICLE:
			YELL:
			TELL:
			LOGOUT:
		endcase
**/

#include "network.h"
#include "udpclient.h"
#include "bbs.h"

// Global variable
port_t port;
Udpclient client;

// Client State (used to perform different receving model)
State state = STATE_RECV_TEXT;

// Login state
bool isLogin = false;

// Receive buffer
char buff[MAXBUFF];

// Send buffer
char sendbuff[MAXBUFF];

void handler_echo_recv_cli(int, struct sockaddr_in, void*, size_t);
void handler_recv_mux(int, struct sockaddr_in, void*, size_t);

void func_display_text(void *buff, size_t buffsize);
void func_add_article(void *buff, size_t buffsize);

int instr_type(char opcode);
int encode(std::vector<std::string> args, char *dst, size_t dst_len);

void mainloop();
void state_mux(char opcode);
void send_visit();
void send_showarticles();
std::vector<std::string> getUserinput();

int
main(int argc, char *argv[]){
	if(argc < 3){
		std::cout << "Usage: cli [IP Address] [Port]" << std::endl;
		return 0;
	}

	port = atoi(argv[2]);

	try{
		client = Udpclient(argv[1], port);
		mainloop();
		client.shutdown();
	}catch(const char *err_msg){
		std::cerr << err_msg << std::endl;
	}

	return 0;
}

void 
mainloop(){
	struct pollfd pfds[2];
	pfds[0].fd = fileno(stdin); pfds[0].events = POLLRDNORM;
	pfds[1].fd = client.getSockfd(); pfds[1].events = POLLRDNORM;

	int nready = 2;
	send_visit();

	for(;;){
		poll(pfds, nready, -1);

		// Incoming message from server
		if(pfds[1].revents & (POLLRDNORM | POLLERR)){
			client.receive_msg(buff, MAXBUFF, handler_recv_mux);
		}

		// User input
		if(pfds[0].revents & POLLRDNORM){
			try{
				// Sender FSM
				switch(state){
					default:
					{	
						std::vector<std::string> args = getUserinput();
						if(args.size() < 1) continue;
						int opcode = encode(args, sendbuff, sizeof(sendbuff));
						state_mux(opcode);
						client.send_msg(sendbuff, strlen(sendbuff));
					}
					break;
				}
		
			}catch(const char *err_msg){
				std::cerr << err_msg << std::endl;
			}
		}
	}
}

void 
handler_echo_recv_cli(int sockfd, struct sockaddr_in addr, void *buff, size_t buffsize){
	char *ch_buff = (char*)buff;
	ch_buff[buffsize] = '\0';
	std::cout << ch_buff;
}

void 
handler_recv_mux(int sockfd, struct sockaddr_in addr, void *buff, size_t buffsize){
	switch(state){
		case STATE_SEND_ARTICLE:
		{	
			func_display_text(buff, buffsize);
			func_add_article(sendbuff, sizeof(sendbuff));
			client.send_msg(sendbuff, strlen(sendbuff));
			state = STATE_RECV_TEXT;
		}
		break;
		case STATE_RECV_TEXT:
			func_display_text(buff, buffsize);
			break;
		case STATE_RECV_FILE:
			break;
		default:
			break;
	}
}

void
func_display_text(void *buff, size_t buffsize){
	if(buffsize < 2) {
		std::cout << "func_display_text(): buff error" << std::endl;
		return;
	}

	char *ch_buff = (char*)buff;
	ch_buff[buffsize] = '\0';
	if(ch_buff[0] == CTRL_CLEAR)
		system("clear");
	ch_buff++;
	std::cout << ch_buff;
}

void func_add_article(void *buff, size_t buffsize){
	int cnt = 0;
	char *chbuff = (char*)buff;
	char c;
	while( (c = getch()) != ESCAPE){
		if(c == '\x7f'){
			cnt--;
			putchar('\b');
		}
		else{
			chbuff[cnt++] = c;
			putchar(c);
		}
		fflush(stdout);
	}
	chbuff[cnt] = '\0';
	std::cout << "End-of-Article" << std::endl
			  << "[RT]urn" << std::endl;
}

void
send_visit(){
	client.send_msg("RT\n", sizeof("RT\n"));
}

void
send_showarticles(){
	client.send_msg("SA\n", sizeof("SA\n"));
}

int encode(std::vector<std::string> args, char *dst, size_t dst_len){
	size_t instr_len;
	char opcode = decode_op(args[0]);

	int type = instr_type(opcode);
	if(type == 2){
		if(args.size() != 3) 
			throw "encode(): invalid args length";
		instr_len = snprintf(dst, dst_len, "%s\n%s\n%s\n",args[0].c_str(), args[1].c_str(), args[2].c_str());
	}
	else if(type == 1){
		if(args.size() != 2) 
			throw "encode(): invalid args length";
		printf("op: 0x%x\n",opcode);
		instr_len =snprintf(dst, dst_len, "%s\n%s\n",args[0].c_str(), args[1].c_str());
	}
	else{
		instr_len = snprintf(dst, dst_len, "%s\n",args[0].c_str());
	}

	if(instr_len < 0) "encode(): encoding error";

	return opcode;
}

void state_mux(char opcode){
	switch(opcode){
		case OP_RETURN:
			state = STATE_RECV_TEXT;
			break;
		case OP_VISIT: // Send visit command -> wait text echo from server
			state = STATE_RECV_TEXT;
			break;
		case OP_REGISTER: // Send register -> wait confirm from server (ACK: login, NAK: error)
			state = STATE_RECV_TEXT;
			break;
		case OP_LOGIN: // Send login -> wait confirm from server (ACK: login, NAK: error)
			state = STATE_RECV_TEXT;
			break;
		case OP_YELL:
			break;
		case OP_TELL:
			break;
		case OP_SHOWUSER:
			break;
		case OP_SHOWARTICLE:
			break;
		case OP_ADD_ARTICLE:
			state = STATE_SEND_ARTICLE;
			break;
		case OP_ENTER_ARTICLE:
			break;
		case OP_RESPONSE:
			break;
		case OP_DOWNLOAD_ATTACH:
			break;
		case OP_UPLOAD_ATTACH:
			break;
		case OP_ADD_BLACK:
			break;
		case OP_DEL_BLACK:
			break;
		case OP_LOGOUT:
			break;
		default:
			break;
	}
}

int instr_type(char opcode){
	for(int i = 0; D_TYPES[i] != 0x0; i++)
		if(opcode == D_TYPES[i])
			return 2;
	for(int i = 0; S_TYPES[i] != 0x0; i++)
		if(opcode == S_TYPES[i])
			return 1;
	for(int i = 0; Z_TYPES[i] != 0x0; i++)
		if(opcode == Z_TYPES[i])
			return 0;
	return -1;
}

std::vector<std::string> getUserinput(){
	std::string sendline;
	std::vector<std::string> args;
	if(getline(std::cin, sendline) != NULL){
		if(sendline.size() >= 1){
			try{
				args = split(sendline);
			} catch(const char *err_msg){
				std::cerr << err_msg << std::endl;
			}
		}
	}
	return args;
}