#include "../include/fs_struct_data.h"

Fs::Fs(string ip, int port, int maxcl){

         this->ip = ip;
         this->port = port;
         this->num_maxcl = maxcl;
         this->num_current_cl=0;
         this->next = NULL;


}

void Fs::add_file(string file_name){

         files_registered.push_back(file_name);
         
}

int Fs::remove_file(string file_name){
         //check if the file is in the list
         list<string>::iterator found = find( files_registered.begin(), files_registered.end(), file_name);
         if(found == files_registered.end())
                  return ERR_NOT_FILE_REGISTERED;
         files_registered.remove(file_name);
         return 0;
}

bool Fs::busy(){
   
   if (num_maxcl == num_current_cl){
        return true;
   }

   return false;
    
   
}


Fs_list::Fs_list(){
   first=NULL;
   last=NULL;
}

void Fs_list::add_fs(string ip, int port, int maxcl){
     
    Fs* elem = new Fs(ip, port, maxcl);

    if ( (first == NULL) && (last == NULL))
    {  
          first=elem;
          last=elem;
          elem->next=NULL;
    }
    else
    {
  
          elem->next=first;
          first=elem;   
    }      
    
}

int Fs_list::remove_fs(string ip, int port){

   Fs *temp, *prec;

   temp=first;

   if( (temp->ip == ip) && (temp->port == port) )
   {
        first=temp->next;
        delete temp;
        return 0;
   }

   prec = temp;
   while(temp!=NULL)
   {
       temp=temp->next;
       if( (temp->ip == ip) && (temp->port == port) )
                             break;
       prec=temp;
   }

   if(temp == NULL)
         return -1;
   prec->next = temp->next;
   delete temp;
   return 0;
}

Fs* Fs_list::search_fs(string ip, int port){

    Fs *temp=first;
    Fs *elem=NULL;
    while( (elem==NULL) || (temp!=last))
    {
        
         if( (temp->ip == ip) && (temp->port == port)){
                     elem=temp;
                     break;
         }
         temp=temp->next;
    }
    return elem;

}

