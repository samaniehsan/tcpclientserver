#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "common.h"

#include "clientchannel.h"

using namespace std;

ClientChannel::ClientChannel(
  const char * szAddr,
  const unsigned short int nPort
) {

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
 clientSocket = socket(PF_INET, SOCK_STREAM, 0);
 initServerAddress(&szAddr[0],nPort);
}

ClientChannel::~ClientChannel() {
  close(clientSocket);
}

bool ClientChannel::addStudents(int clientSocket) {
  const int nStudents= 10;
  Student students[nStudents] = {
      {100000,"John","Smith",10},
      {100001,"Jane","Moore",20},
      {100002,"Alex","Holland",30},
      {100003,"Alex","Bear",40},
      {100004,"Cody","White",50},
      {100005,"Kyle","Black",60},
      {100006,"Shannon","O'Reilly",70},
      {100007,"Tim","Duncan",80},
      {100008,"Manu","Ginobili",90},
      {100009,"Tony","Parker",100}     
  };
  int i;
  for(i=0; i<nStudents; i++) {
    const Student & student = students[i];
    if(!SocketHelper::addStudent(
      clientSocket,
      student.studentId, 
      student.firstName,
      student.lastName,
      student.score)) {
        return false;
      }
  }
  return true;
}

int ClientChannel::run(
) {
  /*---- Connect the socket to the server using the address struct ----*/
  socklen_t addr_size = sizeof (serverAddr);
  int nCode = connect(
    clientSocket,
    (struct sockaddr *) &serverAddr,
    addr_size);
  if (nCode == 0 ) {
    if(!addStudents(clientSocket)) {
      char buffer[1024];
      /*---- Read the message from the server into the buffer ----*/
      int nRead = recv(clientSocket, buffer, 1024, 0);

      if(nRead >= 0) {
        close(clientSocket);
        /*---- Print the received message ----*/
        cout<<"Data received: "<<buffer<<endl;
      } else {
        SocketHelper::printError("receiving");
        close(clientSocket);
      }
      return 0;
    }    
  } else {
      SocketHelper::printError("Connecting");
      return 3;
  }
}

void ClientChannel::initServerAddress(
  const char * szAddr,
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
