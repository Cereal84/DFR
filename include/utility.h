#ifndef utility_h
#define utility_h

#include <string>
#include <stdio.h>
#include <arpa/inet.h>
#include "../include/message_FS_DFR.h"
#include "../include/message_CL_DFR.h"

using namespace std;

/// Queue length
#define BACKLOG_LENGTH 20


/** @brief basic socket initialization
 *  Basic socket initialization
 *  @param port port
 *  @param ip ip address
 *  @param addr informations about the address
 *  @return -1 if an error occur, otherwise return  the socket descriptor
 */
int init_sd(int port, string ip, sockaddr_in &addr);

/** @brief Socket initialization for a server
 *  Socket initialization for a server
 *  @param port port
 *  @param ip ip address
 *  @param addr 
 *  @return -1 if an error occur, otherwise return  the socket descriptor
 */
int init_sd_srv(string ip , int port);

/** @brief Socket initialization for a client
 *  Socket initialization for a client
 *  @param port port
 *  @param ip ip address
 *  @param addr 
 *  @return -1 if an error occur, otherwise return  the socket descriptor
 */
int init_sd_cl(string ip, int port);



string cut_str(string &msg, string separator);


#endif 
