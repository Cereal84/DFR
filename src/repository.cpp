#include <sstream>

#include "../include/repository.h"


Repository::Repository(){
     
      pthread_mutex_init(&mutex,0);
      file_list.clear();
      no_file.clear();
      
}

void Repository::add_file(string file_name, string ip, int port) {

        struct location loc;
        pthread_mutex_lock(&mutex);
        //if this file hasn't register yet add it to the list
        if(file_list.find(file_name) == file_list.end())
                   file_list[file_name];
        
        //initialize the location structure
        loc.fs = fs_list.search_fs(ip,port);
        
        pthread_cond_init(&loc.wait_notify , 0);
        pthread_cond_init(&loc.wait_unregister , 0);
        loc.num_wait = 0;
        loc.invisible = false;
        if( !no_file.empty() & no_file.find(file_name) != no_file.end() ){
           for(int i=0; i < loc.fs->num_maxcl ; i++){
                pthread_cond_signal(&no_file[file_name].waiting_file);
           }
        }

        //add
        (file_list.find(file_name)->second).push_back(loc);
      
        pthread_mutex_unlock(&mutex);
       

}

void Repository::remove_file(string file_name, string ip, int port){
 
        pthread_mutex_lock(&mutex);

        Fs* fs = fs_list.search_fs( ip, port);

        //removes the info for the FS from the list
        std::list<location>::iterator list_iter = (file_list.find(file_name)->second).begin(); 

        while( list_iter != (file_list.find(file_name)->second).end() ){
            if (list_iter->fs == fs){
                   break;
            }
 
            list_iter++;
        }

	/* if some client is waiting for a notify or is still downloading the resource 
	   put the item as invisible, in this way no one can make a request for it.	
	*/
        if ( (list_iter->num_wait != 0) || (list_iter->fs->num_current_cl != 0))
        {
            //set as invisible
            list_iter->invisible = true;
            //wait con waiting_unregister semaphore and set the mutex
            pthread_cond_wait(&list_iter->wait_unregister,&mutex);
            
        }
        
        (file_list.find(file_name)->second).erase(list_iter);
          
        //If the file isn't mainteined from any FS removes the entry from file_list
        if( (file_list.find(file_name)->second).empty() == true ){
            file_list.erase(file_name);
        }
        pthread_mutex_unlock(&mutex);

}

int Repository::where_is(string file_name, std::list<location>::iterator &item ){

        //the file isn't on the list of registered files
        if(file_list.find(file_name) == file_list.end()){
                   //pthread_mutex_unlock(&mutex);
                   return ERR_FILE_NOT_EXISTS;
        }

        std::list<location>::iterator list_iter = (file_list.find(file_name)->second).begin(); 
        bool found = false;
        while( list_iter != (file_list.find(file_name)->second).end() ){
            if( (list_iter->fs->busy() == false) && (list_iter->invisible == false) ){
            
                   found = true;
                   break;
            }
 
            list_iter++;
        }
        
        if(!found){
            if(list_iter->invisible == true)
                  return ERR_FILE_NOT_EXISTS;
            return ERR_TOO_MANY_CLIENTS;
        }               
        
        item = list_iter;
        return OK;

}


void Repository::add_file_server(string ip, int port, int maxcl){
       pthread_mutex_lock(&mutex);
       fs_list.add_fs(ip, port, maxcl);
       pthread_mutex_unlock(&mutex);

}

int Repository::remove_file_server(string ip, int port){

       pthread_mutex_lock(&mutex);
       if(fs_list.remove_fs( ip , port) == -1)
                      return -1;
       pthread_mutex_unlock(&mutex);
       return 0;

}

int Repository::begin_file_transfer(string file_name, int sk_client, detail_location& fs){

	std::list<location>::iterator item;
	int ret;
    
	pthread_mutex_lock(&mutex);
	ret = where_is(file_name, item);

	// Too many clients or file not found
	if(ret != OK){ 
		pthread_mutex_unlock(&mutex);
		return (ret);
	}
    	//found a FS which contains the file
	item->cl_sk.push_back(sk_client);
	item->fs->num_current_cl++;
	fs.ip = item->fs->ip;
	fs.port = item->fs->port;
	pthread_mutex_unlock(&mutex);
	return OK;

}

int Repository::end_file_transfer(string file_name, int sk_client, detail_location fs){


	pthread_mutex_lock(&mutex);

	if( (file_list.find(file_name)->second).empty() )
	{
		pthread_mutex_unlock(&mutex);
		return ERR_NO_TRANSFER;
	}

	std::list<location>::iterator item = (file_list.find(file_name)->second).begin();


	while ( item!= (file_list.find(file_name)->second).end() ) {
		if ( (item->fs->ip == fs.ip) && (item->fs->port == fs.port) )
			break;
		item++;
	}

	// check if the client has made a download request
	list<int>::iterator pos = find( item->cl_sk.begin(), item->cl_sk.end(), sk_client);
	if( pos == item->cl_sk.end())
	{
		pthread_mutex_unlock(&mutex);
		return ERR_NO_TRANSFER;
	}


	/* if exist a entry in "no_file" list then check if there are clients 
	   which are waiting for a notify, is there isn't no one delete the entry
	*/
	if( no_file.find(file_name) != no_file.end()  ){

		if(no_file[file_name].num_wait != 0 ){
			pthread_cond_signal(&no_file[file_name].waiting_file);
		}else{
			no_file.erase(file_name);
		}

	}else if(item->num_wait > 0 ){ // check if there are already requests for that file in the FS
		pthread_cond_signal(&item->wait_notify);
	}

	item->fs->num_current_cl--;
	item->cl_sk.remove(sk_client);

	/* if a entry is insivible means that the FS want to delete the file. 
	   Check if all the clients who are waiting have ended the download.
	*/
	if( (item->num_wait == 0) && (item->invisible == true) && (item->fs->num_current_cl == 0 ))
	        pthread_cond_signal(&item->wait_unregister);

	pthread_mutex_unlock(&mutex);
	return OK;

}


int Repository::reg_notify(string file_name, int sk_client, detail_location& fs){

	std::list<location>::iterator item;
	int ret;
	stringstream messaggio;
	string response;

	messaggio.str("");


	pthread_mutex_lock(&mutex);

	// check if that file exists 
	ret = where_is(file_name, item);

	if( ret == ERR_FILE_NOT_EXISTS & no_file.find(file_name) == no_file.end()){
		struct notify_no_file entry;
		entry.num_wait = 0;
		pthread_cond_init(&entry.waiting_file , 0);
		no_file[file_name] = entry;
	}

	if( no_file.find(file_name) != no_file.end() ){

		no_file[file_name].num_wait++;
		pthread_cond_wait(&no_file[file_name].waiting_file,&mutex);
		no_file[file_name].num_wait--;
		ret = where_is(file_name, item);
	}

	// If no one is available wait on the semaphore  "wait_notify" of the first FS of the list
	if(ret != OK){
	
		first_fs(file_name,item);
		item->num_wait++;
		pthread_cond_wait(&item->wait_notify,&mutex);
		item->num_wait-- ;
      
	}
      
	// send a message with "NOTIFY <file_name>"
	messaggio<<NOTIFY<<"|"<<file_name;
	send_msg(sk_client, messaggio.str());
	receive_msg(sk_client, response);
    

	// if you receive back a "ERR_NO_NOTIFICATION" free the mutex and return the error
	if( atoi( response.c_str() ) == ERR_NO_NOTIFICATION){
		pthread_mutex_unlock(&mutex);
		return ERR_NO_NOTIFICATION;
	}

	item->cl_sk.push_back(sk_client);
	item->fs->num_current_cl++;
	fs.ip = item->fs->ip;
	fs.port = item->fs->port;
	messaggio.str("");
	messaggio<<fs.ip<<"|"<<fs.port;
	send_msg(sk_client,messaggio.str());
	pthread_mutex_unlock(&mutex);
	return OK;

}

void Repository::first_fs(string file_name, std::list<location>::iterator& item){
    
	item = (file_list.find(file_name)->second).begin() ;

}
