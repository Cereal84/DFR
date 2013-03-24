#ifndef repository_h
#define repository_h

#include "fs_struct_data.h"
#include "message_CL_DFR.h"
#include "message_FS_DFR.h"
#include "comunication.h"
#include <map>
#include <list>
#include <pthread.h>
#include <algorithm>
#include <pthread.h>



using namespace std;

///Contiene le informazioni utili ai Client per localizzare un File Server
/** @struct detail_location
 *  This structure it is used by the DFR's threads which handle FSs and contains the info
 *  to locate a FS.
 */
struct detail_location{
       ///FS's IP address
       string ip;
       ///FS's port on which it listen for client request
       int port;

};

///Contiene le informazioni, dato, un file sullo stato del File Server
/** @struct location
 *
 * Contains usefull information about a file as :
 *  - pointer to a FS which has it
 *  - a conditional semaphore for the notify when the file is available
 *  - a conditional semaphore used to know when delete a file
 *  - list of clients that are downloading the file from the FS
 * 
 *  the FS pointer is used to perform a direct access to the FS's information without searching it
 *  in the FS list.
 */
struct location{
       ///pointer to a FS object
       Fs* fs;

       ///it is used to notify the CLs when a file is available
       pthread_cond_t wait_notify;
       //it is used to delete a file waiting that all the CLs ended to download it
       pthread_cond_t wait_unregister;

       /// number of CLs that are waiting the notify or that are downloading it
       int num_wait; // it used to realize a simple load balancing mechanism

       /// contains the list of CL's sockets which reqeust the file to the FS
       list<int> cl_sk;

       bool invisible;

};

///This strcture is used to notify files that aren't available yet on the repository.
/** @struct notify_no_file
 *  This strcture is used to notify files that aren't available yet on the repository.
 */
struct notify_no_file{

       //it is used to wait that all the CLs end the downloading before delete the file
       pthread_cond_t waiting_file;

       /// number of CLs that are waiting the notify or that are downloading it
       int num_wait; // it used to realize a simple load balancing mechanism

};


///Data structure used to realize the repository
/** @class Repository
 *  This class contains the data structure and the method used to realize the repostory.
  * The structure is as follow :
  *     - FSs list
  *     - list of available files
  *     - each file can be present on more FS
  */
class Repository{


     public :
        /** FSs list
         */
        Fs_list fs_list;
   
        /** Files lists.
         *  Contains the list of files registered by each FS.
         */
        map< string ,list<location> > file_list;

        map<string, struct notify_no_file > no_file;

        ///mutex
        pthread_mutex_t mutex;

     public:

        /** @brief constructor
         *  The repository is initialize as empty.
         */
        Repository();
        

        /** @brief Add a file to the repository
         * Add a file to the repository
         *    - if it doesn't exist will be added the file and the FS associated
         *    - if it already exists will be added a location structure associated to the FS
         *  @param file_name filename to be register
         *  @param ip  FS's IP address
         *  @param port FS's port
         */
        void add_file(string file_name, string ip, int port);

     
        /** @brief  it unregisters a file of a certain FS
         * it unregistersde a file froma  certain FS if no one it is waiting a notify or is downloading it,
	 * otherwise put it as invisible. In this way no one can request a notify for that file and it can
	 * wait that everyone ends the downloading.
	 *	- removes the FS's information if the file exist on others FSs
	 *	- removes the file informations from the repository if no one FS has maintining it
         *  @param file_name filename to unregisters
         *  @param ip  FS's IP address
         *  @param port FS's port
         */
        void remove_file(string file_name, string ip, int port);


        /** @brief Add all the informations about a FS in the DFR's FSs registered list.
	 *  Add all the informations about a FS in the DFR's FSs registered list.
         *  @param ip FS's IP address
         *  @param port FS's port number
         *  @param maxcl maximum number of clients supported simultaneously by FS
         */
        void add_file_server(string ip, int port, int maxcl);
   
        /** Removes a FS from the DFR
         * Removes a FS from the DFR
         * @param ip    FS's IP address
         * @param port  FS's port number
         * @return -1 if there isn't that FS, 0 FS removed
         */  
        int remove_file_server(string ip, int port);

        /** @brief handles the command START_FILE_TRANSFER 
         *  check if a file si available on a FS
         *  @param file_name filename to be serached
         *  @param sk_client socket of the client
         *  @param fs a detail_location object
         *  @return OK , ERR_FILE_NOT_EXIST, ERR_TOO_MANY_CLIENT
         */
        int begin_file_transfer(string file_name, int sk_client, detail_location& fs);

        /** @brief handles the command END_FILE_TRANSFER
         *  check if the client had requested that file, in this case removes the
         *  informations about it
         *  @param file_name file name 
         *  @param sk_client CL's socket descriptor
         *  @param fs FS's informations on which the CL has downloaded the file
         *  @return
         *     - ERR_NO_TRANSFER if the client hasn't request that file
         *     - OK  no errors occur
         */
        int end_file_transfer(string file_name, int sk_client, detail_location fs);


        /** @brief handles the notify request for a file
         *  handles the notify request for a file
         *  @param file_name filename of which the CL want the notify
         *  @param sk_client CL socket
         *  @param fs detail_location with the FS's informations
         */
        int reg_notify(string file_name, int sk_client, detail_location& fs);

    private:

        /** @brief  Search a file in the repository
         *  Search a file and return, if exist, an object location with the informations about the FS which has it.
         *  @param file_name filename to find
         *  @param &location \see location
         *  @return
         *        - ERR_FILE_NOT_EXISTS if the file doesn't exist
         *        - ERR_TOO_MANY_CLIENTS if all the FSs which have the file are too busy
         */
        int where_is(string file_name, std::list<location>::iterator &list_iter);

        /** @brief return the informations about the first FS on the list which has the file
         *  give a file returns (on item) the informations about the fisrt FS which has this file
         *  @param file_name filename
         *  @param item iterator of the first location object
         */
        void first_fs(string file_name, std::list<location>::iterator &item );

};

#endif 
