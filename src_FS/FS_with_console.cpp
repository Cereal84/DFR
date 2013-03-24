/*
 * File:   FS.cpp
 * Author: Alessandro Pischedda
 *
 * Created on 19 marzo 2010, 18.45
 */

#include "../include/FS.h"

int port_fs_srv;
int maxcl;
string ip_fs_srv;

void show_help(){
    cout<<"\nHELP \n\n";
    cout<<"-) register_file <file_name> <maxclient> \n";
    cout<<"-) unregister_file <file_name> \n";
    cout<<"-) disconnect \n";
    return;
}




void *bd_cl(void *socket){
    int cl_sd =  (size_t)socket,ret;
    string file_name;
    std::stringstream response;
    int fd;

    response.str("");
    receive_msg(cl_sd,file_name);

    pthread_mutex_lock(&mutex);

    //Is the file in the repository ?
    if(list_file.find(file_name) == list_file.end()){

        pthread_mutex_unlock(&mutex);
        response<<ERR_FILE_NOT_EXISTS;
        cout<<"ERR: The file "<<file_name<<" requested from client "<<cl_sd<<" doesn't exist"<<endl;
        send_msg(cl_sd,response.str());
        close(cl_sd);
        pthread_exit(NULL);
    }

    // check if there are too many requests for this file and send back an error
    if(list_file[file_name].current_cl >= list_file[file_name].max_cl){

        pthread_mutex_unlock(&mutex);
        response<<ERR_TOO_MANY_CLIENTS;
        cout<<"ERR: Too much request on file "<<file_name<<" isn't impossible serve the client "<<cl_sd<<endl;
        send_msg(cl_sd,response.str());
        close(cl_sd);
        pthread_exit(NULL);
    }

    list_file[file_name].current_cl++;
    pthread_mutex_unlock(&mutex);
    fd = open(file_name.c_str(), O_RDONLY);

    if (fd == -1 ){
        cout<<"ERR: impossible open the file "<<file_name<<" requested from client "<<cl_sd<<endl;
        response<<ERROR;
        send_msg(cl_sd,response.str());
        close(cl_sd);
        pthread_exit(NULL);
    }

    // about the file
    struct stat info;
    int file_size;
    char *buffer;

    stat(file_name.c_str(),&info);
    file_size=info.st_size;
    buffer = new char[file_size];
    ret = read(fd, buffer, file_size);

    if (ret == -1){
        cout<<"ERR: impossible open the file "<<file_name<<" requested from client "<<cl_sd<<endl;
        response<<ERROR;
        send_msg(cl_sd,response.str());
        close(cl_sd);
        pthread_exit(NULL);
    }

    cout<<"CL-> transmission of file "<<file_name<<" to client "<<cl_sd<<" completed"<<endl;
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

   cout<<"FS Server"<<endl;
   cout<<"Waiting client connection...."<<endl;

   while(1){
       char clnt[15];
       
       // get a new request 
       len = sizeof(cl_addr);
       cl_sd=accept(srv_sd,(struct sockaddr *) &cl_addr, (socklen_t*)&len);
       cl_address=inet_ntoa(cl_addr.sin_addr);
       if(cl_sd==-1){
         cout<<"It is impossible serve the client "<<inet_ntoa(cl_addr.sin_addr)<<endl;
       }
       ret = pthread_create(&cl_s, 0, bd_cl , (void*)cl_sd);
       if(ret != 0) {
            printf("It is impossible create a thread to handle the client's request\n");
            pthread_exit(NULL);
        }
       cout<<"The client "<<inet_ntoa(cl_addr.sin_addr)<<" is connected and has the following ID "<<cl_sd<<endl;

    }
}

void* bd_to_dfr(void* sk){
  int socket = (size_t)sk, ret;
  string comando, response="";
  stringstream messaggio;
  bool disconnect = false;

  pthread_detach(pthread_self());

  cout<<"FS client v.0.1\n\n";
  cout<<"'?' o 'help' for the commands list\n\n";

  messaggio<<CONNECT<<"|"<<ip_fs_srv<<"|"<<port_fs_srv<<"|"<<maxcl;
  send_msg(socket, messaggio.str());
  receive_msg(socket, response);

  while(!disconnect){
      comando="";
      cout<<"fs> ";
      getline(cin,comando);
      console(comando, socket, disconnect);
  }

  close(socket);
  cout<<"Disconnection from DFR done"<<endl;
  pthread_exit(NULL);
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
    else if( comando == "help" | comando == "?")
            show_help();
    else {
        cout<<"ERR: Unknown command, write help o ? for the commands list"<<endl;
    }
    return;
}



void register_file(string cmd, int socket){
    std::stringstream message;
    string file_name, response;
    struct file_info f_info;
    struct stat info;

    message.str("");
    file_name = cut_str(cmd, " ");
    if(file_name == "register_file"){
        cout<<"file name missing, read the help for the commands sintax"<<endl;
        return;
    }
    if (stat(file_name.c_str(), &info) == -1){
        cout<<"ERR: the file "<<file_name<<" doesn't exist"<<endl;
        return;
    }
    message<<REGISTER_FILE<<"|"<<file_name;
    send_msg(socket,message.str());
    receive_msg(socket,response);
    if( atoi(response.c_str()) != OK ){
        cout<<"/n ERR: an error occur on the file registration"<<endl;
        return;
    }
    

    f_info.current_cl = 0;
    f_info.max_cl = atoi( cut_str(cmd, " ").c_str());
    list_file[file_name] = f_info;
    cout<<"file "<<file_name<<" registered"<<endl;
    return;
   
}

void unregister_file(string cmd, int socket){
    std::stringstream message;
    string file_name, response;
    struct file_info f_info;

    message.str("");
    file_name = cut_str(cmd, " ");
    if(file_name == "unregister_file"){
        cout<<"file name missing, read the help for the commands sintax"<<endl;
        return;
    }
    message<<UNREGISTER_FILE<<"|"<<file_name;
    send_msg(socket,message.str());
    receive_msg(socket, response);
    
    if( (atoi(response.c_str())) == ERR_FILE_NOT_REGISTERED )
               cout<<"file "<<file_name<<" isn't registerd on DFR"<<endl;
    else{
               list_file.erase(file_name);
               cout<<"file "<<file_name<<" unregistered"<<endl;
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
	//VARIABILI
	int ret,port_dfr;
	int sk_dfr, sk_cl;
	struct sockaddr_in srv_addr;
	string ip_dfr ="";
	pthread_t dfr_s,cl_s;

	if (argc != 6){
		cout<<"Error, one of the parameters is missing, the right command is:\n";
		cout<<"./FS <ip_DFR> <port_DFR> <ip_FSR> <port_FSR> <maximum_number_of_client>\n";
		exit(1);
	}


	port_dfr=atoi(argv[2]);
	port_fs_srv = atoi(argv[4]);
	if(port_dfr < 1024 || port_dfr > 65535){
		cout<<"Invalid port number for DFR"<<endl;
		exit(1);
	}

	if(port_fs_srv < 1024 || port_fs_srv > 65535){
		cout<<"Invalid port number for FS"<<endl;
		exit(1);
	}

	ip_fs_srv.assign(argv[3]);
	ip_dfr.assign(argv[1]);
	maxcl = atoi(argv[5]);

	sk_dfr=init_sd_cl(ip_dfr, port_dfr);

	if( sk_dfr == -1)
		exit(1);

	cout<<"Listening for client on port "<<port_fs_srv<<endl;
	sk_cl = init_sd_srv("127.0.0.1", port_fs_srv);
	if(sk_cl == -1)
		exit(-1);

	ret = pthread_create(&cl_s, 0, listener_cl , (void*)sk_cl);
	ret = pthread_create(&dfr_s, 0, bd_to_dfr , (void*)sk_dfr);
  

	pthread_join(dfr_s, 0);
	close(sk_dfr);
	close(sk_cl);
	return (EXIT_SUCCESS);
}

