#ifndef DFR_H
#define DFR_H


/**
 * @mainpage
 *
 * @author Pischedda Alessandro
 *
 * @section LEGEND
 * 
 * DFR -> Distribuited File "Repository" @n
 * FS  -> File Server @n
 * CL  -> Client @n
 *
 *
 *
 * @section DESCRIPTION
 * 
 * The project realize a file distribuited repository. It is composed by the
 * following components :
 *  - DFR 
 *  - FS
 *  - CL
 *
 *
 * @subsection DFR
 * 
 * It is a multithread process which implements the repository's abstraction, accept connection from CL and FS. The CLs request a file to DFR and if is it (the file) available the DFR tell him about the FS which has the file. The FS can register on DFR their file in order to make them available.
 *
 * @subsection FS
 * 
 * It's a multithread process, it handles the client's requests and communicate with the DFR to register o deregister his files.
 *
 * @subsection CL
 * 
 * It requests a file to the DFR and if is it available make a request to the relative FS.
 *
 *
 *
 * @section HOW TO COMPILE
 * 
 * Using a shell go to the root directory and type :
 *
 * @code make @endcode
 *
 * When it ends you can find the executables in the directories in "bin" (directory).
 * It's possible to compile specific modules using :
 *
 * @code     make dfr @endcode
 * @code     make fs @endcode
 * @code     make cl @endcode
 *
 *
 * @section HOW TO USE
 * 
 *
 * @subsection DFR
 *
 * @code      ./DFR <port for FSs> <port for CLs> @endcode
 *
 * @subsection FS
 *
 * @code      ./FS <DFR ip> <DFR port> <ip to CL's requests> <port for CL> <max number of cl it must handle> <file1> <max cl for file1>
		<file2> <max cl for file2> .... @endcode
 *
 * @subsection CL
 *
 * @code       ./CL <DFR ip> <DFR port> <mode> <file to be requested> @endcode
 *
 * The parameter "mode" is used to the tests.
 *
 * @section TEST
 * 
 * The directory "test" there is a script used to execute one of 2 available tests. To execute one of the tests type in the shell :
 *
 * @code       ./test @endcode
 *
 * At the end of test the you can find the file downloaded by the CLs in test/CL.
 *
 * The script "reset" will be executed every time you start a test, it is used to remove the files created in previous test.
 *
 * !!!ATTENTION!!!
 *
 * The first time you want execute a test you must execute the setup script :
 *
 * @code      ./setup @endcode
 *
 * this creates the directories and copy the executable files in the test directory.
 *
 * @subsection TEST1
 * 
 * There are 2 FS, each one specify 3 files (one of them is the same for the FS), for each file they can handle only 2 CL. 10 CL try, periodically, to access at the common file.
 *
 * @subsection TEST2
 * 
 * 10 CL request to be notified when a certain file, that currentely doesn't exist, is registered by some FS. After that a FS register the file for a max of 3 CL. Every CL is notified.
 *
 * Alla fine dei test nella cartella CL si possono trovare i file richiesti dai vari CL.
 *
 *
 * @section PROBLEMI
 * 
 * @b P - Script aren't executable.
 *
 * @b S - Use 
 *	@code chmod +x <nome file> @endcode
 *
 */



/**
 * This componenr it's a multithread process, it is used to realize a repository
 * abstraction. It accept connections from FSs and CLS.
 * The CLs request a file at the DFR, if the file exist then it give back info 
 * about the FS which has it. Instead the FSs can un/register a file to the DFR.
 */
#include "../include/repository.h"
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "../include/comunication.h"
#include "../include/utility.h"
#include "../include/message_FS_DFR.h"
#include "../include/DFR_FS.h"
#include "../include/message_CL_DFR.h"
#include "../include/DFR_CL.h"


int main(int argc, char* argv[]);

#endif 
