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

void parser(const int argc, const char* argv[]){
    
    if(argc < 2) {
        std::cout<<"Usage: client <Service Register IP Address> <Port>"<<std::endl;
        exit(-1);
    }
    
}


int main(const int argc, const char* argv[])
{
    parser(argc, argv);
    
    Client cln;

    if(cln.decisor()){
        
        char* buffer = cln.getRandomLocalFile("jpg");
        
    }
    else{         //get img from remote server
        
        //contact repository from get image
        //close repository socket
        //open provider repository
        //get list
        //random choice
        //get img
        //close socket

        
    }
    
    //contact repository to find other server to modify img
    cln.makeSocketClientRegistry(argv[1], argv[2]);
    
    //...
    
    
    //contact repository to find server to store image
    //close repository socket
    //open clientServer socket
    //store img
   
    
    
    //loop it!!!
    
    
    
    return 0;
}

