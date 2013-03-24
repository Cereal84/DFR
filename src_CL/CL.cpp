/*
 * File:   CL.cpp
 * Author: Alessandro Pischedda
 *
 * Created on 19 march 2010, 18.45
 */


#include "../include/CL.h"



void end_file_transfer(string file_name, int socket){
     std::stringstream messaggio;
     string response;

     messaggio<<END_FILE_TRANSFERT<<"|"<<file_name;
     send_msg(socket,messaggio.str());
     receive_msg(socket, response);
     if( (atoi(response.c_str())) == ERR_NO_TRANSFER )
             cout<<"the file "<<file_name<<" wasn't requested before"<<endl;
}

int get_file(string file_name, string new_file, string ip_fs, int port_fs){
    int sk_fs;
    string response;


    sk_fs=init_sd_cl(ip_fs, port_fs);
    if(sk_fs == -1)
        return ERROR;

    send_msg(sk_fs,file_name);
    receive_msg(sk_fs,response);

    switch (atoi(cut_str(response,"|").c_str())) {
        case ERROR :{
            cout<<"CL> ERR: Unknown error occur on FS"<<endl;
            return ERROR;
        }
        case ERR_TOO_MANY_CLIENTS:{
            cout<<"CL> ERR: File Server too busy"<<endl;
            return ERR_TOO_MANY_CLIENTS;
        }
        case OK:{
            int fd, file_size;

            fd = open(new_file.c_str(), O_CREAT | O_WRONLY);
            if(fd == -1){
                cout<<"CL> ERR: It is impossible open the file "<<new_file<<endl;
                return 0;
            }
            file_size = atoi( cut_str(response,"|").c_str() );
            write(fd, response.c_str(), file_size );
            close(fd);
            return OK;
        }
    }

}


int start_transfer_file(string file_name,string new_file_name ,int socket,string &ip, int &port){

      string response;
      int  id_comando,ret=-1;
      std::stringstream messaggio;

      messaggio<<START_FILE_TRANSFERT<<"|"<<file_name;
      send_msg(socket,messaggio.str());
      receive_msg(socket,response);
      id_comando =atoi( cut_str(response,"|").c_str() );
      if(id_comando == ERR_TOO_MANY_CLIENTS){
             cout<<"CL> Too much clients"<<endl;
             return ERR_TOO_MANY_CLIENTS;
      }
      if( id_comando == ERR_FILE_NOT_EXISTS){
             cout<<"CL> The file "<<file_name<<" doesn't exist on DFR"<<endl;
             return ERR_FILE_NOT_EXISTS;
      }
      ip = cut_str(response,"|");
      port = atoi ( cut_str(response,"|").c_str() );
      ret = get_file(file_name, new_file_name, ip, port);
      if(ret == OK)
        end_file_transfer(file_name,socket);
      return ret;
}



int notify(string file_name, string new_file_name, int socket, string &ip, int &port){
    std::stringstream messaggio;
    string response, file_response;
    int ret = -1;



    messaggio<<REG_NOTIFY<<"|"<<file_name;
    send_msg(socket, messaggio.str());

    receive_msg(socket, response);

    file_response = cut_str(response, "|");
    file_response = cut_str(response, "|");

    /* if you haven't send a notification request for this file send back a 
       ERR_NO_NOTIFICATION message, otherwise start the download
    */

    if( file_response != file_name){

        messaggio<<ERR_NO_NOTIFICATION;
        send_msg(socket, messaggio.str());
        return ERR_NO_NOTIFICATION;

    }

    messaggio.str("");
    messaggio<<OK;
    send_msg(socket,messaggio.str());
    receive_msg(socket,response);
    ip = cut_str(response,"|");
    port = atoi (cut_str(response,"|").c_str());
    ret = get_file(file_name, new_file_name , ip, port);
    if(ret == OK)
        end_file_transfer(file_name,socket);
    return ret ;
}

bool disconnect(int socket){
    std::stringstream message;
    string response;
    message.str("");

    message<<DISCONNECT;
    send_msg(socket, message.str());
    receive_msg(socket, response);
    if ( atoi(response.c_str()) != OK)
        return false;
    return true;

}



void cl_test2(int socket, string file_request, string new_file_name){
  
  int ret = -1,port;
  string ip;


  ret = notify(file_request, new_file_name, socket, ip, port);
  while(ret != OK){
      ret = get_file(file_request, new_file_name,ip, port);
      if (ret == OK)
          end_file_transfer(file_request, socket);
      if(ret == ERROR){
          end_file_transfer(file_request, socket);
          ret=OK;
      }
  }
  disconnect(socket);
  close(socket);
  
  cout<<"CL> disconnection from DFR done"<<endl;

}

void cl_test1(int socket, string file_request, string new_file_name){
    int ret = -1, port;
    string ip;

    ret = start_transfer_file(file_request, new_file_name,socket, ip, port);
        

    ret = get_file(file_request, new_file_name,ip, port); 
    while(ret != OK){
          ret = get_file(file_request, new_file_name,ip, port);
          if (ret == OK)
                end_file_transfer(file_request, socket);
          if(ret == ERROR){
                end_file_transfer(file_request, socket);
                ret=OK;
          }
    }
    disconnect(socket);
    close(socket);

    cout<<"CL> disconnection from DFR done"<<endl;
    
}

/*
 *
 */
int main(int argc, char** argv) {
 //VARIABLES
  int port_dfr, sk_dfr, mode;

  string ip_dfr ="";
  string file_request, new_file_name;

  if (argc != 6){
         cout<<"Error, a parameter is missing, the correct command is:"<<endl;
         cout<<"./CL <ip_DFR> <port_DFR> <test_mode> <file_request> <new_file_name>"<<endl;
         exit(1);
  }


  port_dfr=atoi(argv[2]);

  if(port_dfr < 1024 || port_dfr > 65535){
      cout<<"CL> Invalid port number for DFR"<<endl;
      exit(1);
  }

  file_request = argv[4];
  new_file_name = argv[5];
  ip_dfr.assign(argv[1]);
  sk_dfr=init_sd_cl(ip_dfr, port_dfr);
  if( sk_dfr == -1)
       exit(1);

  mode = atoi(argv[3]);

  switch(mode){
      case 1:{
          cl_test1(sk_dfr,file_request, new_file_name);
          break;
      }
      case 2:{
          cl_test2(sk_dfr,file_request, new_file_name);
          break;
      }
  }
  end_file_transfer(file_request, sk_dfr);
  return (EXIT_SUCCESS);
}


