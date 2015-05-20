CC = g++
STD = -std=c++11
SERV_SRC = src/server.cpp src/udpserver.cpp src/network.cpp src/BBSServer.cpp src/account_manager.cpp src/util.cpp src/connection.cpp
ARTICLE_SRC = src/article.cpp
CLI_SRC = src/client.cpp src/udpclient.cpp src/network.cpp src/util.cpp
SERV_OUT = bin/server/server
CLI_OUT = bin/client/client

all: $(SERV_SRC) $(CLI_SRC)
	$(CC) -o $(SERV_OUT) $(SERV_SRC) $(ARTICLE_SRC) $(STD)
	$(CC) -o $(CLI_OUT) $(CLI_SRC) $(STD)