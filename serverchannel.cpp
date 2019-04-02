#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

#include "serverchannel.h"

using namespace std;
//typedef STUDENT_MAP::size_type size_type;

ServerChannel::ServerChannel(
  const unsigned short int nPort
):serverSocket(-1){
  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
 serverSocket = socket(PF_INET, SOCK_STREAM, 0);
 initServerAddress(nPort);
}

ServerChannel::~ServerChannel() {
  if(serverSocket != -1 )  
    close(serverSocket);

  for (std::vector<int>::const_iterator it=clientSockets.begin(); it!=clientSockets.end(); it++) {    
    int clientSocket = *it;
    if(clientSocket != -1 )  
      close(clientSocket);
  }
}

void ServerChannel::initServerAddress(
  const unsigned short int port
) {
  
   /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  this->serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  this->serverAddr.sin_port = htons(port);
  /* Set IP address to localhost */
  this->serverAddr.sin_addr.s_addr = htonl(INADDR_ANY);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
}


int ServerChannel::run(
) {
  char buffer[1024];
  struct sockaddr_in serverAddr;
  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  
  /*---- Bind the address struct to the socket ----*/
  int nCode = bind(serverSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  if(nCode != 0) {
    SocketHelper::printError("Binding");
    return 3;
  }
  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  nCode = listen(serverSocket,1);
  if(nCode==0) {
    cout<<"Listening"<<endl;
    receive();
  } else {
    SocketHelper::printError("Binding");
    return 4;
  }
}

int ServerChannel::receive() {
    /*---- Accept call creates a new socket for the incoming connection ----*/
  socklen_t addr_size = sizeof (sockaddr_storage);
  struct sockaddr_storage serverStorage;

  for(;;) {
    int clientSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);
    clientSockets.push_back(clientSocket);

    if(clientSocket >= 0) {
    /*---- Send message to the socket of the incoming connection ----*/
        for(;;) {
          RequestHeader header = {0};
          int errorCode = recv(clientSocket, &header, sizeof(RequestHeader), 0);
          if(errorCode == -1) {
            SocketHelper::printError("Accepting");
            return 6;
          }
          errorCode = readData(clientSocket,(Action_Code)header.code);
          if(errorCode == -1) {
            SocketHelper::printError("Accepting");
            return 6;
          }
        }
        
    } else {
        SocketHelper::printError("Accepting");
        return 5;
    }
  }
  return 0;
}

int ServerChannel::handleDisplayAll(int clientSocket) {
  RequestHeader header = {.code=Action_Code_Display_All};
  int errorCode = send(clientSocket,&header,sizeof(header),0);
  if(errorCode == -1)
    SocketHelper::printError("DisplayStudentId_send1");
    return 7;
  int n = (int)studentDatabase.size();
  errorCode = send(clientSocket,&n,sizeof(int),0);
  if(errorCode == -1)
    SocketHelper::printError("DisplayStudentId_send2");
    return 8;
  for (STUDENT_MAP::iterator it=studentDatabase.begin(); it!=studentDatabase.end(); ++it) {
      errorCode = send(clientSocket,&it->second,sizeof(Student), 0);
      if(errorCode == -1)
        SocketHelper::printError("DisplayStudentId_send3");
        return 9;
  }
  return 0;
}

int ServerChannel::handleDisplayScore(int clientSocket) {
    ScoreRequest header = {};
    int errorCode = recv(clientSocket, &header, sizeof(ScoreRequest), 0);
    if(errorCode == -1)
      SocketHelper::printError("handleDisplayScore_recv");
      return 10;
    STUDENT_MAP tempDb;
    for (STUDENT_MAP::iterator it=studentDatabase.begin(); it!=studentDatabase.end(); ++it) {
        if(it->second.score > header.score) {
            tempDb.insert(pair<int, Student>(it->first, it->second)); 
        }
    }
    int n = (int)tempDb.size();
    errorCode = send(clientSocket,&n,sizeof(int),0);
    if(errorCode == -1)
      SocketHelper::printError("handleDisplayScore_recv");
      return 11;

    for (STUDENT_MAP::iterator it=tempDb.begin(); it!=tempDb.end(); ++it) {
        errorCode = send(clientSocket,&it->second,sizeof(Student), 0);
        if(errorCode == -1)
          SocketHelper::printError("handleDisplayScore_recv");
          return 12;
    }
  return 0;
}


int ServerChannel::handleDisplayStudentId(int clientSocket) {
  StudentRequest studentRequest = {0};
  int errorCode = recv(clientSocket, &studentRequest, sizeof(StudentRequest), 0);
  if(errorCode == -1)
    SocketHelper::printError("DisplayStudentId_recv");
    return 13;

  STUDENT_MAP::iterator it;
  it = studentDatabase.find(studentRequest.studentId);

  if (it != studentDatabase.end()) {
      int n = 1;
      errorCode = send(clientSocket,&n,sizeof(int), 0);
      if(errorCode == -1)
        SocketHelper::printError("DisplayStudentId_Send1");
        return 14;
      
      errorCode = send(clientSocket,&it->second,sizeof(Student), 0);
      if(errorCode == -1)
          SocketHelper::printError("DisplayStudentId_Send2");
        return 15;
  } else {
    int n = 1;
    errorCode = send(clientSocket,&n,sizeof(int), 0);
    
    if(errorCode == -1)
      SocketHelper::printError("DisplayStudentId_Send3");
      return 16;
  }
  return 0;
}

int ServerChannel::handleAdd(int clientSocket) {
  Student student = {0};
  int errorCode = recv(clientSocket, &student, sizeof(Student), 0);
  if(errorCode == -1) {
    SocketHelper::printError("handleAdd_Send");
    return 17;
  }
  STUDENT_MAP::iterator it = studentDatabase.find(student.studentId);
  if(it == studentDatabase.end()) {
    studentDatabase.insert(
      pair<unsigned int, Student>(
        student.studentId, student));
  } else {
    it->second = student;
  }
  return 0;
}

int ServerChannel::handleDelete(int clientSocket) {
  StudentRequest studentRequest = {0};
  int errorCode = recv(clientSocket, &studentRequest, sizeof(StudentRequest), 0);
  if(errorCode == -1) {
    SocketHelper::printError("handleDelete_Recv");
  }
  STUDENT_MAP::iterator it;
  it = studentDatabase.find(studentRequest.studentId);
  if (it != studentDatabase.end()) {
      studentDatabase.erase(it);
  }
  return 0;
}

int ServerChannel::readData(
  int clientSocket,
  Action_Code code
  ) {
    switch(code) {
        case Action_Code_Display_All:
            return handleDisplayAll(clientSocket);
            break;
        case Action_Code_Display_Score:
            return handleDisplayScore(clientSocket);
            break;
        case Action_Code_Display_Id:
            return handleDisplayStudentId(clientSocket);
            break;
        case Action_Code_Display_Add:
            return handleAdd(clientSocket);
            break;
        case Action_Code_Display_Delete:
            return handleDelete(clientSocket);
            break;
        default:
          return 6;
    }
    return 6;
}