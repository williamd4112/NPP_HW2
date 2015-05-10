#include "bbs.h"


const char *ACCOUNTS_FILENAME = "accounts";
const char *ARTICLE_BASE = "articles";
const char *ENTRY_MENU = "\x01[R]egister\t[L]ogin\n";
const char *MAIN_MENU = "\x01[SU]sers\t[SA]rticles\t[Y]ell\t[T]ell\t[O]Logout\n";
const char *ARTILCELIST_MENU = "[RT]urn\t[E]nter Article\t[A]dd Article\t";
const char *ARTICLE_ENTER_IDERROR = "\x02Enter failed: no such article.\n";
const char *ARTICLE_MENU_OWNER = "[RT]urn\t[U]pload\t[D]ownload\t\n[AB]add black\t[DB]del black";
const char *ARTICLE_MENU_NORMAL = "[RT]urn\t[R]esponse\t[D]ownload\t";
const char *ARTICLE_EDIT_MENU = "\x01[ESC]Send\n";
const char *ARTICLE_BLACK_MENU = "[RT]urn\n";
const char *ARTICLE_APPEND_BLACK_SUCCEED = "\x01 Add black user ID succeed !\n[RT]urn\n";
const char *ARTICLE_APPEND_BLACK_FAILED = "\x01 Add black user ID failed\n[RT]urn\n";
const char *ARTICLE_DEL_BLACK_SUCCEED = "\x01 Del black user ID succeed !\n[RT]urn\n";
const char *ARTICLE_DEL_BLACK_FAILED = "\x01 Del black user ID failed\n[RT]urn\n";
const char *RESPONSE_SUCCESS = "\x01Response succeed !\n[RT]urn\n";
const char *RESPONSE_FAILED = "\x01Error: response failed\n[RT]urn\n";
const char *MSG_LOGIN_SUCCESS = "\x01Login succeess\n[RT]urn\n";
const char *MSG_LOGIN_UID_ERROR = "\x01Login failed: No such user ID\n[RT]turn\n";
const char *MSG_LOGIN_PWD_ERROR = "\x01Login failed: Password wrong\n[RT]turn\n";
const char *MSG_LOGOUT = "\x01Logout succeessfully\n[RT]urn\n";
const char *MSG_REG_SUCCESS = "\x01Register succeess\n[RT]turn\n";
const char *MSG_REG_DUP = "\x01Register failed: Duplicated User ID\n[RT]turn\n";
const char *MSG_REG_INVALIDARGS = "\x01Register failed: Invalid Arguments\n[RT]turn\n";

const std::string OP_LIST[] = {"", "V", "R", "L", "Y", "T", "SU", "SA", "A", "E", "RP", "D", "U", "AB", "DB", "O", "RT"};
const char D_TYPES[] = {OP_REGISTER, 
						OP_LOGIN, 
						OP_TELL,
						0x0};

const char S_TYPES[] = {OP_ADD_ARTICLE, 
						OP_ENTER_ARTICLE, 
						OP_YELL, 
						OP_RESPONSE, 
						OP_DOWNLOAD_ATTACH, 
						OP_UPLOAD_ATTACH, 
						OP_ADD_BLACK, 
						OP_DEL_BLACK,
						0x0};

const char Z_TYPES[] = {OP_RETURN,
						OP_VISIT,
						OP_SHOWUSER,
						OP_SHOWARTICLE,
						OP_LOGOUT,
						0x0};

std::vector<std::string> split(const std::string& source){
	std::stringstream ss(source);
    std::vector<std::string> tokens((std::istream_iterator<std::string>(ss)),
                     				 std::istream_iterator<std::string>());

    return tokens;
}

char decode_op(std::string& opcode){
	for(int i = 0; i < OPSET_SIZE; i++){
		if(opcode == OP_LIST[i])
			return (char)i;
	}
	return -1;
}