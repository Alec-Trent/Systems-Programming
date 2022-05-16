#include "r_client.h"

/* Add any includes needed*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <netdb.h>
/*Opcodes for RPC calls*/
#define open_call   1 
#define close_call  2
#define read_call   3
#define write_call  4
#define seek_call   5
#define pipe_call   6
#define dup2_call   7

int entry(int argc, char *argv[]);

/* main - entry point for client applications.
 *
 * You are expected to develop a client program which will provide an 
 * environment into which we can plug an application and execute it 
 * using the remote versions of the supported calls. The client program 
 * therefore should expect a <hostname> <portnumber> pair as its first 
 * two arguments and attempt to connect the server. Once it connects, it
 * should call the user program which has a function called entry 
 * analogous to the main program in an ordinary C program. The entry 
 * routine should have the argv and argc arguments and return an integer 
 * value, just like the ordinary main. The client program should strip 
 * off the first two arguments, create a new argv array and call the entry procedure. 
 * Finally, when entry exits, the return value should be returned from the 
 * main procedure of the client.
 */
int socket_fd;
int main(int argc, char *argv[]){
	//setup socket and connect
	char* host = argv[1];
        int port = atoi(argv[2]);
        struct sockaddr_in remote;
        struct hostent *h;
	char * arg[2];
	int argn;

        socket_fd = socket(AF_INET, SOCK_STREAM, 0);
        bzero((char*)&remote, sizeof(remote));
        remote.sin_family = AF_INET;
	h = gethostbyname(host);
	bcopy((char*)h->h_addr, (char*)&remote.sin_addr, h->h_length);
        remote.sin_port = htons(port);

        connect(socket_fd, (struct sockaddr*)&remote, sizeof(remote));
	
	//fix up arguments subtract agrv/argc? 1 and 2
	arg[0] = argv[0];
	arg[0] = argv[3];
	arg[1] = argv[4];

	argn = argc - 2;

	//you will need to call entry() rclient1 and 2 respectivley
	int rtn = entry(argn, arg);
	return rtn; // placeholder
}

/* r_open
 * remote open
 */
int r_open(const char *pathname, int flags, int mode){
	int strLength = strlen(pathname); //gives me string length of pathname
	
	int lengthOfMessage = 1 + 2 + strLength + sizeof(flags) + sizeof(mode); // length of total message
	char * message = malloc(lengthOfMessage + 1);
	char * rMessage = malloc(lengthOfMessage + 1);
	
	message[0] = 1; //opcode for open
	message[1] = (strLength >> 8) & 0xff; //length
	message[2] = (strLength) & 0xff;

	for(int i = 0; i < strLength; i++){
		message[3 + i] = pathname[i];
	}
	
	message[3 + strLength] = (flags >> 24) & 0xff; // put the flags.
	message[4 + strLength] = (flags >> 16) & 0xff;
	message[5 + strLength] = (flags >> 8) & 0xff;
	message[6 + strLength] = (flags) & 0xff;
	
	message[7 + strLength] = (mode >> 24) & 0xff; // put the mode.
	message[8 + strLength] = (mode >> 16) & 0xff;
	message[9 + strLength] = (mode >> 8) & 0xff;
	message[10 + strLength] = (mode) & 0xff;

	write(socket_fd, message, lengthOfMessage); //send to server
	read(socket_fd, rMessage, 8);	//return from server
	int recievedMessage = (rMessage[0] << 24) | (rMessage[1] << 16) | (rMessage[2] << 8) | rMessage[3];
	int recievedError = (rMessage[4] << 24) | (rMessage[5] << 16) | (rMessage[6] << 8) | rMessage[7];
	errno = recievedError;

	free(message);
	free(rMessage);

	return recievedMessage;
}

/* r_close
 * remote close
 */
int r_close(int fd){
	int lengthOfMessage = 1 + sizeof(fd); // length of total message
	char * message = malloc(lengthOfMessage);
	char * rMessage = malloc(lengthOfMessage);
	message[0] = 2; //opcode for close
	
	message[1] = (fd >> 24) & 0xff; // put the fd.
	message[2] = (fd >> 16) & 0xff;
	message[3] = (fd >> 8) & 0xff;
	message[4] = (fd) & 0xff;

	write(socket_fd, message, lengthOfMessage); //send to server
	read(socket_fd, rMessage, 8);	//return from server
	int recievedMessage = (rMessage[0] << 24) | (rMessage[1] << 16) | (rMessage[2] << 8) | rMessage[3];
	int recievedError = (rMessage[4] << 24) | (rMessage[5] << 16) | (rMessage[6] << 8) | rMessage[7];
	errno = recievedError;

	free(message);
	free(rMessage);

	return recievedMessage;
}

/* r_read
 * remote read
 */
int r_read(int fd, void *buf, int count){
	int lengthOfMessage = 1 + sizeof(fd) + sizeof(count); // length of total message
	char * message = malloc(lengthOfMessage);
	char * rMessage = malloc(lengthOfMessage);
	message[0] = 3; //opcode for read
	
	message[1] = (fd >> 24) & 0xff; // put the fd.
	message[2] = (fd >> 16) & 0xff;
	message[3] = (fd >> 8) & 0xff;
	message[4] = (fd) & 0xff;

	message[5] = (count >> 24) & 0xff; // put the count.
	message[6] = (count >> 16) & 0xff;
	message[7] = (count >> 8) & 0xff;
	message[8] = (count) & 0xff;

	write(socket_fd, message, lengthOfMessage); //send to server
	read(socket_fd, rMessage, 8);	//return from server
	int recievedMessage = (rMessage[0] << 24) | (rMessage[1] << 16) | (rMessage[2] << 8) | rMessage[3];
	int recievedError = (rMessage[4] << 24) | (rMessage[5] << 16) | (rMessage[6] << 8) | rMessage[7];
	errno = recievedError;
	read(socket_fd, buf, recievedMessage);
	
	free(message);
	free(rMessage);
	
	return recievedMessage;
}

/* r_write
 * remote write
 */
int r_write(int fd, const void *buf, int count){ //NOT DONE YET
	int lengthOfMessage = 1 + sizeof(fd) + sizeof(count) + count; // length of total message
	char * message = malloc(lengthOfMessage);
	char * rMessage = malloc(lengthOfMessage);
	const char * buffer = buf;
	message[0] = 4; //opcode for write
	
	message[1] = (fd >> 24) & 0xff; // put the fd.
	message[2] = (fd >> 16) & 0xff;
	message[3] = (fd >> 8) & 0xff;
	message[4] = (fd) & 0xff;

	message[5] = (count >> 24) & 0xff; // put the count.
	message[6] = (count >> 16) & 0xff;
	message[7] = (count >> 8) & 0xff;
	message[8] = (count) & 0xff;

	for(int i = 0; i < count; i++){
		message[9 + i] = buffer[i];
	}

	write(socket_fd, message, lengthOfMessage); //send to server
	read(socket_fd, rMessage, 8);	//return from server
	int recievedMessage = (rMessage[0] << 24) | (rMessage[1] << 16) | (rMessage[2] << 8) | rMessage[3];
	int recievedError = (rMessage[4] << 24) | (rMessage[5] << 16) | (rMessage[6] << 8) | rMessage[7];
	errno = recievedError;

	free(message);
	free(rMessage);

	return recievedMessage;
}

/* r_lseek
 * remote seek
 */
int r_lseek(int fd, int offset, int whence){
	int lengthOfMessage = 1 + sizeof(fd) + sizeof(offset) + sizeof(whence); // length of total message
	char * message = malloc(lengthOfMessage);
	char * rMessage = malloc(lengthOfMessage);
	message[0] = 5; //opcode for lseek
	
	message[1] = (fd >> 24) & 0xff; // put the fd.
	message[2] = (fd >> 16) & 0xff;
	message[3] = (fd >> 8) & 0xff;
	message[4] = (fd) & 0xff;

	message[5] = (offset >> 24) & 0xff; // put the offset.
	message[6] = (offset >> 16) & 0xff;
	message[7] = (offset >> 8) & 0xff;
	message[8] = (offset) & 0xff;

	message[9] = (whence >> 24) & 0xff; // put the whence.
	message[10] = (whence >> 16) & 0xff;
	message[11] = (whence >> 8) & 0xff;
	message[12] = (whence) & 0xff;

	write(socket_fd, message, lengthOfMessage); //send to server
	read(socket_fd, rMessage, 8);	//return from server
	int recievedMessage = (rMessage[0] << 24) | (rMessage[1] << 16) | (rMessage[2] << 8) | rMessage[3];
	int recievedError = (rMessage[4] << 24) | (rMessage[5] << 16) | (rMessage[6] << 8) | rMessage[7];
	errno = recievedError;

	free(message);
	free(rMessage);

	return recievedMessage;
}

/* r_pipe
 * remote pipe
 */
int r_pipe(int pipefd[2]){ //NOT DONE
	int lengthOfMessage = 1; // length of total message
	char * message = malloc(lengthOfMessage);
	char * rMessage = malloc(lengthOfMessage + 16);
	message[0] = 6; //opcode for pipe

	write(socket_fd, message, lengthOfMessage); //send to server
	read(socket_fd, rMessage, 16);	//return from server
	int recievedMessage = (rMessage[0] << 24) | (rMessage[1] << 16) | (rMessage[2] << 8) | rMessage[3];
	int recievedError = (rMessage[4] << 24) | (rMessage[5] << 16) | (rMessage[6] << 8) | rMessage[7];
	int pipe0 = (rMessage[8] << 24) | (rMessage[9] << 16) | (rMessage[10] << 8) | rMessage[11];
        int pipe1 = (rMessage[12] << 24) | (rMessage[13] << 16) | (rMessage[14] << 8) | rMessage[15];	
	pipefd[0] = pipe0; //read end
	pipefd[1] = pipe1; //write end
	errno = recievedError;

	free(message);
	free(rMessage);

	return recievedMessage;
}

/* r_dup2
 * remote dup2
 */
int r_dup2(int oldfd, int newfd){
	int lengthOfMessage = 1 + sizeof(oldfd) + sizeof(newfd); // length of total message
	char * message = malloc(lengthOfMessage);
	char * rMessage = malloc(lengthOfMessage);
	message[0] = 7; //opcode for dup
	
	message[1] = (oldfd >> 24) & 0xff; // put the oldfd.
	message[2] = (oldfd >> 16) & 0xff;
	message[3] = (oldfd >> 8) & 0xff;
	message[4] = (oldfd) & 0xff;

	message[5] = (newfd >> 24) & 0xff; // put the newfd.
	message[6] = (newfd >> 16) & 0xff;
	message[7] = (newfd >> 8) & 0xff;
	message[8] = (newfd) & 0xff;

	write(socket_fd, message, lengthOfMessage); //send to server
	read(socket_fd, rMessage, 8);	//return from server
	int recievedMessage = (rMessage[0] << 24) | (rMessage[1] << 16) | (rMessage[2] << 8) | rMessage[3];
	int recievedError = (rMessage[4] << 24) | (rMessage[5] << 16) | (rMessage[6] << 8) | rMessage[7];
	errno = recievedError;

	free(message);
	free(rMessage);

	return recievedMessage;
}
