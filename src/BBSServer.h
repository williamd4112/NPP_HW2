#ifndef _BBSSERVER_H_
#define _BBSSERVER_H_

#include <map>
#include <vector>
#include "udpserver.h"
#include "account_manager.h"
#include "connection.h"
#include "util.h"
#include "network.h"
#include "article.h"

extern const char *SCREEN_CLEAR_CTRL;
extern const char *RETURN_OPTION;
extern const char *ACCOUNTS_FILENAME;
extern const char *ENTRY_MENU;
extern const char *MAIN_MENU;
extern const char *ARTILCELIST_MENU;
extern const char *ARTICLE_ENTER_IDERROR;
extern const char *ARTICLE_EDIT_MENU;
extern const char *ARTICLE_APPEND_BLACK_SUCCEED;
extern const char *ARTICLE_APPEND_BLACK_FAILED;
extern const char *ARTICLE_DEL_BLACK_SUCCEED;
extern const char *ARTICLE_DEL_BLACK_FAILED;
extern const char *RESPONSE_SUCCESS;
extern const char *RESPONSE_FAILED;
extern const char *MSG_LOGIN_SUCCESS;
extern const char *MSG_LOGIN_UID_ERROR;
extern const char *MSG_LOGIN_PWD_ERROR;
extern const char *MSG_LOGOUT;
extern const char *MSG_REG_SUCCESS;
extern const char *MSG_REG_DUP;
extern const char *MSG_REG_INVALIDARGS;


class BBSServer;
typedef void (BBSServer::*text_func) (Connection&, std::vector<String>);
typedef struct text_func_record {
	const char* key;
	text_func func;
} text_func_record;

extern text_func_record GUEST_FUNC_MAP[];
extern text_func_record USER_FUNC_MAP[];

class BBSServer: public Udpserver {
public:
	BBSServer(in_addr_t, port_t);
	~BBSServer();

	void recv();
	void pollWaiting();

	void state_recv_ack(Connection& conn, void *buff, size_t size);
	void state_recv_text(Connection& conn, void *buff, size_t size, text_func_record* func_table);

	void handle_entry(int, struct sockaddr_in, void *, size_t);

	void handle_guest(int, struct sockaddr_in, void *, size_t);
	void guest_register(Connection& conn, std::vector<String> args);
	void guest_login(Connection& conn, std::vector<String> args);
	void guest_echo(Connection& conn, std::vector<String> args);
	void guest_return(Connection& conn, std::vector<String> args);

	void handle_user(int, struct sockaddr_in, void *, size_t);
	void user_return(Connection& conn, std::vector<String> args);
	void user_show_users(Connection& conn, std::vector<String> args);
	void user_show_articles(Connection& conn, std::vector<String> args);
	void user_yell(Connection& conn, std::vector<String> args);
	void user_tell(Connection& conn, std::vector<String> args);
	void user_logout(Connection& conn, std::vector<String> args);

	// For testing
	void handle_echo(int, struct sockaddr_in, void *, size_t);

	// Error handling
	void connectionManager_err(ConnectionManager::Exception e);
	void accountManager_err(AccountManager::Exception e);
	void util_err(Util::Exception e);

private:
	char recvbuff[MAXBUFF];
	AccountManager account_manager;
	ConnectionManager conn_manager;

};

#endif