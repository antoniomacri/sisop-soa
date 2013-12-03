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

typedef struct server_s{
    string addr;
    string port;
} server_t;

typedef struct service_s{
    string cmd;
    list <server_t> srv;
} service_t;


/*
//unary_function deprecated in c++11
 
struct find_cmd : unary_function <service_t, bool>{
    string cmd;
    find_cmd(string cmd):cmd(cmd){}
    bool operator() (service_t const& s) const{
        return s.cmd == cmd;
    }    
};
*/


class Client {
    
private:
    
    struct sockaddr_in serverAddress;
    struct hostent *server;
    int socketDescriptor;
    
    list <service_t> srvList;
    
public:
    
    struct cache_s{
        string name;
        string data;
        string addr;
        string port;
    } cache;
    
private:
    
    void openConnection(const string, const string);
    void closeConnection();
    
    void connectReg(const string, const string);
    void disconnectReg();
    
    void connectSrv(const string, const string);
    void disconnectSrv();

    void insertSrv(const string, const string, const string);
    
public:
    
    Client();
    ~Client();

    bool locateSrv(const string, const string, const string);
    bool checkSrvList(const string);
    
    void srvReq(const string, const string, const string);
    void srvReq(const string, const string, const string, const string);
    void srvReq(const string, const string, const string, const string, const string);
    
    string getLocalImg();
    
    bool decisor();
    void sys_err(const string);
    void usr_err(const string);
    void usr_msg(const string);

};


void Client::openConnection(const string addr, const string port){
    
    bzero(&serverAddress, sizeof(struct sockaddr_in));
    serverAddress.sin_port = htons( atoi(port.c_str()) );
    
    server = gethostbyname(addr.c_str());
    if(server == 0)
        sys_err("Server not reachable");
    
    bcopy( (char*) server->h_addr, (char*) &serverAddress.sin_addr.s_addr, server->h_length );
    
    if( connect(socketDescriptor, CAST_ADDR(&serverAddress), sizeof(struct sockaddr_in)) < 0 )
        sys_err("Connection failed");
    
}


void Client::closeConnection(){
    
    close(socketDescriptor);
    
}


void Client::connectReg(const string regAddr, const string regPort){
    
    string str("Searching service repository at ");
    str.append(regAddr);
    str.append(":");
    str.append(regPort);
    str.append("...");
    usr_msg(str);
    
    openConnection(regAddr, regPort);
    
    usr_msg("Service repository found");
    
}


void Client::disconnectReg(){
    
    closeConnection();
    usr_msg("Socket closed");
    usr_msg("Connection with service register is terminated");
    
}


void Client::connectSrv(const string srvAddr, const string srvPort){
    
    string str("Searching service provider at ");
    str.append(srvAddr);
    str.append(":");
    str.append(srvPort);
    str.append("...");
    usr_msg(str);
    
    openConnection(srvAddr, srvPort);
    
    usr_msg("Service provider found");
    
}


void Client::disconnectSrv(){
    
    closeConnection();
    usr_msg("Socket closed");
    usr_msg("Connection with service provider is terminated");
    
}


void Client::insertSrv(const string srvAddr, const string srvPort, const string cmd){
    
}


Client::Client(){
    
    usr_msg("Client is running...");
    socketDescriptor = socket(AF_INET, SOCK_STREAM, 0);
    serverAddress.sin_family = AF_INET;
    
}


Client::~Client(){
    
}


bool Client::locateSrv(const string regAddr, const string regPort, const string cmd){
    
    string msg("Searching service provider for '");
    msg.append(cmd);
    msg.append("'...");
    usr_msg(msg);
    
    connectReg(regAddr, regPort);
    
    char buffer[22]; // "xxx.xxx.xxx.xxx:yyyyy\n"
    
    if( write(socketDescriptor, cmd.c_str(), (int)cmd.size()) < 0 )
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
        usr_msg(msg);
        
        return false;
    }
    
    else{
        int i = (int)strBuffer.find(":");
        string srvAddr(strBuffer, 0, i);

        i++;
        string srvPort(strBuffer, i, strBuffer.size());;
        
        insertSrv(srvAddr, srvPort, cmd);
        
        msg.clear();
        msg.append("Service provider address acquired (");
        msg.append(strBuffer);
        msg.append(")");
        usr_msg(msg);
        
        cache.addr = srvAddr;
        cache.port = srvPort;
        
        return true;
    }
    
}


bool Client::checkSrvList(const string cmd){
    
    usr_msg("Checking local service provider list...");
    
    if(srvList.empty()){
        usr_msg("Local service provider list empty");
        
        return false;
    }
    
    //list <service_t>::iterator iter = find(srvList.begin(), srvList.end(), find_cmd(cmd));
    
    list <service_t>::iterator iter = srvList.begin();
    
    do{
        
        if(iter->cmd == cmd)
            break;
        else
            advance(iter,1);
        
    }while(iter!=srvList.end());
    
    
    if( iter == srvList.end()){
        string msg("The list doesn't contain any provider for '");
        msg.append(cmd);
        msg.append("'");
        usr_msg(msg);
        return false;
    }
    
    else{
        
        string srvAddr = iter->srv.begin()->addr;
        string srvPort = iter->srv.begin()->port;
    
        string msg("Service provider found ");
        msg.append(srvAddr);
        msg.append(":");
        msg.append(srvPort);
        usr_msg(msg);
    
        cache.addr = srvAddr;
        cache.port = srvPort;
    
        return true;
    }
    
}





void Client::srvReq(const string srvAddr, const string srvPort, const string cmd){
    
    connectSrv(srvAddr, srvPort);
    
    disconnectSrv();
    
}


void Client::srvReq(const string srvAddr, const string srvPort, const string cmd, const string name){
    
    connectSrv(srvAddr, srvPort);
    
    disconnectSrv();
    
}


void Client::srvReq(const string srvAddr, const string srvPort, const string cmd, const string data, const string name){
    
    connectSrv(srvAddr, srvPort);
    
    disconnectSrv();
    
}


string Client::getLocalImg(){
    
    /*
     char buffer[10];
     GetModuleFileName( NULL, buffer, 10 );
     string::size_type pos = string( buffer ).find_last_of( "\\/" );
     string( buffer ).substr( 0, pos);
     */
    
    srand((unsigned)time(NULL));
    
    string path("/Users/francescoracciatti/Desktop/Client/Client/img/");
    
    string name(to_string(rand()%10));
    name.append(".jpg");
    
    string complete(path);
    complete.append(name);
    
    ifstream file(complete, ios::binary);
    
    if(file.is_open()){
        string msg("File ");
        msg.append(name);
        msg.append(" found in ");
        msg.append(path);
        msg.append("");
        usr_msg(msg);
    }
    else{
        string msg("File ");
        msg.append(name);
        msg.append(" not found");
        usr_err(msg);
    }
    
    file.seekg(0,ios::end);
    ifstream::pos_type size = file.tellg();
    file.seekg(0);
        
    char* buffer = new char[size];
    file.read(buffer, size);
    file.close();
    
    string strBuffer(buffer);
    return strBuffer;
    
}


bool Client::decisor(){
    
    srand((unsigned)time(NULL));
    
    if(rand() % 10 <= 5)
        return true;
    
    return false;
}

void Client::sys_err(const string msg){
    
    cout<<msg<<endl;
    cerr<<strerror(errno)<<endl;
    exit(-1);
    
}

void Client::usr_err(const string msg){
    
    cout<<msg<<endl;
    exit(-1);
    
}

void Client::usr_msg(const string msg){
    
    cout<<msg<<endl;
    
}



















#endif
