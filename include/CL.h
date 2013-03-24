/* 
 * File:   CL.h
 * Author: Alessandro Pischedda
 *
 * Created on 14 april 2010, 12.25
 */

/**
 * The proces request a file to the DFR.
 */

#ifndef _CL_H
#define	_CL_H
#include "../include/comunication.h"
#include "../include/utility.h"
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>


/**
 * Notify the DFR that the file it is downloaded
 * @param file_name file name downloaded
 * @param socket socket_descriptor of DFR
 */
void end_file_transfer(string file_name, int socket);

/**
 *  Handle the procedure to request the download of a particular file in a
 *  certain FS, it is specified using IP address and port.
 *
 *  @param file_name file name to be downloaded
 *  @param new_file_name the new filename
 *  @parm ip_fs File Server IP address
 *  @param port_fs File Server port
 *  @return
 *      ERR_TOO_MANY_CLIENTS if the FS is too busy
 *      ERROR an unknown error happened
 *      FILE_NOT_EXISTS the requested file does not exist
 *      OK everything goes fine
 */
int get_file(string file_name, string new_file, string ip_fs, int port_fs);


/**
 * Handle the operations to request a file at the DFR and then download it.
 * The parameter "new_file_name" is needed due this function use the method get_data.
 * 
 * @param file_name file name to be downloaded
 * @param new_file_name new file name
 * @param socket socket_descriptor to DFR
 * @param ip FS IP address
 * @param port FS port
 * @return
 *      OK means that the file it is downloaded
 *      ERR_FILE_NOT_EXISTS  the file isn't in the repository
 *      ERR_TOO_MANY_CLIENT the FS it is too busy
 *
 */
int start_transfer_file(string file_name,string new_file_name ,int socket,string &ip, int &port);


/**
 * Handle the notify for a file that will be downloaded
 * @param file_name filename to be requested
 * @param new_file_name name of the new file
 * @param socket socket_descriptor to DFR
 * @param ip indirizzo FS IP address
 * @param port FS port
 * @return
 *      OK 
 */
int notify(string file_name, string new_file_name, int socket, string &ip, int &port);


/**
 * Disconnect 
 * @param socket socket descriptor associated
 * @return  TRUE  everything goes well
 *          FALSE an error occur
 */
bool disconnect(int socket);


void cl_test2(int socket, string file_request, string new_file_name);

void cl_test1(int socket, string file_request, string new_file_name);

#endif	/* _CL_H */

