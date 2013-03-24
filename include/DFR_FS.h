#ifndef DFR_FS_H
#define DFR_FS_H

#include "../include/repository.h"
#include "../include/comunication.h"
#include <sstream>
#include <pthread.h>

/** @brief Handle the requests of a certain FS
 */
void *handler_fs(void *sk_fs);


/** 
 *  This method, foreach connection from a FS, creates a new thread which handles 
 *  the connection.
 *
 */
void *listener_fs(void *sk);

#endif 

