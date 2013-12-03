//
//  ClientLib.h
//  Client
//
//  Created by Francesco Racciatti & Antonio Macrì on 01/12/13.
//  Copyright (c) 2013 Francesco Racciatti & Antonio Macrì. All rights reserved.
//

#ifndef Client_ClientLib_h
#define Client_ClientLib_h

#define CAST_ADDR(x) (struct sockaddr *) (x)

#include <iostream>
#include <string>
#include <sstream>
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <fstream>
#include <list>
#include <vector>

#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <stdlib.h>

#include <arpa/inet.h>
#include <unistd.h>
#include <fstream>
#include <errno.h>

//#include "boost/filesystem/operations.hpp"
//#include "boost/filesystem/path.hpp"


using namespace std;

struct server_s{
    string addr;
    string port;
};

struct cmd_s{
    string cmd;
    list <server_s> srv;
};

struct find_cmd : unary_function <cmd_s, bool>{
    string cmd;
    find_cmd(string cmd):cmd(cmd){}
    bool operator() (cmd_s const& c) const{
        return c.cmd == cmd;
    }    
};

class Client {
    
private:
    
    struct sockaddr_in serverAddress;
    struct hostent *server;
    int socketDescriptor;
    
    list <cmd_s> srvList;
    
public:
    
    string name;
    string data;
    server_s srv;
       
private:
    
    void openConnection(const char*, const char*);
    void closeConnection();
    
    void insertSrv(const char*, const char*, const char*);
    
    void connectReg(const char*, const char*);
    void disconnectReg();
    
    void connectSrv(const char*, const char*);
    void disconnectSrv();

    
public:
    Client();
    ~Client();

    bool locateSrv(const char*, const char*, const char*);
    bool checkSrvList(const char*);
    
    void srvReq(const char*, const char*, const char*);
    void srvReq(const char*, const char*, const char*, const char*);
    
    
    char* getLocalFile(const char*);    
    bool decisor();
    void sys_err(const char*);
    void usr_err(const char*);
    void usr_msg(const char*);
    
    
    
    //---
    void connectProvider(const char*);
    void requestService(const char* );
    
    //void getListOfServices();
    //char* getLocalFile(const char*, const char*);
    //char* getRemoteFile(const char*);
    
    
    void closeClient();

};










// ---















Client::Client(){
    
    usr_msg("Client is running...");
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress.sin_family = AF_INET;
        
}

Client::~Client(){
    
}

void Client::openConnection(const char* ipAddress, const char* port){
        
    bzero(&serverAddress, sizeof(struct sockaddr_in));
    serverAddress.sin_port = htons( atoi(port) );
    
    server = gethostbyname(ipAddress);
    if(server == 0)
        sys_err("Server not reachable");
    
    bcopy( (char*) server->h_addr, (char*) &serverAddress.sin_addr.s_addr, server->h_length );
    
    if( connect(socketDescriptor, CAST_ADDR(&serverAddress), sizeof(struct sockaddr_in)) < 0 )
        sys_err("Connection failed");
    
}

void Client::closeConnection(){

    close(socketDescriptor);
    
}

void Client::disconnectReg(){
    
    closeConnection();
    usr_msg("Socket closed");
    usr_msg("Connection with service register is terminated");
    
}

void Client::disconnectSrv(){
    
    closeConnection();
    usr_msg("Socket closed");
    usr_msg("Connection with service provider is terminated");
    
}


void Client::connectReg(const char* ipAddress, const char* port){
    
    std::string str;
    str.append("Searching service repository at ");
    str.append(ipAddress);
    str.append(":");
    str.append(port);
    str.append("...");
    usr_msg(str.c_str());
    
    openConnection(ipAddress, port);
    
    usr_msg("Service repository found");

}

void Client::connectSrv(const char* ipAddress, const char* port){
    
    std::string str;
    str.append("Searching service provider at ");
    str.append(ipAddress);
    str.append(":");
    str.append(port);
    str.append("...");
    usr_msg(str.c_str());
    
    openConnection(ipAddress, port);
    
    usr_msg("Service provider found");
    
}



bool Client::locateSrv(const char* regAddr, const char* regPort, const char* cmd){
    
    string msg("Searching service provider for '");
    msg.append(cmd);
    msg.append("'...");
    usr_msg(msg.c_str());
    
    connectReg(regAddr, regPort);
    
    char buffer[22]; // "xxx.xxx.xxx.xxx:yyyyy\n"
    
    if( write(socketDescriptor, cmd, strlen(cmd)) < 0 )
        sys_err("Error during socket operation (write)");
    
    if ( read(socketDescriptor, buffer, strlen(buffer)) < 0 )
        sys_err("Error during socket operation (read)");

    disconnectReg();    
    
    string strBuffer(buffer);
    
    if(strBuffer.compare("NoSrvForReq") == 0){
        msg.clear();
        msg.append("No service provider for '");
        msg.append(cmd);
        msg.append("'...");
        usr_msg(msg.c_str());

        return false;
    }
    
    else{
        int i = (int)strBuffer.find(":");
        string srvAddr(strBuffer, 0, i);
        
        i++;
        string srvPort(strBuffer, i, strBuffer.size());;
        
        insertSrv(srvAddr.c_str(), srvPort.c_str(), cmd);
        
        msg.clear();
        msg.append("Service provider address acquired (");
        msg.append(strBuffer);
        msg.append(")");
        usr_msg(msg.c_str());
        
        srv.addr = srvAddr;
        srv.port = srvPort;
        
        return true;
    }
}


bool Client::checkSrvList(const char* cmd){
    
    usr_msg("Checking in the service provider list...");
    
    if(srvList.empty()){
        usr_msg("Server list empty");
        return false;
    }
        
    list <cmd_s>::iterator iter = find(srvList.begin(), srvList.end(), find_cmd(cmd));
    
    if( iter == srvList.end()){
        string msg("The list doesn't contain any provider for '");
        msg.append(cmd);
        msg.append("'");
        usr_msg(msg.c_str());
        return false;
    }
    
    string srvAddr = iter->srv.begin()->addr;
    string srvPort = iter->srv.begin()->port;
    
    string msg("Service provider found ");
    msg.append(srvAddr);
    msg.append(":");
    msg.append(srvPort);
    usr_msg(msg.c_str());
    
    srv.addr = srvAddr;
    srv.port = srvPort;
    
    return true;
    
}


void Client::srvReq(const char* addr, const char* port, const char * cmd){
    
    connectSrv(addr, port);
    
    
    
    disconnectSrv();
    
}

void Client::srvReq(const char* addr, const char* port, const char* cmd, const char* data){

    connectSrv(addr, port);
    
    
    
    disconnectSrv();
    
}


void Client::insertSrv(const char* addr, const char* port, const char* cmd){
    
    //inserisce il nuovo server nella lista dei comandi per i quali sono stati individuati server
    
    
}





//char* Client::chooseRandom(const char* ext){}
  



char* Client::getLocalFile(const char* ext){
    
    /*
     char buffer[10];
     GetModuleFileName( NULL, buffer, 10 );
     string::size_type pos = string( buffer ).find_last_of( "\\/" );
     string( buffer ).substr( 0, pos);
     */
    
    srand((unsigned)time(NULL));
    
    std::string path;
    path.append("/Users/francescoracciatti/Desktop/Client/Client/");
    path.append("img/");
    
    std::string name;
    name.append(std::to_string(rand()%10));
    name.append(".");
    name.append(ext);
    
    std::string complete;
    complete.append(path);
    complete.append(name);
    
    std::ifstream file(complete, std::ios::binary);
    
    if(file.is_open()){
        std::string msg;
        msg.append("File ");
        msg.append(name);
        msg.append(" found in ");
        msg.append(path);
        msg.append("");
        usr_msg(msg.c_str());
    }
    else{
        std::string msg;
        msg.append("File ");
        msg.append(name);
        msg.append(" not found");
        usr_err(msg.c_str());
    }
    
    file.seekg(0,std::ios::end);
    std::ifstream::pos_type size = file.tellg();
    file.seekg(0);
    
    char* buffer = new char[size];
    
    file.read(buffer, size);
    
    file.close();
    
    return buffer;
    
}

bool Client::decisor(){
    
    srand((unsigned)time(NULL));
    
    if(rand() % 10 <= 5)
        return true;
    
    return false;
}

void Client::sys_err(const char* msg){
    
    std::cout<<msg<<std::endl;
    std::cerr<<strerror(errno)<<std::endl;
    exit(-1);
    
}

void Client::usr_err(const char* msg){
    
    std::cout<<msg<<std::endl;
    exit(-1);
    
}

void Client::usr_msg(const char* msg){
    
    std::cout<<msg<<std::endl;
    
}



















#endif
