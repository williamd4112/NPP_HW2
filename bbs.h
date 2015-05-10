/**
	File: bbs.h
	Description:
	this file define message transfering protocol between server and client

	Instruction format:
	Double Args:
		Register:
			R[0]
			<UID>
			<PWD>
		Login:
			L[0]
			<UID>
			<PWD>
		Tell:
			T[0]
			<UID>
			<MSG>
	
	Single Args:
		Add article:
			A[0]
			<Title>
		Enter article: 
			E[0]
			<AID>
		Yell:
			Y[0]
			<MSG>
		Response:
			R[0]
			<MSG>
		Download:
			D[0]
			<Filename>
		Upload:
			U[0]
			<Filename>
		Add blacklist:
			AB[0]
			<UID>
		Del blacklist:
			DB[0]
			<UID>
	
	Visit: 1 byte
		V[0]
	Return: 1 byte
		RT[0]
	Show user: 1 byte
		SU[0]
	Show articles: 1 byte
		SA[0]
	Logout: 1 byte
		O[0]
**/

#ifndef _BBS_H_
#define _BBS_H_

#include <cstring>
#include <iostream>
#include <vector>
#include <sstream>
#include <iterator>
#include <cstdlib>
#include <cstdio>
#include <map>

#define CTRL_CLEAR '\x01'
#define CTRL_APPEND '\x02'
#define ESCAPE '\x1b'

#define OP_VISIT 0x01
#define OP_REGISTER 0x02
#define OP_LOGIN 0x03
#define OP_YELL 0x04
#define OP_TELL 0x05
#define OP_SHOWUSER 0x06
#define OP_SHOWARTICLE 0x07
#define OP_ADD_ARTICLE 0x08
#define OP_ENTER_ARTICLE 0x09
#define OP_RESPONSE 0x0a
#define OP_DOWNLOAD_ATTACH 0x0b
#define OP_UPLOAD_ATTACH 0x0c
#define OP_ADD_BLACK 0x0d
#define OP_DEL_BLACK 0x0e
#define OP_LOGOUT 0x0f
#define OP_RETURN 0x10

#define OPSET_SIZE 17
#define UID_SIZE 10
#define PWD_SIZE 10
#define MSG_SIZE 80
#define TITLE_SIZE 80
#define FILENAME_SIZE 256
#define MAX_RESPONSE_LEN 256

#define EID_INVALIDARGS 0x1
#define EID_UIDDUPLICATED 0x2
#define EID_UIDNOTEXIST 0x3
#define EID_PWDERROR 0x4
#define EID_CRETEARTICLE 0x5

#define HEADER_LEN 120

extern const char *ACCOUNTS_FILENAME;
extern const char *ARTICLE_BASE;
extern const char *ENTRY_MENU;
extern const char *MAIN_MENU;
extern const char *ARTILCELIST_MENU;
extern const char *ARTICLE_ENTER_IDERROR;
extern const char *ARTICLE_MENU_OWNER;
extern const char *ARTICLE_MENU_NORMAL;
extern const char *ARTICLE_EDIT_MENU;
extern const char *ARTICLE_BLACK_MENU;
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

extern const std::string OP_LIST[];
extern const char D_TYPES[];
extern const char S_TYPES[];
extern const char Z_TYPES[];

enum Position{
	MAINMENU,
	USER_LIST,
	ARTICLE_LIST,
	ARTICLE_EDIT,
	ARTICLE,
	CONFIRM_PAGE
};

enum State{
	STATE_RECV_LOGIN_ACK,
	STATE_RECV_TEXT,
	STATE_RECV_ARTICLE,
	STATE_RECV_FILE,
	STATE_SEND_ARTICLE
};

std::vector<std::string> split(const std::string& source);
char decode_op(std::string& opcode);


#endif