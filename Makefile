all: server client
server:
	g++ server.cpp -o server
client:
	g++ client.cpp -o client
clean:
	rm -f server client *.o
