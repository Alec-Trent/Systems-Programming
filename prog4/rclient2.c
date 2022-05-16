#include "r_client.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
/* Add any includes needed*/

/* entry
 * This is essentially the "main" for any user program linked with
 * r_client. Main in r_client will establish the connection with the
 * server then call entry. From entry, we can implement the desired 
 * user program which may call any of the r_ RPC functions.
 *
 * rclient2 should open a local file as output and a remote file as input. 
 * It should seek the remote file to position 10 and copy the rest 
 * to the local file.
 */
int entry(int argc, char* argv[]){
	//you will call the r_ functions here.
	
	//open input remotely
	int infd = r_open(argv[0], O_CREAT | O_RDWR | O_APPEND, 0666);      

	//open output locally
        int outfd = open(argv[1], O_RDWR, 0666);

	//lseek input file to pos 10
	r_lseek(infd, 10, SEEK_SET);

	//copy input to output file
	unsigned char * charBuff = malloc(1);
        int red = r_read(infd, charBuff, 1);
        while(red != 0){
               write(outfd, charBuff, 1);
                red = r_read(infd, charBuff, 1);
        }

	free(charBuff); //free mem

	//close both
        r_close(infd);
        close(outfd);
	return 0; //placeholder
}
