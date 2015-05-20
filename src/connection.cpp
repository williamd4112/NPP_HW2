#include "connection.h"

Connection::Connection(Udpserver& server, ConnectionManager& manager, SIN addr):
	parent(server),
	manager(manager),
	src_addr(addr),
	login_state(false),
	state(RECV_TEXT) {

}

Connection::~Connection() {

}

std::string 
Connection::getAddr_p(){
	std::stringstream ss;
	char ip_str[IPV4_ADDRLEN + 1];
	inet_ntop(AF_INET, &src_addr.sin_addr, ip_str, sizeof(ip_str));

	ss << ip_str << ":" << ntohs(src_addr.sin_port);

	return ss.str();
}

bool 
Connection::isTimeout() {
	time_t cur = time(NULL);
	// std::cout << "Time: " << difftime(cur, last_time) << "\t" << getAddr_p() << std::endl;
	return (difftime(cur, last_time) > TIMEOUT);
}

bool 
Connection::isLogin() {
	return login_state;
}

void 
Connection::translate(Connection::State s) {
	state = s;
}

void
Connection::save_state() {
	last_state = state;
}

void 
Connection::reset_timer() {
	last_time = time(NULL);
}

void 
Connection::send_msg(const char *msg) {
	memcpy(sendbuff, msg, strlen(msg));
	buffsize = strlen(msg);
	save_state();
	translate(RECV_ACK);
	send_buff();
	manager.addWaiting(*this);
}

void 
Connection::send_buff() {
	parent.send_msg(sendbuff, buffsize, &src_addr);
	reset_timer();
}

void 
Connection::retransmit() {
	send_buff();
}

ConnectionManager::ConnectionManager(Udpserver& server): parent(server) {

}

ConnectionManager::~ConnectionManager() {

}

bool ConnectionManager::isExist(SIN addr) {
	return (conn_map.find(addr) != conn_map.end());
}


Connection& ConnectionManager::getConnection(SIN& addr) {
	std::map<SIN, Connection>::iterator it;
	if ((it = conn_map.find(addr)) != conn_map.end())
		return it->second;
	else
		throw KEY_NOFOUND;
}

void ConnectionManager::addConnection(SIN& addr) {
	std::map<SIN, Connection>::iterator it;
	if ((it = conn_map.find(addr)) == conn_map.end())
		conn_map.insert(ConnectionRecord(addr, Connection(parent, *this, addr)));
	else
		throw CONN_DUP;
}

void ConnectionManager::removeConnection(SIN& addr) {
	std::map<SIN, Connection>::iterator it;
	if ((it = conn_map.find(addr)) != conn_map.end())
		conn_map.erase(it);
	else
		throw KEY_NOFOUND;
}

void ConnectionManager::addWaiting(Connection& conn) {
	std::cout << conn.getAddr_p() << " add to wait" << std::endl;
	waitting_conn.push_back(conn);
}

void ConnectionManager::removeWaiting(Connection& conn) {
	waitting_conn.remove(conn);
}

void ConnectionManager::pollWaiting(){
	for(std::list<Connection>::iterator it = waitting_conn.begin(); it != waitting_conn.end(); it++){
		if(it->isTimeout()){
			it->retransmit();
		}
	}
}