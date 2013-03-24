#ifndef DFR_CL_H
#define DFR_CL_H

#include "../include/repository.h"
#include "../include/comunication.h"
#include <sstream>
#include <pthread.h>

/** @brief handle the requests for a Client
 *  It is the body of the thread which handles the connection and 
 *  the comands of a CLient
 */
void *handler_cl(void *sk_fs);


/**
 *  This thread creates a thread for each connection request.
 *
 */
void *listener_cl(void *sk);


#endif

