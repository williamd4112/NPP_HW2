#include "article.h"

const char *ARTICLE_BASE = "articles";
const char *ARTICLE_ATTACHMENTS = "attachments";
const char *ARTICLE_MENU_OWNER = "[RT]urn\t[U]pload\t[D]ownload\t\n[AB]add black\t[DB]del black";
const char *ARTICLE_MENU_NORMAL = "[RT]urn\t[R]esponse\t[D]ownload\t";
const char *ARTICLE_BLACK_MENU = "[RT]urn\n";

std::string MSG_PERMISSION_DENIED = "Permission denied";

Articleheader::Articleheader(const char *article_dir){
	std::string filename(article_dir);
	filename = filename + "/info";
	
	std::cout << "Load " << filename << std::endl;
	std::ifstream ifs(filename.c_str(), std::ios::in);
	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if(!ifs.good()) throw EID_CREATEARTICLE;

	try{
		getline(ifs, aid);
		getline(ifs, title);
		getline(ifs, pid);
		getline(ifs, date);
	
		int bc;
		ifs >> bc;
		for(int i = 0; i < bc; i++){
			std::string bid;
			getline(ifs, bid);
			blacklist.push_back(bid);
		}

		int fc;
		ifs >> fc;
		for(int i = 0; i < fc; i++){
			std::string fname;
			getline(ifs, fname);
			fileslist.push_back(fname);
		}
	}catch(std::ifstream::failure e){
		std::cerr << "Article(): read error" << std::endl;
		ifs.close();
	}
	ifs.close();

	loadBlacklist(article_dir);
}

Articleheader::Articleheader(){

}

Articleheader::~Articleheader(){

}

std::string Articleheader::toString(){
	char buff[HEADER_LEN];
	snprintf(buff, HEADER_LEN, "%-10s\t%-10s\t%-40s\t%-20s",
			aid.c_str(),
			pid.c_str(),
			title.c_str(),
			date.c_str());
	return std::string(buff);
}

void
Articleheader::loadBlacklist(const char *article_dir){
	char blacklist_path[FILENAME_SIZE];
	snprintf(blacklist_path, FILENAME_SIZE, "%s/%s",article_dir, "blacklist");

	std::fstream fs(blacklist_path, std::ios::in);
	if(fs.bad()) throw EID_FILEOPEN;
	
	std::string line;
	while(getline(fs, line) != NULL)
			blacklist.push_back(line);
	fs.close();
}

void Article::init(const char *article_dir){
	this->article_dir = article_dir;

	header = Articleheader(article_dir);
	std::string filename(article_dir);
	filename = filename + "/context";
	
	std::cout << "Load " << filename << std::endl;
	std::ifstream ifs(filename.c_str(), std::ios::in);
	//ifs.exceptions(std::ifstream::failbit);

	if(!ifs.good()) throw EID_CREATEARTICLE;

	std::string line;
	while(getline(ifs, line) != NULL){
		context << line << std::endl;
	}

	ifs.close();
}

Article::Article(const char *article_dir){
	init(article_dir);
}

Article::Article(std::string aid, std::string title, std::string pid, std::string date){
	header.aid = aid;
	header.title = title;
	header.pid = pid;
	header.date = date;
}


Article::Article(std::string& aid){
	char buff[FILENAME_SIZE];
	snprintf(buff, FILENAME_SIZE, "%s/%s",ARTICLE_BASE, aid.c_str());
	init(buff);
}

Article::Article(){

}

Article::~Article(){

}

std::string& Article::getPid(){
	return header.pid;
}

std::string Article::toString(std::string vid){
	std::stringstream ss;
	if(std::find(header.blacklist.begin(), header.blacklist.end(), vid) != header.blacklist.end()){
		ss << MSG_PERMISSION_DENIED << std::endl << ARTICLE_BLACK_MENU;
		return ss.str();
	}

	ss << "Article ID: " << header.aid << std::endl
	   << "Title: " << header.title << std::endl
	   << "Publisher ID: " << header.pid << std::endl
	   << "Publihed Date: " << header.date << std::endl
	   << context.str() << std::endl
	   << "-------------------------------------------" << std::endl
	   << getResponse() << std::endl
	   << getAttachments() << std::endl;

	if(vid == header.pid)
		ss << ARTICLE_MENU_OWNER << std::endl;
	else
		ss << ARTICLE_MENU_NORMAL << std::endl;
	return ss.str();
}

std::string Article::getResponse(){
	std::string response_path = article_dir + "/response";
	std::ifstream ifs(response_path, std::ios::in);
	if(ifs.fail()) {
		ifs.close();
		return std::string("Response load error.");
	}

	std::stringstream ss;
	std::string line;
	while(getline(ifs, line) != NULL)
		ss << line << std::endl;
	ifs.close();
	return ss.str();
}

std::string Article::getAttachments(){
	std::string attachment_path = article_dir + "/" + ARTICLE_ATTACHMENTS;
	std::vector<std::string> fileslist = getFileslist(attachment_path.c_str());
	std::stringstream ss;
	ss << "Attachments:" << std::endl;
	for(std::vector<std::string>::iterator it = fileslist.begin(); it != fileslist.end(); it++)
		ss << "-" << *it << std::endl;

	return ss.str();
}

void Article::store(const char *article_dir){
	int flag = mkdir(article_dir, S_IRWXU | S_IRWXG | S_IROTH | S_IXOTH);
	if(flag != 0) 
		throw article_dir;


	// Write Info file
	header.store(article_dir);

	// Write Context
	std::string filename(article_dir);
	filename = filename + "/context";
	
	std::cout << "Store " << filename << std::endl;
	std::ofstream ifs(filename.c_str(), std::ios::out);
	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if(!ifs.good()) throw EID_CREATEARTICLE;

	try{
		ifs << context.str();
	}catch(std::ifstream::failure e){
		std::cerr << "Article(): read error" << std::endl;
	}

	ifs.close();

	// Wrtie Response
	std::string res_filename(article_dir);
	res_filename = res_filename + "/response";
	std::ofstream res_ofs(res_filename, std::ios::out);
	if(res_ofs.fail()) throw EID_CREATEARTICLE;
	res_ofs << "Response:" << std::endl;
	res_ofs.close();

}

void Article::createBlacklist(){
	std::string blacklist_path = article_dir + "/blacklist";
	std::ofstream ofs(blacklist_path, std::ios::out);
	if(ofs.bad()) throw EID_FILEOPEN;

	// ofs << "Blacklist" << std::endl;
	ofs.close();
}

void Article::appendBlacklist(std::string& bid){
	std::string blacklist_path = article_dir + "/blacklist";
	std::ofstream ofs(blacklist_path, std::ios::app);
	if(ofs.bad()) throw EID_FILEOPEN;
	std::cout << "Append black ID" << std::endl;
	ofs << bid << std::endl;
	ofs.close();
}

void Article::removefromBlacklist(std::string& bid){
	std::string blacklist_path = article_dir + "/blacklist";

	std::fstream fs(blacklist_path, std::ios::out | std::ios::trunc);

	if(fs.bad()) throw EID_FILEOPEN;
	for(std::vector<std::string>::iterator it = header.blacklist.begin();
		it != header.blacklist.end();
		it++){
		if(*it == bid)
			continue;
		fs << *it << std::endl;
	}
	fs.close();
}

void Article::store(){
	char buff[FILENAME_SIZE];
	snprintf(buff, FILENAME_SIZE, "%s/%s", ARTICLE_BASE, header.aid.c_str());
	std::cout << "DIR: " << buff << std::endl; 
	store(buff);
}

void Articleheader::store(const char *article_dir){
	// Write Header
	std::string filename(article_dir);
	filename = filename + "/info";
	
	std::cout << "Store " << filename << std::endl;
	std::fstream ifs(filename.c_str(), std::ios::out);
	ifs.exceptions(std::ifstream::failbit | std::ifstream::badbit);

	if(!ifs.good()) throw EID_CREATEARTICLE;

	try{
		ifs << aid << std::endl
			<< title << std::endl
			<< pid << std::endl
			<< date << std::endl;

		ifs << blacklist.size() << std::endl;
		for(std::vector<std::string>::iterator it = blacklist.begin();
			it != blacklist.end();
			it++)
			ifs << *it << std::endl;

		ifs << fileslist.size() << std::endl;
		for(std::vector<std::string>::iterator it = fileslist.begin();
			it != fileslist.end();
			it++)
			ifs << *it << std::endl;
	}catch(std::ifstream::failure e){
		std::cerr << "Article(): read error" << std::endl;
	}
}

/*
int main(){
	std::string id = "Williamd";
	Article a("bin/server/articles/1");
	std::cout << a.toString(id) << std::endl;
	return 0;
}
*/