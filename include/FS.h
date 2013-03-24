/* 
 * File:   FS.h
 * Author: Alessandro
 *
 * Created on 5 aprile 2010, 1.43
 */

/**
 * Multithread process. It handles the requests that coming from the clients and send command to the DFR.
 */

#ifndef _FS_H
#define	_FS_H

#include <map>
#include <string>
#include <pthread.h>
#include <stdlib.h>
#include "../include/DFR_FS.h"
#include "../include/comunication.h"
#include "../include/utility.h"
#include <iostream>
#include <sstream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

using namespace std;

///Usato dal File Server per memorizzare il numero di client limite per il file e il numero massimo di client correnti che stanno scaricando il file
/** @struct file_info
 * Struttura dati usata dal File Server per memorizzare il numero di client limite
 * per il file e il numero massimo di client correnti che stanno scaricando il file
 */
struct file_info{
    int current_cl;
    int max_cl;
};

/**
 * Contains the list of file which it has and their informations.
 */
map<string, file_info> list_file;

/**
 * Mutex used to handle the list_file map.
 */
pthread_mutex_t mutex;

/**
 * Handles the registration of a file 
 * @param cmd commnad received from the console
 * @param socket socket used to send the command to the DFR
 */
void register_file(string cmd, int socket);

/**
 * Handles the unregistration of a file
 * @param cmd commnad received from the console
 * @param socket socket used to send the command to the DFR
 */
void unregister_file(string cmd, int socket);

/**
 * Disconnect the FS from the DFR
 * @param socket socket used to send the command to the DFR
 * @return TRUE for success, otherwise FALSE if an error occur
 */
bool disconnect(int socket);

/**
 * This function realize a simple console where write down the commands
 * @param cmd command
 * @param sk socket descriptor to the DFR
 * @disconnect TRUE if the disconnection has success
 */
void console(string cmd, int sk, bool &disconnect);

int main(int argc, char* argv[]);

#endif	/* _FS_H */

