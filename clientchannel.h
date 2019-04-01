#include <sys/socket.h>
#include <netinet/in.h>

class ClientChannel {
public:
    ClientChannel(const char * szAddr, const unsigned short port);
    ~ClientChannel();

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
int clientSocket;
sockaddr_in serverAddr;
};
