all: server client
server:
	g++ server.cpp common.cpp -o server
client:
	g++ client.cpp common.cpp clientchannel.cpp -o client
clean:
	rm -f server client *.o
