#include "r_client.h"
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
/* Add any includes needed*/

/* entry
 * This is essentially the "main" for any user program linked with
 * r_client. Main in r_client will establish the connection with the
 * server then call entry. From entry, we can implement the desired 
 * user program which may call any of the r_ RPC functions.
 *
 * rclient1 remotely opens an output file, locally opens an input file, 
 * copies the input file to the output file and closes both files.
 */
int entry(int argc, char* argv[]){
	//you will call the r_ functions here.
	//printf("hello world\n");
	
	//open input locally
	int infd = open(argv[0], O_CREAT | O_RDWR | O_APPEND, 0666);
	
	//open output remotely
	int outfd = r_open(argv[1], O_RDWR, 0666);


	//Dupe Testing
	/*int newNew = 6;	
	int duped = r_dup2(outfd, newNew);
	printf("duped: %d\n", duped);*/


	//Pipe Testing
	/*int fds[2];
	char * message = malloc(100);
	message = "I like ham.";

	r_pipe(fds);
	
	r_write(fds[1], message, 12);
	r_read(fds[0], message, 4);

	printf("message: %s\n", message);

	r_write(fds[1], message, 12);

	r_close(fds[1]);
	r_close(fds[0]);*/


	//copy input to output file 
	unsigned char * charBuff = malloc(1);
	int red = read(infd, charBuff, 1);
	while(red != 0){
		r_write(outfd, charBuff, 1); //SWITCH
		red = read(infd, charBuff, 1);
	}

	free(charBuff); //free mem

	//close both
	close(infd);
	r_close(outfd);
	//close(duped);

	return 0;
}
