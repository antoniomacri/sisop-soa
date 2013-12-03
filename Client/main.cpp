//
//  main.cpp
//  Client
//
//  Created by Francesco Racciatti on 01/12/13.
//  Copyright (c) 2013 Francesco Racciatti. All rights reserved.
//

/*
 
 DALLE SPECIFICHE
 
 Il client che ha bisogno di utilizzare un servizio per prima cosa si rivolge al registro dei servizi,
 chiedendo l'indirizzo e la porta del service provider a cui rivolgersi.
 
 Il client deve ciclicamente:
 1. leggere un .jpg a caso o dal disco o dal secondo provider;
 2. chiedere a caso o la rotazione o il flip al primo provider;
 3. cercare di memorizzare il .jpg modificato sul secondo provider senza modificare il nome del file
 
 
 Un servizio può esser visto come una funzione che:
 1. ha un nome (che identifica univocamente il servizio nel sistema)
 2. prende dei parametri in ingresso
 3. fornisce dei parametri in uscita
 
 Un servizio è quindi descritto da una stringa contenente il nome e da una lista ordinata di parametri,
 ognuno dei quali contiene un tipo e una direzione (IN o OUT). Il tipo del parametro può essere uno dei seguenti:
 1. integer
 2. double
 3. stringa
 4. buffer di byte
 
 
 */

#include "ClientLib.h"


void usr_err(const char* msg){
    
    cout<<msg<<std::endl;
    exit(-1);
    
}

void usr_msg(const char* msg){

    cout<<msg<<std::endl;
    
}

bool choose(){
    
    usr_msg("Continue? (y or n)");
    string in;
    cin>>in;
    
    if( in == "y")
        return true;

    return false;
    
}


int main(const int argc, const char* argv[])
{    
    if(argc < 2) {
        cout<<"Usage: client <Service Register IP Address> <Port>"<<endl;
        exit(-1);
    }
    
    string regAddr = argv[1];
    string regPort = argv[2];
    
    Client cln;
    
    bool loop;
    
    string cmd;    
    
    do{
        
        if( cln.decisor() )
            cln.getLocalImg();
        
        else{
            
            if( !cln.checkSrvList("GetList"))
                if( !cln.locateSrv(argv[1], argv[2], "GetList"))
                    usr_err("Change service register");
            
            cln.srvReq(cln.cache.addr, cln.cache.port, "GetList");
            cln.srvReq(cln.cache.addr, cln.cache.port, "GetImage", cln.cache.name);
            
        }
        
        if( cln.decisor() ){
            cmd.clear();
            cmd.append("RotateImage");
        }

        else{
            cmd.clear();
            cmd.append("HorizontalFlipImage");
        }
        
        if( !cln.checkSrvList(cmd.c_str()) )
            if( !cln.locateSrv(argv[1], argv[2], cmd) )
                usr_err("Change service register");
        
        cln.srvReq(cln.cache.addr, cln.cache.port, cmd, cln.cache.name, cln.cache.data);

        cmd.clear();
        cmd.append("StoreImage");
        
        if( !cln.checkSrvList(cmd) )
            if( !cln.locateSrv(argv[1], argv[2], cmd) )
                usr_err("Change service register");
        
        cln.srvReq(cln.cache.addr, cln.cache.port, cmd, cln.cache.name, cln.cache.data);
     

        loop = choose();
        
    }while(loop);
    
    return 0;
}

