.PHONY:all
all:clientTcp serverTcpd
Method=#-DMY_SELF

clientTcp:tcp_client.cc
	g++ -o $@ $^ $(Method) -std=c++11 -ljsoncpp
serverTcpd:tcp_server.cc
	g++ -o $@ $^ $(Method) -std=c++11 -lpthread -ljsoncpp

.PHONY:clean
clean:
	rm -f clientTcp serverTcpd