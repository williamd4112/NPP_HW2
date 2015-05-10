/**
	Udpserver: a udp based server, receive message from only one port
			   and dispatch the message to specific function to handle it
	Take simple bbs server for example:
	
	Macro:
		ADMIN 0x1
		OWNER 0x2
		USER 0x4
		BLACK 0x8

	Terminology:
		Article base:
			the directory used to store article folder
		Article folder:
			the folder used to store article distributor, text content, response, attachment
		Permission:
			1st bit: Administrator
			2nd bit: Owner
			3rd bit: User
			4th bit: Black user


	Data structure:
		map<struct sockaddr_in, ClientHandler> map_clihandlers
		queue<string> yell_queue
		queue<pair<string, string>> tell_queue
	
	Not yet Login: (cliaddr not in map_clihandlers)
		case (msg[OP])
			VISIT: pass cliaddr and fd to handler_entry
				   echo $Entry menu
			LOGIN: pass cliaddr and fd to handler_login
				   if ID, PWD both in database:
				   		set a ClientHandler in $map_clihandlers
				   		echo True
				   else:
				   		echo False
			REGISTER: pass cliaddr and fd to handler_register
					if ID, PWD both not in database:
						store the ID, PWD in database
						echo True
					else:
						echo False
		endcase

	Login: (cliaddr in map_clihandlers)
		Clienthandler clihandler = map_clihandlers[cliaddr];
		case (msg[OP])
			VISIT: pass cliaddr and fd to handler_menu
					echo $Main menu
			SHOW USER: pass cliaddr and fd to handler_showuser
					echo $User List
			SHOW ARTICLES: pass cliaddr and fd to handler_showarticles
					echo $Article List
			ADD ARTICLE: pass cliaddr and fd to handler_addarticle
					create a "Article folder" in server and name it by $ArticleID
					this instruction will make Clienthandler experienced the following state transitions
					STATE_RECV_INSTR => STATE_RECV_ARTICLE => STATE_RECV_FILE => STATE_RECV_INSTR
			ENTER ARTICLE: pass cliaddr and fd to handler_enterarticle
					$ArticleID = msg[ID]
					if $ArticleID found in "Article base":
						$Info = read from info file
						$Blacklist = $Info.blacklist
						if $clihandler.id in $Blacklist
							$Permission |= BLACK
						$Permission |= Permission($);

						if $Permission & BLACK is true:
							echo Article(); (that is empty article)
						else:
							echo Article(ArticleID,
										 PublisherID,
										 PublishingDate,
										 AttachFiles List); (article will load text content when contsturcting)
					else:
						$echo $MSG_ARTICLE_NOT_FOUND
			YELL: pass cliaddr and fd to handler_yell
					$Message = msg[message]
					push $Message to $yell_queue
					each polling cycle, server will check the queue.
					If the queue is not empty, server will pop message and broadcast message to all Clienthandler in map_clihandlers until the queue is empty
			TELL: pass cliaddr and fd to handler_tell
					$Message = msg[message]
					push $Message to $tell_queue
					like YELL case, but serer only send message to the specific client
			LOGOUT: pass clliaddr and fd handler_tell
					server will delete the Clienthandler from map_clihandlers
					then, this "cliaddr" will return to the not-yet login state
			default:
					echo $MSG_UNKNOWN_OP
		endcase
**/

#ifndef _UDPSERVER_H_
#define _UDPSERVER_H_

#include "network.h"

class Udpserver {
public:
	Udpserver(in_addr_t, port_t);
	Udpserver();
	~Udpserver();
	bool recv(void*, size_t, handle_func);
	bool send_msg(const char*, size_t, SIN*);
	bool send_msg(const std::string&, SIN*);
	bool send_msg(const char*, SIN*);
	bool send_ack(SIN* cliaddr);
	bool send_nak(SIN* cliaddr);
	sockfd_t getSockfd();
	void shutdown();
private:
	sockfd_t sockfd;
	port_t port;
	SIN servaddr;
};

#endif