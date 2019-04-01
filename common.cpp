#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <iostream>
#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include "common.h"

using namespace std; 
extern int errno;


int SocketHelper::printError(const char * errTitle) {
  char * pszErrorString = strerror(errno);
  cout<<"Eror "<<errTitle<<":"<<pszErrorString<<endl;
}

bool SocketHelper::addStudent(
  int socket,
  unsigned int studentId,
  const char * firstName,
  const char * lastName,
  BYTE score) {
    Student student;
    student.studentId = studentId;
    strncpy(student.firstName,firstName,11);
    strncpy(student.lastName,lastName,11);
    student.score = score;
    int nCode = send(socket,&student,sizeof(Student),0);
    if(nCode != -1) {
        return true;
    }
    SocketHelper::printError("send");
    return false;
}

/*
StudentRequest::StudentRequest() {
    studentId = 0;
}

StudentRequest::StudentRequest(unsigned int studentId) {
    this.studentId = studentId;
}

Student::Student() {
    studentId = 0;
    score = 0;
    memset(&firstName,0,11);
    memset(&firstName,0,11);
}

Student::Student(
    unsigned int studentId,
    const char * Fname,
    const char * Lname,
    BYTE score) {
    this.studentId = studentId;
    strncpy(this.firstName,firstName,11);
    strncpy(this.lastName,lastName,11);
    this.score = score;
}
*/