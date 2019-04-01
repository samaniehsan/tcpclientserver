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

ServerChannel::ServerChannel(
  const unsigned short int nPort
) {

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
 serverSocket = socket(PF_INET, SOCK_STREAM, 0);
 initServerAddress(nPort);
}

ServerChannel::~ServerChannel() {
  close(sererSocket);
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
  if(nCode==0)
    cout<<"Listening"<<endl;
    receive();
  else {
    SocketHelper::printError("Binding");
    return 4;
  }
}

int ServerChannel::receive() {
    /*---- Accept call creates a new socket for the incoming connection ----*/
  socklen_t addr_size = sizeof serverStorage;
  struct sockaddr_storage serverStorage;

  for(;;) {
    int clientSocket = accept(serverSocket, (struct sockaddr *) &serverStorage, &addr_size);

    if(newSocket >= 0) {
    /*---- Send message to the socket of the incoming connection ----*/
        RequestHeader header = {0};
        int nRead = recv(clientSocket, header, sizeof(RequestHeader), 0);
        strcpy(buffer,"Hello World\n");
        int nSent = send(clientSocket,buffer,13,0);
        if(nSent >= 0) {
            close(newSocket);
        } else {
        SocketHelper::printError("sending");
        close(newSocket);
        close(welcomeSocket);
        return 6;  
        }
    } else {
        close(welcomeSocket);
        SocketHelper::printError("Accepting");
        return 5;
    }
  }
  
}

int ServerChannel::getData(int clientSocket, Action_Code code) {;
    switch(code) {
        case Action_Code_Display_All:
            RequestHeader header = {Action_Code_Display_All};
            send(clientSocket,&header,sizeof(header),0);
            size_type n = studentDatabase.size();
            send(clientSocket,&n,sizeof(size_type),0);
            for (std::map<int, Student>::iterator it=studentDatabase.begin(); it!=studentDatabase.end(); ++it) {
                send(clientSocket,it->second,sizeof(Student), 0);
            }
            break;
        case Action_Code_Display_Score:
            ScoreRequest header = {0};
            int nRead = recv(clientSocket, header, sizeof(ScoreRequest), 0);
            std::map<int, Student> tempDb;
            for (std::map<int, Student>::iterator it=studentDatabase.begin(); it!=studentDatabase.end(); ++it) {
                if(it.score > header.score) {
                    tempDb.insert(pair<int, Student>(it->first, it->second)); 
                }
            }
            size_type n = tempDb.size();
            send(clientSocket,&n,sizeof(size_type),0);
            for (std::map<int, Student>::iterator it=tempDb.begin(); it!=tempDb.end(); ++it) {
                send(clientSocket,it->second,sizeof(Student), 0);
            }
            break;
        case Action_Code_Display_Id:
            StudentRequest studentRequest = {0};
            int nRead = recv(clientSocket, studentRequest, sizeof(StudentRequest), 0);
            std::map<char,int>::iterator it;
            it = mymap.find(studentRequest.studentId);

            if (it != mymap.end()) {
                size_type n = 1;
                send(clientSocket,n,sizeof(size_type), 0);
                send(clientSocket,it->second,sizeof(Student), 0);
            }

            break;
        case Action_Code_Display_Add:
            StudentRequest studentRequest = {0};
            int nRead = recv(clientSocket, header, sizeof(RequestHeader), 0);
            break;
        case Action_Code_Display_Delete:
            StudentRequest studentRequest = {0};
            int nRead = recv(clientSocket, header, sizeof(RequestHeader), 0);
            break;
    }
    return 6;
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
  this->serverAddr.sin_addr.s_addr = inet_addr(szAddr);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);
}
