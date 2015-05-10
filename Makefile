CC = g++
SERVER_SRC = server.cpp udpserver.cpp clienthandler.cpp bbs.cpp article.cpp network.cpp
SERVER_OUT = bin/server/server

CLIENT_SRC = client.cpp udpclient.cpp bbs.cpp article.cpp network.cpp
CLIENT_OUT = bin/client/client

STD = -std=c++11

all: $(SERVER_SRC)
	$(CC) -D LOG -o $(SERVER_OUT) $(SERVER_SRC) $(STD)
	$(CC) -D LOG -o $(CLIENT_OUT) $(CLIENT_SRC) $(STD)