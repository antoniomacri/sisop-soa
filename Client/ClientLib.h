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
#include <cstring>
#include <ctime>
#include <cstdlib>
#include <vector>
#include <fstream>

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

class Client {
    
private:
    
    struct sockaddr_in serverAddress;
    struct hostent *server;
    int socketDescriptor;
    
    struct provider{
        std::string ipAddress;
        std::string port;
    };
    
    provider* providersList;
    
private:
    
    void makeSocket(const char*, const char*);
    void closeSocket();
    
public:
    Client();
    ~Client();
    
    void makeSocketClientRegistry(const char*, const char*);
    void makeSocketClientProvider(const char*, const char*);
    
    bool decisor();
    char* getRandomLocalFile(const char*);
    
    void sys_err(const char*);
    void usr_err(const char*);
    void usr_msg(const char*);
    
    
    
    
    //---
        
    char* getRandomLocalFile(const char*, const char*);
    void connectProvider(const char*);
    void requestService(const char* );
    
    //void getListOfServices();
    //char* getLocalFile(const char*, const char*);
    //char* getRemoteFile(const char*);
    
    
    void closeClient();

};

Client::Client(){
    
    usr_msg("Client is running...");
        
}

Client::~Client(){
    
}

void Client::makeSocket(const char* ipAddress, const char* port){
    
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    
    bzero(&serverAddress, sizeof(struct sockaddr_in));
    serverAddress.sin_family = AF_INET;
    serverAddress.sin_port = htons( atoi(port) );
    
    server = gethostbyname(ipAddress);
    if(server == 0)
        sys_err("Server not found");
    
    bcopy( (char*) server->h_addr, (char*) &serverAddress.sin_addr.s_addr, server->h_length );
    
    if( connect(socketDescriptor, CAST_ADDR(&serverAddress), sizeof(struct sockaddr_in)) < 0 )
        sys_err("Connection failed");
    
}

void Client::closeSocket(){

    close(socketDescriptor);
    usr_msg("Socket closed");
    
}

void Client::makeSocketClientRegistry(const char* ipAddress, const char* port){
    
    std::string str;
    str.append("Searching service repository at ");
    str.append(ipAddress);
    str.append(":");
    str.append(port);
    str.append("...");
    usr_msg(str.c_str());
    
    makeSocket(ipAddress, port);
    
    usr_msg("Service repository found");

}

void Client::makeSocketClientProvider(const char* ipAddress, const char* port){
    
    std::string str;
    str.append("Searching service provider at ");
    str.append(ipAddress);
    str.append(":");
    str.append(port);
    str.append("...");
    usr_msg(str.c_str());
    
    makeSocket(ipAddress, port);
    
    usr_msg("Service provider found");
    
}

char* Client::getRandomLocalFile(const char* ext){
    
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






/*
void Client::connectProvider(const char* cmd){
    
    char buffer[22];    // "xxx.xxx.xxx.xxx:yyyyy\n"
    std::string strBuffer;
        
    std::string msg;
    msg.append("Searching service provider for service request '");
    msg.append(cmd);

    msg.append("'...");
    usr_msg(msg.c_str());
    
    write(socketRegistry, cmd, strlen(cmd));
    read(socketRegistry, buffer, strlen(buffer));
    
    strBuffer.copy(buffer,strlen(buffer),0);
    std::size_t found = strBuffer.find(":");
    int index = (int) found;
    
    std::string ipProvider;
    ipProvider.append(strBuffer,0,index);
    ipProvider.append("");

    std::string portProvider;
    index++;
    portProvider.append(strBuffer, index, strBuffer.size());
    
    socketClientServer(ipProvider.c_str(), portProvider.c_str());
    
}


*/


















#endif
