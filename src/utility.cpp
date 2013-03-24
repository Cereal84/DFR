#include "../include/utility.h"
#include <string.h>
#include <iostream>

using namespace std;

int init_sd(int port, string ip, sockaddr_in &addr){
    int sd;
    
    memset(&addr, 0, sizeof(sockaddr_in));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(port);
    if(inet_pton(AF_INET, ip.c_str(), &addr.sin_addr.s_addr) <= 0) {
        printf("IP address invalid\n");
        return -1;
    }
    sd = socket(AF_INET, SOCK_STREAM, 0);
    if(sd < 0) {
        printf("Socket creation failed\n");
        return -1;
    }
    return sd;
}


int init_sd_srv(string ip , int port){
    sockaddr_in my_addr;
    int sd;
    
    sd = init_sd(port, ip , my_addr);
    if(sd == -1)
          return sd;    

    if(bind(sd, (sockaddr*) &my_addr, sizeof(my_addr)) < 0) {
        close(sd);
        printf("impossible perform the bind\n");
        return -1;
    }
    if(listen(sd, BACKLOG_LENGTH) < 0) {
        close(sd);
        printf("listen failed\n");
        return -1;
    }

    return sd;

}


int init_sd_cl(string ip , int port){
    sockaddr_in srv_addr;
    int sd;

    sd = init_sd(port, ip, srv_addr);
    if(sd == -1)
          return sd;     

    if(connect(sd, (sockaddr*) &srv_addr, sizeof(srv_addr)) < 0) {
        printf( "Connection failed\n");
        close(sd);
        return -1;
    }
    return sd;

}

string cut_str(string &msg, string separator){
    int pos;
    string part;
    pos = msg.find(separator,0);
    part = msg.substr(0, pos);
    msg.erase(0,pos+1);
    return part;

}


