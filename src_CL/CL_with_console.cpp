/*
 * File:   CL.cpp
 * Author: Alessandro Pischedda
 *
 * Created on 19 marzo 2010, 18.45
 */

#include <stdlib.h>
#include "../include/DFR_CL.h"
#include "../include/comunication.h"
#include "../include/utility.h"
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

string file_request;

void end_file_transfer(string file_name, int socket){
     std::stringstream messaggio;
     string response;

     messaggio<<END_FILE_TRANSFERT<<"|"<<file_name;
     send_msg(socket,messaggio.str());
     receive_msg(socket, response);
     if( (atoi(response.c_str())) == ERR_NO_TRANSFER )
             cout<<"file "<<file_name<<" wasn't requested"<<endl;
}

int get_file(string file_name, string new_file, string ip_fs, int port_fs){
    int sk_fs;
    string response;

    cout<<"Connecting to IP "<<ip_fs<<" PORT "<<port_fs<<endl;

    sk_fs=init_sd_cl(ip_fs, port_fs);
    if(sk_fs == -1)
        return 0;

    send_msg(sk_fs,file_name);
    receive_msg(sk_fs,response);

    switch (atoi(cut_str(response,"|").c_str())) {
        case ERROR :{
            cout<<"ERR: Unknown error occur on FS"<<endl;
            return ERROR;
        }
        case ERR_TOO_MANY_CLIENTS:{
            cout<<"ERR: FS too busy"<<endl;
            return ERR_TOO_MANY_CLIENTS;
        }
        case OK:{
            int fd, file_size;

            fd = open(new_file.c_str(), O_CREAT | O_WRONLY);
            if(fd == -1){
                cout<<"ERR: Isn't possible open the file "<<new_file<<endl;
                return -30;
            }
            file_size = atoi( cut_str(response,"|").c_str() );
            write(fd, response.c_str(), file_size );
            close(fd);
            return OK;
        }
    }

}


void show_help(){
    cout<<"\nHELP \n\n";
    cout<<"-) start_file_transfer <file_name> <new_file_name>\n";
    cout<<"-) end_file_transfer <file_name> \n";
    cout<<"-) notify <file_name> <new_file_name>\n";
    cout<<"-) disconnect \n";
    return;
}

void transfer_file(string comando, int socket){

      string ip, response,file_name, new_file_name;
      int port, id_comando,ret;
      std::stringstream messaggio;

      file_name = cut_str(comando, " ");
      new_file_name = cut_str(comando, " ");
      messaggio<<START_FILE_TRANSFERT<<"|"<<file_name;
      send_msg(socket,messaggio.str());
      receive_msg(socket,response);
      id_comando =atoi( cut_str(response,"|").c_str() );
      if(id_comando == ERR_TOO_MANY_CLIENTS){
             cout<<"Too client"<<endl;
             return;
      }
      if( id_comando == ERR_FILE_NOT_EXISTS){
             cout<<"The file "<<file_name<<" doesn't exist on DFR"<<endl;
             return;
      }
      ip = cut_str(response,"|");
      port = atoi ( cut_str(response,"|").c_str() );
      ret = get_file(file_name, new_file_name, ip, port);
      end_file_transfer(file_name,socket);
}



void notify(string comando, int socket){
    std::stringstream messaggio;
    string ip,response,file_name,new_file_name;
    int port;

    file_name = cut_str(comando, " ");
    new_file_name = cut_str(comando, " ");
    messaggio<<REG_NOTIFY<<"|"<<file_name;
    send_msg(socket, messaggio.str());
    file_request = file_name;
    receive_msg(socket, response);

    file_name = cut_str(comando, "|");
    file_name = cut_str(comando, "|");

    /* if you haven't send a notification request for this file send back a 
       ERR_NO_NOTIFICATION message, otherwise start the download
    */
    if( file_name != file_request){

        messaggio<<ERR_NO_NOTIFICATION;
        send_msg(socket, messaggio.str());
        return;
        
    }
    messaggio.str("");
    messaggio<<OK;
    send_msg(socket,messaggio.str());
    receive_msg(socket,response);
    ip = cut_str(response,"|");
    port = atoi (cut_str(response,"|").c_str());
    get_file(file_name, new_file_name , ip, port);
    end_file_transfer(file_name,socket);
    return;
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

void console_cl(string cmd, int sk, bool &exit){

    string comando;
    std::stringstream message;

    comando = cut_str(cmd, " ");

    if( comando == "")
	return;
    else if( comando == "start_file_transfer")
        transfer_file(cmd, sk);

    else if( comando == "end_file_transfer")
        end_file_transfer(cmd, sk);

    else if( comando == "notify")
        notify(cmd, sk);

    else if( comando == "disconnect"){
            if( disconnect(sk) == true)
                exit = true;
    }
    else if( comando == "help" | comando == "?")
            show_help();
    else {
        cout<<"ERR: Command unknown, write help or ? for the commands list"<<endl;
    }
    return;
}

/*void bd_to_dfr(int socket){
 
  string comando="", file_request;
  bool disconnect = false;

  while(!disconnect){
      comando="";
      cout<<"client> ";
      getline(cin,comando);
      console(comando, socket, disconnect);

            case START_FILE_TRANSFERT :{
                string file_name,ip;
                int port;
                file_name = cut_str(comando, "|");
                messaggio<<START_FILE_TRANSFERT<<"|"<<file_name;
                send_msg(socket,messaggio.str());
                receive_msg(socket,response);
                id_comando =cut_str(response,"|");
                if( atoi(id_comando.c_str()) == ERR_TOO_MANY_CLIENTS){
                    cout<<"Troppi client"<<endl;
                    break;
                }
                if( atoi(id_comando.c_str()) == ERR_FILE_NOT_EXISTS){
                    cout<<"Il file "<<file_name<<" non e' presente nel DFR"<<endl;
                    break;
                }
                ip = cut_str(response,"|");
                port = atoi ( cut_str(response,"|").c_str() );
                transfer_file(file_name, "prova", ip, port);
                messaggio.str("");
                messaggio<<END_FILE_TRANSFERT<<"|"<<file_name;
                send_msg(socket,messaggio.str());
                receive_msg(socket, response);
                if( (atoi(response.c_str())) == ERR_NO_TRANSFER )
                    cout<<"file "<<file_name<<" non era stato richiesto"<<endl;
                break;
            }
            case END_FILE_TRANSFERT :{
                string file_name;
                file_name = cut_str(comando, "|");
                messaggio<<END_FILE_TRANSFERT<<"|"<<file_name;
                send_msg(socket,messaggio.str());
                receive_msg(socket, response);
                if( (atoi(response.c_str())) == ERR_NO_TRANSFER )
                    cout<<"file "<<file_name<<" non era stato richiesto"<<endl;
                break;
            }
            case REG_NOTIFY :{
                 string file_name;
                 file_name = cut_str(comando, "|");
                 messaggio<<REG_NOTIFY<<"|"<<file_name;
                 send_msg(socket, messaggio.str());
                 file_request = file_name;
                 receive_msg(socket, response);
                 //controlla se avevi fatto richiesta di notifica del suddetto file
                 //se si ti arrivano i dati e puoi inziiare a scaricare
                 //se no manda ERR_NO_NOTIFICATION
                string ip,port;
                file_name = cut_str(comando, "|");
                file_name = cut_str(comando, "|");
                if( file_name != file_request){

                    messaggio<<ERR_NO_NOTIFICATION;
                    send_msg(socket, messaggio.str());
                    break;
                }
                messaggio.str("");
                messaggio<<OK;
                send_msg(socket,messaggio.str());
                receive_msg(socket,response);
                ip = cut_str(response,"|");
                port = cut_str(response,"|");
                cout<<"il file si trova in ip: "<<ip<<" porta: "<<port<<endl;
                break;
            }
            case DISCONNECT :{
                 messaggio<<DISCONNECT;
                 send_msg(socket, messaggio.str());
                 receive_msg(socket, response);
                 disconnect=true;
                break;
            }
        }//fine switch
  }//fine while
  close(socket);
  cout<<"disconnessione dal DFR avvenuta"<<endl;
}*/


void bd_to_dfr(int socket){

  string comando="";
  bool disconnect = false;

  while(!disconnect){
      comando="";
      cout<<"client> ";
      getline(cin,comando);
      console_cl(comando, socket, disconnect);
  }//end while

  close(socket);
  cout<<"disconnected from DFR"<<endl;
}


int main(int argc, char** argv) {

  int ret,port_dfr;
  int sk_dfr;
  struct sockaddr_in srv_addr;
  string ip_dfr ="";

  if (argc != 3){
         cout<<"Error, a parameter miss, use :"<<endl;
         cout<<"./CL <ip_DFR> <porta_DFR> "<<endl;
         exit(1);
  }

  port_dfr=atoi(argv[2]);

  if(port_dfr < 1024 || port_dfr > 65535){
      cout<<"Invalid DFR port number [1024-65535]"<<endl;
      exit(1);
  }
  
  ip_dfr.assign(argv[1]);
  sk_dfr=init_sd_cl(ip_dfr, port_dfr);
  if( sk_dfr == -1)
       exit(1);

  cout<<"Connected to DFR "<<argv[1]<<" on port "<<port_dfr<<"\n";
  bd_to_dfr(sk_dfr) ;
  return (EXIT_SUCCESS);
}
