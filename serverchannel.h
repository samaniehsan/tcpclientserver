#include <sys/socket.h>
#include <netinet/in.h>
#include <map>
#include <vector> 

#include "common.h"

typedef std::map<unsigned int, Student> STUDENT_MAP;
class ServerChannel {
public:
    ServerChannel(const unsigned short port);
    ~ServerChannel();
    int run();

private:
    bool addStudents(int clientSocket);
    int run(
    int clientSocket,
    const sockaddr_in& serverAddr
    );
    void initServerAddress(const unsigned short int port);
    
    int handleDisplayAll(int clientSocket);
    int handleDisplayScore(int clientSocket);
    int handleDisplayStudentId(int clientSocket);
    int handleAdd(int clientSocket);
    int handleDelete(int clientSocket);    
    int receive();
    int readData(int clientSocket, Action_Code code);
    void printStudents(const std::vector<Student>& studentList);
private:
int serverSocket;
std::vector<int> clientSockets;
sockaddr_in serverAddr;
STUDENT_MAP studentDatabase; 
};
