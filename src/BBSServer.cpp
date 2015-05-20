#include "BBSServer.h"

const char *SCREEN_CLEAR_CTRL = "\033[2J\033[1;1H";
const char *RETURN_OPTION = "[RT]urn";
const char *ACCOUNTS_FILENAME = "accounts";
const char *ENTRY_MENU = "\033[2J\033[1;1H[R]egister\t[L]ogin\n";
const char *MAIN_MENU = "\033[2J\033[1;1H[SU]sers\t[SA]rticles\t[Y]ell\t[T]ell\t[O]Logout\n";
const char *ARTILCELIST_MENU = "[RT]urn\t[E]nter Article\t[A]dd Article\t";
const char *ARTICLE_ENTER_IDERROR = "Enter failed: no such article.\n";
const char *ARTICLE_EDIT_MENU = "\033[2J\033[1;1H[ESC]Send\n";
const char *ARTICLE_APPEND_BLACK_SUCCEED = "\033[2J\033[1;1HAdd black user ID succeed !\n[RT]urn\n";
const char *ARTICLE_APPEND_BLACK_FAILED = "\033[2J\033[1;1HAdd black user ID failed\n[RT]urn\n";
const char *ARTICLE_DEL_BLACK_SUCCEED = "\033[2J\033[1;1HDel black user ID succeed !\n[RT]urn\n";
const char *ARTICLE_DEL_BLACK_FAILED = "\033[2J\033[1;1HDel black user ID failed\n[RT]urn\n";
const char *RESPONSE_SUCCESS = "\033[2J\033[1;1HResponse succeed !\n[RT]urn\n";
const char *RESPONSE_FAILED = "\033[2J\033[1;1HError: response failed\n[RT]urn\n";
const char *MSG_LOGIN_SUCCESS = "\033[2J\033[1;1HLogin succeess\n[RT]urn\n";
const char *MSG_LOGIN_UID_ERROR = "\033[2J\033[1;1HLogin failed: No such user ID\n[RT]turn\n";
const char *MSG_LOGIN_PWD_ERROR = "\033[2J\033[1;1HLogin failed: Password wrong\n[RT]turn\n";
const char *MSG_LOGOUT = "\033[2J\033[1;1HLogout succeessfully\n[RT]urn\n";
const char *MSG_REG_SUCCESS = "\033[2J\033[1;1HRegister succeess\n[RT]turn\n";
const char *MSG_REG_DUP = "\033[2J\033[1;1HRegister failed: Duplicated User ID\n[RT]turn\n";
const char *MSG_REG_INVALIDARGS = "\033[2J\033[1;1HRegister failed: Invalid Arguments\n[RT]turn\n";

text_func_record GUEST_FUNC_MAP[] = {
	{"R", &BBSServer::guest_register},
	{"L", &BBSServer::guest_login},
	{"E", &BBSServer::guest_echo},
	{"RT", &BBSServer::guest_return},
	{"", NULL}
};

text_func_record USER_FUNC_MAP[] = {
	{"RT", &BBSServer::user_return},
	{"SU", &BBSServer::user_show_users},
	{"SA", &BBSServer::user_show_articles},
	{"", NULL}
};

BBSServer::BBSServer(in_addr_t addr, port_t port)
	: Udpserver(addr, port),
	  conn_manager(*this) {
	account_manager = AccountManager(ACCOUNTS_FILENAME);
}

BBSServer::~BBSServer() {

}

void
BBSServer::recv() {
	Udpserver::recv(recvbuff, MAXBUFF - 1, &BBSServer::handle_entry, this);
}

void
BBSServer::pollWaiting() {
	conn_manager.pollWaiting();
}

void
BBSServer::state_recv_ack(Connection& conn, void *buff, size_t size) {
	// Parse ACK Packet
	std::cout << "RECV_ACK" << std::endl;
	char *cbuff = (char*)buff;
	char ack = cbuff[0];

	if (ack == ACK) {
		std::cout << "ACK" << std::endl;
		conn.translate(conn.last_state);
		conn_manager.removeWaiting(conn);
	}
	else
		conn.translate(Connection::RECV_ACK);
}

void
BBSServer::state_recv_text(Connection& conn, void *buff, size_t size, text_func_record* func_table) {
	// Parse text-based command
	char *cbuff = (char*)buff;
	cbuff[size] = '\0';

	std::cout << "RECV_TEXT" << std::endl;
	std::vector<String> args = Util::parse(cbuff);
	if (args.size() < 1)
		throw Util::Exception::ERR_PARSE_SIZE;

	bool isValidcmd = false;
	for (int i = 0; func_table[i].func != NULL; i++) {
		if (args[0] == func_table[i].key) {
			(this->*func_table[i].text_func_record::func)(conn, args);
			isValidcmd = true;
			break;
		}
	}

	if (!isValidcmd) std::cout << "Unknown command" << std::endl;
}


void
BBSServer::handle_entry(int sockfd, struct sockaddr_in addr, void *buff, size_t buffsize) {
	// Pass the data to corresponding handler
	std::cout << "Handle Entry" << std::endl;
	try {
		// Add a new connection
		if (!conn_manager.isExist(addr))
			conn_manager.addConnection(addr);

		// Pass to guest or user
		if (conn_manager.getConnection(addr).isLogin())
			handle_user(sockfd, addr, buff, buffsize);
		else
			handle_guest(sockfd, addr, buff, buffsize);

	} catch (ConnectionManager::Exception e) {
		connectionManager_err(e);
	}
}

void
BBSServer::handle_guest(int sockfd, SIN addr, void *buff, size_t size) {
	std::cout << "Handle Guest" << std::endl;
	try {
		Connection& conn = conn_manager.getConnection(addr);
		switch (conn.state) {
		case Connection::RECV_ACK:
			state_recv_ack(conn, buff, size);
			break;
		case Connection::RECV_TEXT:
			state_recv_text(conn, buff, size, GUEST_FUNC_MAP);
			break;
		default:
			std::cout << "Unknown state" << std::endl;
			break;
		}
	}
	catch (ConnectionManager::Exception e) {
		std::cout << "Connection Manager error:" << std::endl;
	}
	catch (Connection::Exception e) {
		std::cout << "Connection error:" << std::endl;
	}
	catch (AccountManager::Exception e) {
		accountManager_err(e);
	}
	catch (Util::Exception e) {
		util_err(e);
	}
}

void
BBSServer::guest_register(Connection& conn, std::vector<String> args) {
	std::cout << "Guest Register" << std::endl;
	if (args.size() < 3)
		throw Util::ERR_PARSE_SIZE;
	if (account_manager.isExist(args[1].c_str()))
		throw AccountManager::ERR_DUPID;
	else {
		account_manager.insert(args[1], args[2]);
		conn.send_msg(MSG_REG_SUCCESS);
	}
}

void
BBSServer::guest_login(Connection& conn, std::vector<String> args) {
	std::cout << "Guest Login" << std::endl;
	if (args.size() < 3)
		throw Util::ERR_PARSE_SIZE;
	if (!account_manager.isExist(args[1].c_str())){
		conn.send_msg(MSG_LOGIN_UID_ERROR);
		throw AccountManager::ERR_NULL;
	}
	else if(account_manager.isCorrect(args[1], args[2])){
		conn.setLogin(args[1]);
		conn.send_msg(MSG_LOGIN_SUCCESS);
	}
	else{
		conn.send_msg(MSG_LOGIN_PWD_ERROR);
	}
}

void
BBSServer::guest_echo(Connection& conn, std::vector<String> args) {
	std::cout << "Guest Echo" << std::endl;
	try {
		conn.send_msg(args[1].c_str());
	}
	catch (...) {
		std::cerr << "guest_echo(): args error." << std::endl;
	}
}

void
BBSServer::guest_return(Connection& conn, std::vector<String> args) {
	std::cout << "Guest Return" << std::endl;
	try {
		conn.send_msg(ENTRY_MENU);
	}
	catch (...) {
		std::cerr << "guest_return(): error." << std::endl;
	}
}

void
BBSServer::handle_user(int sockfd, SIN addr, void *buff, size_t size) {
	std::cout << "Handle User" << std::endl;
	try {
		Connection& conn = conn_manager.getConnection(addr);
		switch (conn.state) {
		case Connection::RECV_ACK:
			state_recv_ack(conn, buff, size);
			break;
		case Connection::RECV_TEXT:
			state_recv_text(conn, buff, size, USER_FUNC_MAP);
			break;
		default:
			std::cout << "Unknown state" << std::endl;
			break;
		}
	}
	catch (ConnectionManager::Exception e) {
		std::cout << "Connection Manager error:" << std::endl;
	}
	catch (Connection::Exception e) {
		std::cout << "Connection error:" << std::endl;
	}
	catch (AccountManager::Exception e) {
		accountManager_err(e);
	}
	catch (Util::Exception e) {
		util_err(e);
	}
}

void
BBSServer::user_return(Connection& conn, std::vector<String> args) {
	std::cout << "User Return" << std::endl;
	try {
		conn.send_msg(MAIN_MENU);
	}
	catch (...) {
		std::cerr << "user_return(): error." << std::endl;
	}
}

void 
BBSServer::user_show_users(Connection& conn, std::vector<String> args){
	std::cout << "User Show users" << std::endl;
	std::stringstream ss;
	ss << SCREEN_CLEAR_CTRL;
	for(std::map<SIN, Connection>::iterator it = conn_manager.conn_map.begin();
		it != conn_manager.conn_map.end();
		it++){
		if(it->second.isLogin()){
			ss << it->second.getAddr_p() << " " << it->second.id << std::endl;
		}
	}

	ss << RETURN_OPTION << std::endl;

	conn.send_msg(ss.str().c_str());
}

void 
BBSServer::user_show_articles(Connection& conn, std::vector<String> args){
    DIR *d;
    struct dirent *dir;
    d = opendir(ARTICLE_BASE);

    std::stringstream ss;

    char buff[HEADER_LEN];
    snprintf(buff, HEADER_LEN, "%-10s\t%-10s\t%-40s\t%-20s","ArticleID", "Publisher", "Title", "Published Date");

    ss << SCREEN_CLEAR_CTRL;
    ss << buff << std::endl;

    if(d){
        while((dir = readdir(d)) != NULL){
            if(strcmp(dir->d_name, ".") != 0 && strcmp(dir->d_name, "..") != 0){
            	snprintf(buff, MAXLINE, "%s/%s",ARTICLE_BASE, dir->d_name);
            	Articleheader articleheader(buff);
            	ss << articleheader.toString() << std::endl;
            }
        }
        closedir(d);
    }  
    ss << ARTILCELIST_MENU << std::endl;
    conn.send_msg(ss.str().c_str());
}

void 
BBSServer::user_yell(Connection& conn, std::vector<String> args){

}

void 
BBSServer::user_tell(Connection& conn, std::vector<String> args){

}

void 
BBSServer::user_logout(Connection& conn, std::vector<String> args){

}

void
BBSServer::handle_echo(int sockfd, struct sockaddr_in addr, void *buff, size_t buffsize) {

}

void
BBSServer::connectionManager_err(ConnectionManager::Exception e) {
	switch (e) {
	case ConnectionManager::KEY_NOFOUND:
		std::cerr << "ConnectionManager: addr not found." << std::endl;
		break;
	case ConnectionManager::CONN_DUP:
		std::cerr << "ConnectionManager: addr duplicate." << std::endl;
		break;
	default:
		break;
	}
}

void
BBSServer::accountManager_err(AccountManager::Exception e) {
	switch (e) {
	case AccountManager::ERR_DUPID:
		std::cerr << "AccountManager::ERR_DUPID(): id has been exist." << std::endl;
		break;
	case AccountManager::ERR_IO:
		std::cerr << "AccountManager::ERR_IO(): i/o error." << std::endl;
		break;
	case AccountManager::ERR_NULL:
		std::cerr << "AccountManager::ERR_NULL(): no such id." << std::endl;
		break;
	default:
		break;
	}
}

void
BBSServer::util_err(Util::Exception e) {
	switch (e) {
	case Util::ERR_PARSE_SIZE:
		std::cerr << "Util: args parsing error." << std::endl;
		break;
	default:
		break;
	}
}

