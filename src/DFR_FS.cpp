#include "../include/DFR_FS.h"
#include "../include/utility.h"


extern Repository repository;

void *listener_fs(void *socket){
   
   int srv_sd =  (size_t)socket;
   int len,fs_sd,ret;
   struct sockaddr_in fs_addr;
   char *fs_address;
   pthread_t fs_s;

   while(1){
       char clnt[15];
       // Extract from the queue a new request
       len = sizeof(fs_addr);
       fs_sd=accept(srv_sd,(struct sockaddr *) &fs_addr, (socklen_t*)&len);
       fs_address=inet_ntoa(fs_addr.sin_addr);

       if(fs_sd==-1){
         printf("DFR> isn't possible to serve the File Server %s",inet_ntoa(fs_addr.sin_addr));
       }
	
       ret = pthread_create(&fs_s, 0, handler_fs , (void*)fs_sd);
       if(ret != 0) {
            printf("DFR> isn't possible to create a thread to handle the FS's request\n");
            pthread_exit(NULL);
       }
       
    }//end while
   
}


void *handler_fs(void *socket){
    int fs_sd =  (size_t)socket;
    string msg, file_name,command;
    std::stringstream response;
    struct detail_location fs;
    bool disconnect = false;
    list<string> registered_files;

    pthread_detach(pthread_self());

    while(!disconnect){
        response.str("");
        receive_msg(fs_sd,msg);
        command =  cut_str(msg,"|");

	// handle the commands
        switch (atoi(command.c_str()))  {
            case CONNECT : {
                int maxcl;
                fs.ip = cut_str(msg,"|");
                fs.port = atoi( cut_str(msg,"|").c_str() );
                maxcl = atoi( cut_str(msg,"|").c_str() );
                repository.add_file_server(fs.ip,fs.port,maxcl);
                response<<OK;
                send_msg(fs_sd,response.str());
                cout<<"DFR> File Server "<<fs.ip<<" "<<fs.port<<" is connected, his id is "<<fs_sd<<endl;
                break;
            }
            case REGISTER_FILE :{
                file_name = cut_str(msg,"|");
                repository.add_file(file_name,fs.ip,fs.port);
                registered_files.push_back(file_name);
                cout<<"DFR> File Server "<<fs_sd<<" has registered the file "<<file_name<<endl;
                response<<OK;
                send_msg(fs_sd,response.str());
                break;
            }
            case UNREGISTER_FILE :{
                
                file_name = cut_str(msg,"|");
                //int ret;

                list<string>::iterator item = find( registered_files.begin(), registered_files.end(), file_name);
                if(item == registered_files.end()){
                    response<<ERR_FILE_NOT_REGISTERED;
                    send_msg(fs_sd, response.str());
                    break;
                }

                repository.remove_file(file_name,fs.ip,fs.port);
                                
                registered_files.remove(file_name);
                cout<<"DFR> File Server "<<fs_sd<<" has unregistered the file "<<file_name<<endl;
                response<<OK;
                send_msg(fs_sd,response.str());
                break;
            }
            case DISCONNECT :{
                //if it has registered file will going to remove them from repository
                if(!registered_files.empty()){

                   std::list<string>::iterator file;
                   for(file = registered_files.begin();file !=
                                    registered_files.end();file++)
                    {
                                       
                           repository.remove_file(*file,fs.ip,fs.port);
                    }
                    
                    registered_files.clear();
                }
                repository.remove_file_server(fs.ip,fs.port);
                disconnect=true;
                response<<OK;
                send_msg(fs_sd,response.str());
                cout<<"DFR> File Server "<<fs_sd<<" disconnected"<<endl;
                break;
            }
        }//end switch

    }//end while
    close(fs_sd);
    pthread_exit(NULL);

}





