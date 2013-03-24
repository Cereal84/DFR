/*
 * File:   FS.cpp
 * Author: Alessandro Pischedda
 *
 * Created on 19 marzo 2010, 18.45
 */


#include "../include/FS.h"


using namespace std;

int port_fs_srv;
int maxcl;
string ip_fs_srv;
std::list<string> file_to_reg;


void *bd_cl(void *socket){
    int cl_sd =  (size_t)socket,ret;
    string file_name;
    std::stringstream response;
    int fd;

    response.str("");
    receive_msg(cl_sd,file_name);

    pthread_mutex_lock(&mutex);

    // Is the file in the repository ?
    if(list_file.find(file_name) == list_file.end()){

        pthread_mutex_unlock(&mutex);
        response<<ERR_FILE_NOT_EXISTS;
        send_msg(cl_sd,response.str());
        close(cl_sd);
        pthread_exit(NULL);
    }


    // check if there are too many requests for this file
    if(list_file[file_name].current_cl >= list_file[file_name].max_cl){

        pthread_mutex_unlock(&mutex);
        response<<ERR_TOO_MANY_CLIENTS;
        cout<<"FS> ERR: Too much requests for "<<file_name<<", is impossible serve the client "<<cl_sd<<endl;
        send_msg(cl_sd,response.str());
        close(cl_sd);
        pthread_exit(NULL);
    }
    list_file[file_name].current_cl++;
    pthread_mutex_unlock(&mutex);
    fd = open(file_name.c_str(), O_RDONLY);
    if (fd == -1 ){
        cout<<"FS> ERR: iit is impossible open the file "<<file_name<<endl;
        response<<ERROR;
        send_msg(cl_sd,response.str());
        close(cl_sd);
        pthread_exit(NULL);
    }

    // part about the file
    struct stat info;
    int file_size;
    char *buffer;

    // retrieve the file's length
    stat(file_name.c_str(),&info);
    file_size=info.st_size;
    buffer = new char[file_size];
    ret = read(fd, buffer, file_size);
    if (ret == -1){
        cout<<"FS> ERR: it is impossible open the file "<<file_name<<endl;
        response<<ERROR;
        send_msg(cl_sd,response.str());
        close(cl_sd);
        pthread_exit(NULL);
    }

    response<<OK<<"|"<<file_size<<"|"<<buffer;
    send_msg(cl_sd,response.str());

    pthread_mutex_lock(&mutex);
    list_file[file_name].current_cl--;
    pthread_mutex_unlock(&mutex);
    close(cl_sd);
    pthread_exit(NULL);


}

void *listener_cl(void *socket){
   int srv_sd =  (size_t)socket;
   int len,cl_sd,ret;
   struct sockaddr_in cl_addr;
   char *cl_address;
   pthread_t cl_s;

   while(1){
       
       // Extract a new request
       len = sizeof(cl_addr);
       cl_sd=accept(srv_sd,(struct sockaddr *) &cl_addr, (socklen_t*)&len);
       cl_address=inet_ntoa(cl_addr.sin_addr);
       if(cl_sd==-1){
         cout<<"FS> It is impossible serve the client "<<inet_ntoa(cl_addr.sin_addr)<<endl;
       }
       ret = pthread_create(&cl_s, 0, bd_cl , (void*)cl_sd);
       if(ret != 0) {
            printf("FS> It is impossible create the thread to handle the client\n");
            pthread_exit(NULL);
       }
       cout<<"FS> The client "<<inet_ntoa(cl_addr.sin_addr)<<" is connected"<<endl;

    }// end while
}

void* bd_to_dfr(void* sk){
  int socket = (size_t)sk;
  string comando, response="";
  stringstream messaggio;
  bool disconnect = false;

  pthread_detach(pthread_self());


  messaggio<<CONNECT<<"|"<<ip_fs_srv<<"|"<<port_fs_srv<<"|"<<maxcl;
  send_msg(socket, messaggio.str());
  receive_msg(socket, response);

  console("register_file", socket, disconnect);
  sleep(5);
  console("disconnect", socket, disconnect);
  
  close(socket);
  cout<<"FS> Disconnetion from DFR done"<<endl;
  pthread_exit(0);
}


void console(string cmd, int sk, bool &exit){

    string comando;
    std::stringstream message;

    comando = cut_str(cmd, " ");

    if( comando == "")
	return;
    else if( comando == "register_file")
        register_file(cmd, sk);

    else if( comando == "unregister_file")
        unregister_file(cmd, sk);

    else if( comando == "disconnect"){
            if( disconnect(sk) == true)
                exit = true;
    }
    else {
        cout<<"FS> ERR: Unknown command, write help or ? to see the list of commands"<<endl;
    }
    return;
}



void register_file(string cmd, int socket){
    std::stringstream message;
    string file_name, response;
    struct file_info f_info;
    struct stat info;

    for(std::list<string>::iterator iter = file_to_reg.begin();  iter != file_to_reg.end(); iter++)
    {
         message.str("");
         file_name = *iter;
         iter++;
         string max = *iter;
         
         if(file_name == ""){
            cout<<"FS> filename missing, see the help for the command sintax"<<endl;
            return;
         }
         if (stat(file_name.c_str(), &info) == -1){
            cout<<"FS> ERR: file "<<file_name<<" doesn't exist"<<endl;
            continue;
         }
         message<<REGISTER_FILE<<"|"<<file_name;
         send_msg(socket,message.str());
         receive_msg(socket,response);
         if( atoi(response.c_str()) != OK ){
            cout<<"FS> ERR: an error occur during the registration of the file"<<endl;
            return;
         }

         f_info.current_cl = 0;
         f_info.max_cl = atoi( max.c_str() );
         list_file[file_name] = f_info;
         cout<<"FS> file "<<file_name<<" registered"<<endl;
         
   }
    return ;
}

void unregister_file(string cmd, int socket){
    std::stringstream message;
    string file_name, response;

    message.str("");
    file_name = cut_str(cmd, " ");
    if(file_name == "unregister_file"){
        cout<<"FS> filename missing, see the help for the command sintax"<<endl;
        return;
    }
    message<<UNREGISTER_FILE<<"|"<<file_name;
    send_msg(socket,message.str());
    receive_msg(socket, response);

    if( (atoi(response.c_str())) == ERR_FILE_NOT_REGISTERED )
               cout<<"FS> the file "<<file_name<<" wasn't registered on DFR"<<endl;
    else{
               list_file.erase(file_name);
               cout<<"FS> file "<<file_name<<" unregistered"<<endl;
    }
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


int main(int argc, char** argv) {

  int ret,port_dfr;
  int sk_dfr, sk_cl;
  string ip_dfr ="";
  pthread_t dfr_s,cl_s;

  if (argc < 6){
         cout<<"Error, a parameter is missing, the right command sintax is:\n";
         cout<<"./FS <ip_DFR> <port_DFR> <ip_FSR> <port_FSR> <maximum_number_of_clients> "
                 "<file1> <maxcl_file1> <file2> <maxcl_file2> ...\n";
         return 0;
  }

  port_dfr=atoi(argv[2]);
  port_fs_srv = atoi(argv[4]);

  if(port_dfr < 1024 || port_dfr > 65535){
      cout<<"FS> Invalid port number for DFR"<<endl;
      return 0;
  }
  if(port_fs_srv < 1024 || port_fs_srv > 65535){
      cout<<"FS> Invalid port number for FS"<<endl;
      return 0;
  }

  ip_fs_srv.assign(argv[3]);
  ip_dfr.assign(argv[1]);
  maxcl = atoi(argv[5]);

 sk_dfr=init_sd_cl(ip_dfr, port_dfr);

 if( sk_dfr == -1)
       exit(1);
 
 for(int i = 6; i < argc ; i+=2){
      file_to_reg.push_back(argv[i]);
      file_to_reg.push_back(argv[i+1]);
      
  }

  sk_cl = init_sd_srv("127.0.0.1", port_fs_srv);
  if(sk_cl == -1)
      exit(-1);

  ret = pthread_create(&cl_s, 0, listener_cl , (void*)sk_cl);
  ret = pthread_create(&dfr_s, 0, bd_to_dfr , (void*)sk_dfr);

  pthread_join(dfr_s, NULL);
  
  close(sk_dfr);
  close(sk_cl);
  return 0;
}
