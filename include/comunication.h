/** This file contains a mine implementation of a send (send_msg) and receive
 *  Author : Pischedda Alessandro
 */


#ifndef comunication_h
#define comunication_h

#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <string>

using namespace std;




/** send_msg() is used to send a message through a socket
 *  @param sd socket 
 *  @param msg message
 *  @return -1 is an error occur 0 otherwise
 */
int send_msg(int sd, string msg);

/** receive_msg() is used to receive a message from a socket
 *  @param sd socket
 *  @param msg string where store the message
 *  @return -1 is an error occur 0 otherwise
 */
int receive_msg(int sk, string &msg);

#endif
