
#include <iostream>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sstream>
#include "../include/DFR.h"

Repository repository;


int main(int argc, char *argv[])
{
  // Variables
  int srv_fs_sd, srv_cl_sd, son;
  int ret, port_fs,port_cl,len;
  struct sockaddr_in my_addr, clnt_addr;
  char *clnt_address;
  pthread_t fs_s,cl_s;

  if (argc != 3){
         cout<<"Usage:"<<endl; 
         cout<<"./DFR <FS_port> <CLIENT_port>"<<endl;
         return -1;
  }

  cout<<"DFR v.0.1"<<endl; ;
  
  //Retrieve from argv the ports number and convert them in interger
  port_fs=atoi(argv[1]);
  port_cl=atoi(argv[2]);

  if(port_fs < 1024 || port_fs > 65535){
      cout<<"FS port value incorrect, it must between [1024,65535]."<<endl;
      exit(1);
  }
  if(port_cl < 1024 || port_cl > 65535){
      cout<<"CL port value incorrect, it must between [1024,65535]."<<endl;
      exit(1);
  }

  srv_fs_sd = init_sd_srv("127.0.0.1", port_fs);
  srv_cl_sd = init_sd_srv("127.0.0.1", port_cl);
  
  if(srv_fs_sd == -1)
      exit(-1);
  if(srv_cl_sd == -1)
      exit(-1);

  ret = pthread_create(&fs_s, 0, listener_fs , (void*)srv_fs_sd);
  ret = pthread_create(&cl_s, 0, listener_cl , (void*)srv_cl_sd);
  pthread_join(fs_s, 0);
  pthread_join(cl_s, 0);
  close(srv_fs_sd);
  close(srv_cl_sd);
  return(0);
}
