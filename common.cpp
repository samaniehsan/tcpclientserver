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
  std::cerr<<"Error"<<" "<<errTitle<<":"<<pszErrorString<<endl;
}

bool
SocketHelper::addStudent(
  int socket,
  unsigned int studentId,
  const char * firstName,
  const char * lastName,
  BYTE score) {
    RequestHeader header = {Action_Code_Display_Add};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        Student student;
        student.studentId = studentId;
        strncpy(student.firstName,firstName,11);
        strncpy(student.lastName,lastName,11);
        student.score = score;
        int nCode = send(socket,&student,sizeof(Student),0);
        if(nCode != -1) {
            return true;
        } else {
            SocketHelper::printError("addStudent_2");
        }
    }
    SocketHelper::printError("addStudent_1");
    return false;
}



bool SocketHelper::deleteStudent(int socket,  unsigned int studentId) {
    RequestHeader header = {Action_Code_Display_Delete};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        StudentRequest student = {studentId};
        int nCode = send(socket,&student,sizeof(Student),0);
        if(nCode != -1) {
            return true;
        } else {
            SocketHelper::printError("deleteStudent_2");
        }
    }
    SocketHelper::printError("deleteStudent_1");
    return false;
}


bool
SocketHelper::sendAllStudentsRequest(int socket) {
    RequestHeader header = {Action_Code_Display_All};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        return true;
    }
    SocketHelper::printError("sendAllStudentsRequest");
    return false;
}

bool SocketHelper::sendStudentsByScoreRequest(int socket, BYTE score) {
    RequestHeader header = {Action_Code_Display_Score};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        ScoreRequest request = {score};
        nCode = send(socket,&request,sizeof(ScoreRequest),0);
        if(nCode != -1) {
            return true;
        }
    }
    SocketHelper::printError("sendStudentsByScoreRequest");
    return false;
}

bool SocketHelper::sendStudentsByStudentIdRequest(int socket,const unsigned int studentId) {
    RequestHeader header = {Action_Code_Display_Id};
    int nCode = send(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        StudentRequest request = {.studentId=studentId};
        nCode = send(socket,&request,sizeof(StudentRequest),0);
        if(nCode != -1) {
            return true;
        }
    }
    SocketHelper::printError("sendStudentsByStudentIdRequest");
    return false;
}


bool SocketHelper::receiveActionCode(int socket, Action_Code& actionCode) {
    RequestHeader header = {0};
    int nCode = recv(socket,&header,sizeof(RequestHeader),0);
    if(nCode != -1) {
        actionCode = (Action_Code)header.code;
        if(actionCode > 0 ) {
            return true;
        }
        std::cerr<<"wrong action code:"<<actionCode<<endl;
    }
    SocketHelper::printError("receiveActionCode");
    return false;
}

bool SocketHelper::receiveStudentsCount(int socket, int & nCount) {
    int n = 0;
    int nCode = recv(socket,&n,sizeof(int),0);
    if(nCode != -1) {
        nCount = n;
        return true;
    }
    SocketHelper::printError("receiveStudentsCount");
    return false;
}

bool SocketHelper::receiveStudentInfo(int socket,Student & student) {
    int nCode = recv(socket,&student,sizeof(Student),0);
    if(nCode != -1) {
        return true;
    }
    SocketHelper::printError("receiveStudentInfo");
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