#ifndef fs_struct_data_h
#define fs_struct_data_h

#include <iostream>
#include <string>
#include <list>
#include "message_CL_DFR.h"
#include "message_FS_DFR.h"

using namespace std;

/// It contains the informations about a File Server
/**
  * It contains the informations about a File Server as : 
  *      ip address
  *      port
  *      max number of clients
  */
class Fs{
   public:
     string ip;
     int port;
     int num_maxcl;
     int num_current_cl;
     Fs *next;

   public:
     /// Constructor
     /**
      * Constructor
      * @param ip    File Server ip address
      * @param port  File server port
      * @param maxcl max number of clients accepted
      */
     Fs(string ip, int port, int maxcl );

     /** Add a file to the registered files.
      *  @param file_name file to be added
      */
    // void add_file(string file_name);
   
     /** rimuove un file alla lista di file registrati
      *  @param file_name nome del file da rimuovere
      *  @return ERR_NOT_FILE_REGISTERED se il file non è presente nella lista dei file registrati  0 se ha cancellato il file dalla lista
      */
    // int remove_file(string file_name);

     /// Notify if a FS can accept others client or not
     /**
       *
       * @return FALSE if it can serve others client, TRUE otherwise
       */       
     bool busy();
};


/// Contains a list with FS informations.
/**
  * Realize a list that contains the informations of registered FSs.
  * @see class Fs
  */
class Fs_list{
   
   Fs* first;
   Fs* last;
      
   public:
      /// Costructor
      /**
       * Build an empty list.
       */
      Fs_list();
      
      /** Add a File Server
       * Add a FS to the list
       * @param ip    FS IP address
       * @param port  FS port
       * @param maxcl max number of client which a FS can handle at the same time
       */   
      void add_fs(string ip, int port, int maxcl);

      /** Remove a FS
       * Remove a FS from the list
       * @param ip    FS IP address
       * @param port  FS port
       * @return -1 FS not found, 0 FS removed
       */   
      int remove_fs(string ip, int port);

      /** Find a FS
       * Find a FS and retun his pointer.
       * @param ip    FS IP address
       * @param port  FS port
       * @return the class pointer of THE FS if it exist, NULL otherwise
       */   
      Fs* search_fs(string ip, int port);    
}; 

#endif
