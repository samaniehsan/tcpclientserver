#include <sys/socket.h>
#include <netinet/in.h>
#include <map> 

#include "common.h"

class ServerChannel {
public:
    ServerChannel(const unsigned short port);
    ~ServerChannel();

    bool addStudents(int clientSocket);
    int run(
    int clientSocket,
    const sockaddr_in& serverAddr
    );
    void initServerAddress(
    const char * szAddr,
    const unsigned short int port
    );
    int run();
private:
int serverSocket;
sockaddr_in serverAddr;
std::map<int, Student> studentDatabase; 
};
