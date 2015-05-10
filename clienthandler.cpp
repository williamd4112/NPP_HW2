#include "clienthandler.h"

long long MAXARTICLEID = 0;

Clienthandler::Clienthandler(Udpserver& server, std::map<SIN, Clienthandler>& clihandlers_map, SIN src_addr, std::string& id, std::string& pwd):
server(server),
clihandlers_map(clihandlers_map),
id(id),
pwd(pwd),
src_addr(src_addr),
state(STATE_RECV_TEXT){
	cur_aid = "-1";
	pos_st.push(MAINMENU);
	pos = pos_st.top();
	article_ptr = NULL;
}

Clienthandler::~Clienthandler(){

}

std::string Clienthandler::getId(){
	return id;
}

std::string Clienthandler::getPwd(){
	return pwd;
}

std::string Clienthandler::getAddr_p(){
	char ip_str[IPV4_ADDRLEN + 1];
	inet_ntop(AF_INET, &src_addr.sin_addr, ip_str, sizeof(ip_str));

	return std::string(ip_str);
}

SIN& Clienthandler::getAddr(){
	return src_addr;
}

port_t Clienthandler::getPort(){
	return ntohs(src_addr.sin_port);
}

Position Clienthandler::getPos(){
	return pos;
}

State Clienthandler::getState(){
	return state;
}

void Clienthandler::moveTo(Position new_pos){
	pos_st.push(new_pos);
	pos = pos_st.top();
}

void Clienthandler::back(){
	if(pos_st.size() == 1) return;
	pos_st.pop();
	pos = pos_st.top();
}

void Clienthandler::translate(State new_state){
	state = new_state;
}

void 
Clienthandler::statehandler_recv_text(std::vector<std::string> args){
	char opcode = decode_op(args[0]);
	try{
		//printf("0x%X\n",opcode);
		switch(opcode){
			case OP_RETURN:
				std::cout << "OP_RETURN" << std::endl;
				if(pos == ARTICLE) 
					exit_article();
				back();
				switch(pos){
					case MAINMENU:
						func_display_mainmenu();
						break;
					case ARTICLE_LIST:
						func_display_article_list();
						break;
					default:
						break;
				}
				break;
			case OP_VISIT:
				std::cout << "OP_VISIT" << std::endl;
				func_display_mainmenu();
				break;
			case OP_SHOWUSER:
				if(pos != MAINMENU) break;
				std::cout << "OP_SHOWUSER" << std::endl;
				moveTo(USER_LIST);
				func_display_users();
				break;
			case OP_SHOWARTICLE:
				if(pos != MAINMENU) break;
				std::cout << "OP_SHOWARTICLE" << std::endl;
				moveTo(ARTICLE_LIST);
				func_display_article_list();
				break;
			case OP_YELL:
				if(pos != MAINMENU) break;
				std::cout << "OP_YELL" << std::endl;
				func_yell(args[1]);
				break;
			case OP_TELL:
				if(pos != MAINMENU) break;
				std::cout << "OP_TELL" << std::endl;
				func_tell(args[1], args[2]);
				break;
			case OP_ENTER_ARTICLE:
				if(pos != ARTICLE_LIST) break;
				std::cout << "OP_ENTER_ARTICLE" << std::endl;
				try{
					func_display_article(args[1]);
					moveTo(ARTICLE);
					enter_article(args[1]);
				} catch (...){
					std::cerr << "No such Article ID" << std::endl;
					server.send_msg(ARTICLE_ENTER_IDERROR, &src_addr);
					exit_article();
				}
				break;
			case OP_ADD_ARTICLE:
				if(pos != ARTICLE_LIST) break;
				std::cout << "OP_ADD_ARTICLE" << std::endl;
				moveTo(ARTICLE_EDIT);
				func_add_article(args[1]);
				translate(STATE_RECV_ARTICLE);
				break;
			case OP_RESPONSE:
				if(pos != ARTICLE) break;
				std::cout << "OP_RESPONSE" << std::endl;
				try{
					func_response(args[1]);
					server.send_msg(RESPONSE_SUCCESS, &src_addr);
				} catch(...){
					std::cerr << "Failed to response" << std::endl;
					server.send_msg(RESPONSE_FAILED, &src_addr);
				}
				break;
			case OP_ADD_BLACK:
				if(pos != ARTICLE) break;
				std::cout << "OP_ADD_BLACK" << std::endl;
				try{
					func_add_blacklist(args[1]);
					server.send_msg(ARTICLE_APPEND_BLACK_SUCCEED, &src_addr);
				} catch(...){
					std::cerr << "Append black list error" << std::endl;
					server.send_msg(ARTICLE_APPEND_BLACK_FAILED, &src_addr);
				}
				break;
			case OP_DEL_BLACK:
				if(pos != ARTICLE) break;
				std::cout << "OP_DEL_BLACK" << std::endl;
				try{
					func_del_blacklist(args[1]);
					server.send_msg(ARTICLE_DEL_BLACK_SUCCEED, &src_addr);
				} catch(...){
					std::cerr << "Del black list error" << std::endl;
					server.send_msg(ARTICLE_DEL_BLACK_FAILED, &src_addr);
				}
				break;
			case OP_LOGOUT:
				if(pos != MAINMENU) break;
				std::cout << "OP_LOGOUT" << std::endl;
				moveTo(ARTICLE_EDIT);
				func_logout();
				break;
			default:
				printf("UNKNWON OP(0x%x)\n",opcode);
				break;
		}
	} catch(int eid){
		switch(eid){
			case EID_INVALIDARGS:
				break;
			case EID_UIDDUPLICATED:
				break;
			case EID_UIDNOTEXIST:
				break;
			case EID_PWDERROR:
				break;
			default:
				break;
		}
	}
}

void 
Clienthandler::statehandler_recv_article(void *buff, size_t buffsize){
	char *chbuff = (char*)buff;
	chbuff[buffsize] = '\0';
	article_ptr->context << chbuff;
#ifdef LOG 
	std::cout << "Receive article:" << std::endl;
	std::cout << chbuff << std::endl;
#endif
	translate(STATE_RECV_TEXT);
	back();

	try{
		article_ptr->store();
		std::cout << "Write ok" << std::endl;
		delete article_ptr;
	}catch(const char *err_msg){
		std::cerr << "Failed to store:";
		std::cerr << err_msg << std::endl;
		delete article_ptr;
	}

}

void 
Clienthandler::func_display_mainmenu(){
	server.send_msg(MAIN_MENU, &src_addr);
}

void 
Clienthandler::func_display_users(){
	std::stringstream ss;
	ss << CTRL_CLEAR;

	for(ClihanlderIterator it = clihandlers_map.begin(); it != clihandlers_map.end(); it++){
		ss << it->second.getAddr_p() 
		   << ":"  << it->second.getPort()
		   << "\t" << it->second.getId() << std::endl;
	}
	ss << "[RT]urn" << std::endl;

	server.send_msg(ss.str(), &src_addr);
}

void
Clienthandler::func_display_article_list(){
    DIR *d;
    struct dirent *dir;
    d = opendir(ARTICLE_BASE);

    std::stringstream ss;

    char buff[HEADER_LEN];
    snprintf(buff, HEADER_LEN, "%-10s\t%-10s\t%-40s\t%-20s","ArticleID", "Publisher", "Title", "Published Date");

    ss << "\x01";
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
    server.send_msg(ss.str(), &src_addr);
}

void
Clienthandler::func_display_article(std::string& aid){
	char pathbuff[MAXLINE];
	snprintf(pathbuff, MAXLINE, "%s/%s",ARTICLE_BASE, aid.c_str());

	Article article(pathbuff);

	std::stringstream ss;
	ss << '\x01' << article.toString(id) << std::endl;

	server.send_msg(ss.str(), &src_addr);
}

/* Create a temp article to edit */
void 
Clienthandler::func_add_article(std::string& title){
	time_t ticks;

    char buff[MAXLINE];
    ticks=time(NULL);
    snprintf(buff, sizeof(buff), "%.24s", ctime(&ticks));
  	std::string date(buff);
  	std::string aid = std::to_string(getArticleID());

	article_ptr = new Article(aid, title, id, date);
	server.send_msg(ARTICLE_EDIT_MENU, &src_addr);
}

void 
Clienthandler::func_response(std::string& response){
	char buff[FILENAME_SIZE];
	snprintf(buff, FILENAME_SIZE, "%s/%s/%s",ARTICLE_BASE, cur_aid.c_str(), "response");

	std::cout << "Load " << buff << std::endl;
	std::ofstream ofs(buff, std::ios::app);
	// ofs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if(ofs.fail()) throw EID_RESPONSEERROR;

	ofs << getAddr_p() 
	    << ":" << getPort()
	    << "  " << id
	    << "  " << response << std::endl;

	ofs.close();
}

void 
Clienthandler::func_add_blacklist(std::string& bid){
	Article article(cur_aid);
	article.appendBlacklist(bid);
}

void 
Clienthandler::func_del_blacklist(std::string& bid){
	Article article(cur_aid);
	article.removefromBlacklist(bid);
}

void 
Clienthandler::func_yell(std::string& msg){
	std::stringstream ss;
	ss << CTRL_APPEND << getAddr_p() << ":" << getPort() << "\t" << msg << std::endl;

	for(ClihanlderIterator it = clihandlers_map.begin(); it != clihandlers_map.end(); it++){
		if(it->second.getPos() != MAINMENU) continue;
		SIN addr = it->second.getAddr();
		server.send_msg(ss.str(), &addr);
	}
}

void 
Clienthandler::func_tell(std::string& id, std::string& msg){
	std::stringstream ss;
	ss << CTRL_APPEND << getAddr_p() << ":" << getPort() << "\t" << msg << std::endl;

	for(ClihanlderIterator it = clihandlers_map.begin(); it != clihandlers_map.end(); it++){
		if(it->second.getId() != id || it->second.getPos() != MAINMENU) continue;
		SIN addr = it->second.getAddr();
		server.send_msg(ss.str(), &addr);
	}
}

void
Clienthandler::func_logout(){
	server.send_msg(MSG_LOGOUT, &src_addr);
	clihandlers_map.erase(src_addr);
}

long
Clienthandler::getArticleID(){
	FILE *fp = fopen("articles.cfg", "w");
	if(fp == NULL){
		throw "update_maxaid(): failed to open";
	}
	fprintf(fp, "%lld",++MAXARTICLEID);
	fclose(fp);

	return MAXARTICLEID;
}

bool Clienthandler::operator ==(Clienthandler b){
	return id == b.getId();
}

