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
  if(argc < 2) {
    cout<<"command line arguments needed for running server {port}."<<endl;
    cout<<"example:./server 10999"<<endl;
    return 1;
  }
  int nPort = atoi(argv[1]);
  unsigned short maxPort = 0xFFFF;
  if(nPort > maxPort) {
    cout<<"Port:"<<nPort<<" is larger than maximum allowed "<<maxPort<<"."<<endl;
    return 2;
  }
  unsigned short port = nPort;

  int welcomeSocket, newSocket;
  char buffer[1024];
  struct sockaddr_in serverAddr;
  struct sockaddr_storage serverStorage;
  socklen_t addr_size;

  /*---- Create the socket. The three arguments are: ----*/
  /* 1) Internet domain 2) Stream socket 3) Default protocol (TCP in this case) */
  welcomeSocket = socket(PF_INET, SOCK_STREAM, 0);
  
  /*---- Configure settings of the server address struct ----*/
  /* Address family = Internet */
  serverAddr.sin_family = AF_INET;
  /* Set port number, using htons function to use proper byte order */
  serverAddr.sin_port = htons(port);
  /* Set IP address to localhost */
  serverAddr.sin_addr.s_addr = inet_addr("127.0.0.1");
  /* Set all bits of the padding field to 0 */
  memset(serverAddr.sin_zero, '\0', sizeof serverAddr.sin_zero);  

  /*---- Bind the address struct to the socket ----*/
  int nCode = bind(welcomeSocket, (struct sockaddr *) &serverAddr, sizeof(serverAddr));

  if(nCode != 0) {
    printError("Binding");
    return 3;
  }
  /*---- Listen on the socket, with 5 max connection requests queued ----*/
  nCode = listen(welcomeSocket,1);
  if(nCode==0)
    cout<<"Listening"<<endl;
  else {
    printError("Binding");
    return 4;
  }

  /*---- Accept call creates a new socket for the incoming connection ----*/
  addr_size = sizeof serverStorage;
  newSocket = accept(welcomeSocket, (struct sockaddr *) &serverStorage, &addr_size);

  if(newSocket >= 0) {
  /*---- Send message to the socket of the incoming connection ----*/
    strcpy(buffer,"Hello World\n");
    int nSent = send(newSocket,buffer,13,0);
    if(nSent >= 0) {
      close(newSocket);
      close(welcomeSocket);
    } else {
      printError("sending");
      close(newSocket);
      close(welcomeSocket);
      return 6;  
    }
  } else {
    close(welcomeSocket);
    printError("Accepting");
    return 5;
  }
  
  return 0;
}
