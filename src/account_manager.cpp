#include "account_manager.h"

AccountManager::AccountManager(const char *filename){
	account_filename = std::string(filename);
	if(!load(filename))
		std::cerr << "AccountManager(): load initial data error." << std::endl;
}

AccountManager::AccountManager(){

}

AccountManager::~AccountManager(){

}

bool
AccountManager::isExist(const char *id){
	return isExist(std::string(id));
}

bool
AccountManager::isExist(std::string id){
  return (account_table.find(id) != account_table.end());
}

bool
AccountManager::isCorrect(std::string id, std::string pwd){
  if(isExist(id))
    if(account_table.find(id)->second == pwd)
      return true;
  return false;
}

void
AccountManager::insert(const char *id, const char *pwd){
  insert(std::string(id), std::string(pwd));
}

void
AccountManager::insert(std::string id, std::string pwd){
  if(!isExist(id)){
    account_table.insert(AccountRecord(id, pwd));
    update(id, pwd);
  }
  else
    throw ERR_DUPID;
}

void
AccountManager::remove(const char *id){
	AccountTable::iterator it;
	if((it = account_table.find(std::string(id))) != account_table.end()){
		account_table.erase(it);
		update();
	}
	else
		throw ERR_NULL;
}

void 
AccountManager::update(){
	if(!store(account_filename.c_str()))
		throw ERR_IO;
}

void 
AccountManager::update(const char *id, const char *pwd){
	if(!store(account_filename.c_str(), id, pwd))
		throw ERR_IO;
}

void 
AccountManager::update(std::string id, std::string pwd){
  update(id.c_str(), pwd.c_str());
}

bool
AccountManager::load(const char *filename){
	std::ifstream ifs;
    ifs.open(filename, std::ios::in);
    if(!ifs.good()) return false;
    std::string id, pwd;
    while(ifs >> id >> pwd){
    	account_table.insert(AccountRecord(id, pwd));
    }
    ifs.close();
    return true;
}

bool 
AccountManager::store(const char *filename){
	std::ofstream ofs;
	ofs.exceptions (std::ifstream::failbit | std::ifstream::badbit );
  	try {
    	ofs.open(filename, std::ios::out | std::ios::trunc);
    	for(AccountTable::iterator it = account_table.begin(); it != account_table.end(); it++){
    		ofs << it->first << " " << it->second << std::endl;
    	}
    	return true;
  	}
  	catch (std::ifstream::failure e) {
    	std::cerr << "account_manager.load(): Load account data error" << std::endl;
    	ofs.close();
    	return false;
  	}
}

bool 
AccountManager::store(const char *filename, const char *id, const char *pwd){
	std::ofstream ofs;
	ofs.exceptions (std::ifstream::failbit | std::ifstream::badbit );
  	try {
    	ofs.open(filename, std::ios::out | std::ios::app);
    	ofs << id << " " << pwd << std::endl;
    	ofs.close();
    	return true;
  	}
  	catch (std::ifstream::failure e) {
    	std::cerr << "account_manager.loac(): Load account data error" << std::endl;
    	ofs.close();
    	return false;
  	}
}

