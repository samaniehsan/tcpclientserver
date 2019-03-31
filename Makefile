all:
	g++ server.cpp -o server
	g++ client.cpp -o client
server:
	g++ server.cpp -o server
client:
	g++ client.cpp -o client
clean:
	rm -f server client *.o

