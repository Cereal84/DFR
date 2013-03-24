/* 
 * File:   message_FS_DFR.h
 * Author: Alessandro Pischedda
 *
 */

#ifndef _MESSAGE_FS_DFR_H
#define	_MESSAGE_FS_DFR_H

#include "common_messages.h"

/** Error code definition
 *
 */
#define ERR_FILE_NOT_REGISTERED -1


/** FS's commands definition
 *
 */

#define CONNECT 1
#define DISCONNECT 2
#define REGISTER_FILE 3
#define UNREGISTER_FILE 4
#define GET_FILE 14

#endif	/* _MESSAGE_FS_DFR_H */

