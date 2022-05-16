/* Add any includes needed*/
#include <stdio.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <arpa/inet.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/time.h>
#include <sys/resource.h>
#include <sys/wait.h>
/*Opcodes for RPC calls*/
#define open_call   1 
#define close_call  2
#define read_call   3
#define write_call  4
#define seek_call   5
#define pipe_call   6
#define dup2_call   7

/* In this project, we will develop a mini Remote Procedure Call (RPC) 
 * based system consisting of a em server and a client. Using the remote 
 * procedures supplied by the server our client program will be able to 
 * open files and perform computations on the server. 
 *#include <arpa/inet.h>
 * The server should open a socket and listen on an available port. The 
 * server program upon starting should print the port number it is using 
 * on the standard output 
 * (Print only the port number with no additional formatting. You may use printf \%d). 
 * This port number is then manually passed as a command line argument to 
 * the client to establish the connection. In order to implement the RPC, 
 * the server and the client should communicate through a TCP socket. It 
 * is allowed to fork a child for each open connection and delagate the 
 * handling to the child.
 */

int handle_con(int fd);

/* main - server implementation
 */
int main(int argc, char *argv[]){ 
	int listener, conn; //,status
	struct sockaddr_in s1, s2;
	socklen_t length;

	listener  = socket(AF_INET, SOCK_STREAM, 0);
	bzero((char *) &s1, sizeof(s1));
	s1.sin_family = AF_INET;
	s1.sin_addr.s_addr = htonl(INADDR_ANY); // Any of this host’s interfaces is OK.
	s1.sin_port = htons(0); // bind() will gimme unique port.
	
	bind(listener, (struct sockaddr *)&s1, sizeof(s1));
	length = sizeof(s1);
	getsockname(listener, (struct sockaddr *)&s1, &length); //Find out port number
	printf("RSTREAM:: assigned port number %d\n", ntohs(s1.sin_port));
	
	listen(listener,1);
	length = sizeof(s2);

	//loop, keep listener alive 
	while(1){
		conn=accept(listener, (struct sockaddr *)&s2, &length);

		if(fork() == 0){
			handle_con(conn);
			close(listener);
		}
		close(conn);
		//while(wait3(&status, WNOHANG, NULL));
	}
	return 0; // placeholder
}

int handle_con(int fd){
	char opCode;
	char * pathname, * message, * data, whence;
	int lengthOfMessage, fileDesc, oldFileDesc, newFileDesc, flags, mode, offset, count;
	short length;
	unsigned char * len, * flag, * mod, * fileDes, * cont, * offst, * whnce, * nFileDes;
	
	int rtn = read(fd, &opCode, 1);
	while(rtn > 0){
		switch(opCode){
			case 1:
				//printf("OPEN\n");
				//read(args)
				len = malloc(2);
 				read(fd, len, 2);
				length = (len[0] << 8) | len[1];
			
				pathname = malloc(length + 1);
				read(fd, pathname, length);
				pathname[length] = '\0';

				flag = malloc(4);
				read(fd, flag, 4);
				flags = (flag[0] << 24) | (flag[1] << 16) | (flag[2] << 8) | flag[3];
				
				mod = malloc(4);
				read(fd, mod, 4);
				mode = (mod[0] << 24) | (mod[1] << 16) | (mod[2] << 8) | mod[3];

				//call kernel call
				int fd1 = open(pathname, flags, mode);

				//package response
				lengthOfMessage = 4 + 4; // length of total message				
				message = malloc(lengthOfMessage);

				message[0] = (fd1 >> 24) & 0xff; // put the returnValue.
				message[1] = (fd1 >> 16) & 0xff;
				message[2] = (fd1 >> 8) & 0xff;
				message[3] = (fd1) & 0xff;

				message[4] = (errno >> 24) & 0xff; // put the errno.
				message[5] = (errno >> 16) & 0xff;
				message[6] = (errno >> 8) & 0xff;
				message[7] = (errno) & 0xff;
				
				//write response
				write(fd, message, lengthOfMessage); //send to client
				
				//free mallocd space
				free(len);
				free(pathname);
				free(flag);
				free(mod);
				free(message);
				break;

			case 2:
				//printf("CLOSE\n");
				//read(args)
				fileDes = malloc(4);
 				read(fd, fileDes, 4);
				fileDesc = (fileDes[0] << 24) | (fileDes[1] << 16) | (fileDes[2] << 8) | fileDes[3];

				//call kernel call
				int fd2 = close(fileDesc);

				//package response
				lengthOfMessage = 4 + 4; // length of total message				
				message = malloc(lengthOfMessage);

				message[0] = (fd2 >> 24) & 0xff; // put the returnValue.
				message[1] = (fd2 >> 16) & 0xff;
				message[2] = (fd2 >> 8) & 0xff;
				message[3] = (fd2) & 0xff;

				message[4] = (errno >> 24) & 0xff; // put the errno.
				message[5] = (errno >> 16) & 0xff;
				message[6] = (errno >> 8) & 0xff;
				message[7] = (errno) & 0xff;

				//write response
				write(fd, message, lengthOfMessage); //send to client

				//free mallocd space
				free(fileDes);
				free(message);
				break;

			case 3:
				//printf("READ\n");
				//read(args)
				fileDes = malloc(4);
 				read(fd, fileDes, 4);
				fileDesc = (fileDes[0] << 24) | (fileDes[1] << 16) | (fileDes[2] << 8) | fileDes[3];
	
				cont = malloc(4);
				read(fd, cont, 4);
				count = (cont[0] << 24) | (cont[1] << 16) | (cont[2] << 8) | cont[3];
				
				data = malloc(count);				

				//call kernel call
				int fd3 = read(fileDesc, data, count);

				//package response
				lengthOfMessage = 4 + 4 + count ; // length of total message				
				message = malloc(lengthOfMessage);

				message[0] = (fd3 >> 24) & 0xff; // put the returnValue.
				message[1] = (fd3 >> 16) & 0xff;
				message[2] = (fd3 >> 8) & 0xff;
				message[3] = (fd3) & 0xff;

				message[4] = (errno >> 24) & 0xff; // put the errno.
				message[5] = (errno >> 16) & 0xff;
				message[6] = (errno >> 8) & 0xff;
				message[7] = (errno) & 0xff;

				//ADD LENGTH OF DATA READ AND MESSAGE
  		                for(int i = 0; i < count; i = i + 1){
					message[8 + i] = * data;
					data = data + 1;
				}

				//write response
				write(fd, message, lengthOfMessage); //send to client

				//free mallocd space
				free(fileDes);
				free(cont);
				//free(data);
				free(message);
				break;

			case 4:
				//printf("WRITE\n");
				//read(args)
				fileDes = malloc(4);
 				read(fd, fileDes, 4);
				fileDesc = (fileDes[0] << 24) | (fileDes[1] << 16) | (fileDes[2] << 8) | fileDes[3];
	
				cont = malloc(4);
				read(fd, cont, 4);
				count = (cont[0] << 24) | (cont[1] << 16) | (cont[2] << 8) | cont[3];

				data = malloc(count);
				
				read(fd, data, count); //FIGURE OUT VAR WRITE
				//printf("data: %s\n", data); //LOOK INTO THIS LATER

				//call kernel call
				int fd4 = write(fileDesc, data, count);

				//package response
				lengthOfMessage = 4 + 4; // length of total message				
				message = malloc(lengthOfMessage);

				message[0] = (fd4 >> 24) & 0xff; // put the returnValue.
				message[1] = (fd4 >> 16) & 0xff;
				message[2] = (fd4 >> 8) & 0xff;
				message[3] = (fd4) & 0xff;

				message[4] = (errno >> 24) & 0xff; // put the errno.
				message[5] = (errno >> 16) & 0xff;
				message[6] = (errno >> 8) & 0xff;
				message[7] = (errno) & 0xff;

				//write response
				write(fd, message, lengthOfMessage); //send to client

				//free mallocd space
				free(fileDes);
				free(cont);
				free(data);
				free(message);
				break;

			case 5:
				//printf("LSEEK\n");
				//read(args)
				fileDes = malloc(4);
 				read(fd, fileDes, 4);
				fileDesc = (fileDes[0] << 24) | (fileDes[1] << 16) | (fileDes[2] << 8) | fileDes[3];
			
				offst = malloc(4);
				read(fd, offst, 4);
				offset = (offst[0] << 24) | (offst[1] << 16) | (offst[2] << 8) | offst[3];

				whnce = malloc(4);
				read(fd, whnce, 4);
				whence = (whnce[0] << 24) | (whnce[1] << 16) | (whnce[2] << 8) | whnce[3];

				//call kernel call
				int fd5 = lseek(fileDesc, offset, whence);

				//package response
				lengthOfMessage = 4 + 4; // length of total message				
				message = malloc(lengthOfMessage);

				message[0] = (fd5 >> 24) & 0xff; // put the returnValue.
				message[1] = (fd5 >> 16) & 0xff;
				message[2] = (fd5 >> 8) & 0xff;
				message[3] = (fd5) & 0xff;

				message[4] = (errno >> 24) & 0xff; // put the errno.
				message[5] = (errno >> 16) & 0xff;
				message[6] = (errno >> 8) & 0xff;
				message[7] = (errno) & 0xff;

				//write response
				write(fd, message, lengthOfMessage); //send to client

				//free mallocd space
				free(fileDes);
				free(offst);
				free(whnce);
				free(message);
				break;

			case 6:;
				//printf("PIPE\n");
				//make buffer locally
				int pipefd[2];
				//send values filled by pipe() back to client
				
				//call kernel call
				int fd6 = pipe(pipefd);

				//package response
				lengthOfMessage = 4 + 4 + 4 + 4; // length of total message				
				message = malloc(lengthOfMessage);

				message[0] = (fd6 >> 24) & 0xff; // put the returnValue.
				message[1] = (fd6 >> 16) & 0xff;
				message[2] = (fd6 >> 8) & 0xff;
				message[3] = (fd6) & 0xff;

				message[4] = (errno >> 24) & 0xff; // put the errno.
				message[5] = (errno >> 16) & 0xff;
				message[6] = (errno >> 8) & 0xff;
				message[7] = (errno) & 0xff;

				message[8] = (pipefd[0] >> 24) & 0xff; // put the pipefd[0].
				message[9] = (pipefd[0] >> 16) & 0xff;
				message[10] = (pipefd[0] >> 8) & 0xff;
				message[11] = (pipefd[0]) & 0xff;

				message[12] = (pipefd[1] >> 24) & 0xff; // put the pipefd[1].
				message[13] = (pipefd[1] >> 16) & 0xff;
				message[14] = (pipefd[1] >> 8) & 0xff;
				message[15] = (pipefd[1]) & 0xff;

				//write response
				write(fd, message, lengthOfMessage); //send to client

				//free mallocd space
				free(message);
				break;

			case 7:
				//printf("DUPE\n");
				//read(args)
				fileDes = malloc(4);
 				read(fd, fileDes, 4);
				oldFileDesc = (fileDes[0] << 24) | (fileDes[1] << 16) | (fileDes[2] << 8) | fileDes[3];
				printf("o %d\n", oldFileDesc);
				
				nFileDes = malloc(4);
				read(fd, nFileDes, 4);
				newFileDesc = (nFileDes[0] << 24) | (nFileDes[1] << 16) | (nFileDes[2] << 8) | nFileDes[3];
				printf("n %d\n", newFileDesc);

				//call kernel call
				int fd7 = dup2(oldFileDesc, newFileDesc);

				//package response
				lengthOfMessage = 4 + 4; // length of total message				
				message = malloc(lengthOfMessage);

				message[0] = (fd7 >> 24) & 0xff; // put the returnValue.
				message[1] = (fd7 >> 16) & 0xff;
				message[2] = (fd7 >> 8) & 0xff;
				message[3] = (fd7) & 0xff;

				message[4] = (errno >> 24) & 0xff; // put the errno.
				message[5] = (errno >> 16) & 0xff;
				message[6] = (errno >> 8) & 0xff;
				message[7] = (errno) & 0xff;

				//write response
				write(fd, message, lengthOfMessage); //send to client

				//free mallocd space
				free(fileDes);
				free(nFileDes);
				free(message);
				break;

			default:
				printf("fail\n");
				exit(0); //it exits because we have a case that shouldnt be

		}
		read(fd, &opCode, 1);
	}
	exit(0);
}
