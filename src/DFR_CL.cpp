/*
 * File:   cl_2.cpp
 * Author: Alessandro Pischedda
 *
 * Created on 22 March 2010, 13.27
 *
 */
#include "../include/DFR_CL.h"
#include "../include/utility.h"


extern Repository repository;

void *listener_cl(void *socket){

   int srv_sd =  (size_t)socket;
   int len,cl_sd,ret;
   struct sockaddr_in cl_addr;
   char *cl_address;
   pthread_t cl_s;

   while(1){
       char clnt[15];
       //Extract a new request from the queue
       len = sizeof(cl_addr);
       cl_sd=accept(srv_sd,(struct sockaddr *) &cl_addr, (socklen_t*)&len);
       cl_address=inet_ntoa(cl_addr.sin_addr);
       if(cl_sd==-1){
         cout<<"DFR> isn't possible serve Client "<<inet_ntoa(cl_addr.sin_addr)<<endl;
       }
       ret = pthread_create(&cl_s, 0, handler_cl , (void*)cl_sd);
       if(ret != 0) {
            printf("DFR> isn't possible to create a thread to handle the Client's request\n");
            pthread_exit(NULL);
        }
       cout<<"DFR> theclient "<<inet_ntoa(cl_addr.sin_addr)<<" is connected, his id is "<<cl_sd<<endl;

    }//end while

}


void *handler_cl(void *socket){
    int cl_sd =  (size_t)socket,ret;
    string msg, file_name,command,file_dowloading="";
    std::stringstream response;
    struct detail_location loc;
    bool disconnect = false;
    /*mettere un list<string> file_transfert dove aggiungere i file in trasferimento*/
    pthread_detach(pthread_self());

    while(!disconnect){
        response.str("");
        receive_msg(cl_sd,msg);
        command =  cut_str(msg,"|");
        switch (atoi(command.c_str()))  {
            case START_FILE_TRANSFERT : {
                file_name = cut_str(msg,"|");
                ret = repository.begin_file_transfer(file_name,cl_sd,loc);
                if(ret == OK){
                    cout<<"DFR> the client "<<cl_sd<<" has begun to download the file "<<file_name<<
                            " from FS : "<<loc.ip<<" "<<loc.port<<endl;
                    response<<OK<<"|"<<loc.ip<<"|"<<loc.port;
                    send_msg(cl_sd,response.str());
                    file_dowloading = file_name;
                }else{
                    response<<ret;
                    send_msg(cl_sd,response.str());
                }
                break;
            }
            case END_FILE_TRANSFERT :{
                if(file_dowloading.empty()){
                    response<<ERR_NO_TRANSFER;
                    send_msg(cl_sd, response.str());
                    break;
                }
                file_name = cut_str(msg,"|");
                ret = repository.end_file_transfer(file_name,cl_sd,loc);
                if(ret == ERR_NO_TRANSFER){
                    response<<ERR_NO_TRANSFER;
                    send_msg(cl_sd, response.str());
                    break;
                }

                response<<OK;
                send_msg(cl_sd,response.str());
                file_dowloading="";
                cout<<"DFR> the CL "<<cl_sd<<" has ended the download of "<<file_name<<endl;
                break;
            }
            case REG_NOTIFY :{
                file_name = cut_str(msg,"|");
                cout<<"DFR> Notify request from CL "<<cl_sd<<" on file "<<file_name<<endl;
                
                repository.reg_notify(file_name,cl_sd,loc);
                file_dowloading = file_name;
                
                break;
            }
            case DISCONNECT:{
               
                disconnect=true;
                response<<OK;
                send_msg(cl_sd,response.str());

                break;
            }
        }//end switch

    }//end while
    close(cl_sd);
    cout<<"DFR> Client "<<cl_sd<<" disconnected"<<endl;
    pthread_exit(NULL);

}
