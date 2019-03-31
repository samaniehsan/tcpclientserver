#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

using namespace std; 
extern int errno;

int printError(const char * errTitle) {
  char * pszErrorString = strerror(errno);
  cout<<"Eror "<<errTitle<<":"<<pszErrorString<<endl;
}

int main(int argc, char *argv[]){
  if(argc < 3) {
    cout<<"command line arguments needed for client to connect to server {ip} {port}."<<endl;
    cout<<"example:./client 127.0.0.1 10999"<<endl;
    return 1;
  }
  //ipv6 maximum plus null terminator
  char szAddr[46] = {0}; 
  strcpy(&szAddr[0], argv[1]);

  int nPort = atoi(argv[2]);
  unsigned short maxPort = 0xFFFF;
  if(nPort > maxPort) {
    cout<<"Port:"<<nPort<<" is larger than maximum allowed "<<maxPort<<"."<<endl;
    return 2;
  }
  unsigned short port = nPort;

  int clientSocket;
  struct sockaddr_in serverAddr;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  clientSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(port);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr(szAddr);
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Connect the socket to the server using the address struct ----*/
  socklen_t addr_size = sizeof serverAddr;
  int nCode = connect(clientSocket, (struct sockaddr *) &serverAddr, addr_size);
  if (nCode == 0 ) {
    char buffer[1024];
    /*---- Read the message from the server into the buffer ----*/
    int nRead = recv(clientSocket, buffer, 1024, 0);

    if(nRead >= 0) {
      close(clientSocket);
      /*---- Print the received message ----*/
      cout<<"Data received: "<<buffer<<endl;
    } else {
      printError("receiving");
      close(clientSocket);
    }
    return 0;
  } else {
      printError("Connecting");
      return 3;
  }
}