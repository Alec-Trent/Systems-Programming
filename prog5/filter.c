//Add any includes you require.
#include <unistd.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <string.h>
#include <errno.h>

/*Modes*/
#define input	1
#define command	2
#define inOut	3

/* main - implementation of filter
 * In this project, you will be developing a filter program that forks
 * a child and then executes a program passed as an argument and intercepts 
 * all output and input from that program. The syntax for the execution of 
 * the filter program is given by:
 * 
 * 	filter <program name> <arguments to program>
 *
 * The following commands must be implemented:
 * 	//           : Pass the character '/' as input to the program.
 * 	/i           : Go to input only mode.
 * 	/o           : Go to input/output mode.
 * 	/c           : Go to command mode.
 * 	/m <integer> : Set the max-text, maximum number of lines to be displayed.
 * 	/k <integer> : Send the child process indicated signal.
 *
 * See the spec for details.
 * 
 * After receiving each command, the program should output a prompt indicating 
 * the current mode and if there is more data to be displayed. 
 * The prompt syntax is :
 *
 * 	<pid> m <more> #
 *
 * where <pid> is the process id of the child process, m is the current mode (
 * i (input only), c (command), o(input/output)), optional <more> is the text "more" 
 * if there is data available to be displayed, and lastly the pound character.
 */

/*Global Variables*/
int mode; //global variable for first switch 
int max = 20; //max lines of text default
//int cpid;
//child process id
pid_t cpid;


//create three pipes the input, output, and error fd of forked child
int inputPipefd[2];
int outputPipefd[2];
int errorPipefd[2];

//set of file desc
fd_set fds;

/*Function Prototypes*/
char * fncReadLine(int fd){
	char * readLine = malloc(101); //allocate space
	int ind = 0;
	do{
		int readRTN = read(fd, readLine + ind, 1); //read char, put into allocated space
		if(readRTN == 0){ //reached eof
			break;
		}
		ind = ind + 1; //increment to correct location
	}while(readLine[ind - 1] != '\n'); //havent reached null terminator yet
	
	readLine[ind] = '\0'; //pack answer
	return readLine; //return entire line
}

void fncInput(){
	//command modes to switch
	char * lRead = fncReadLine(0);
	if(lRead[0] == '/'){ //check slash for command
		switch(lRead[1]){ //read first char after /
			case 'c':	//command mode
				mode = 2;
				break;
			case 'o':	//output mode
				mode = 3;
				break;
			default:
				printf("Not a valid command\n");
		}
	}else{ //its actual input
		write(inputPipefd[1], lRead, strlen(lRead));
	}
}

void fncCommand(){
	char * lineRead = fncReadLine(0);
	if(lineRead[0] == '/'){ //check slash for command
		switch(lineRead[1]){ //read first char after /
			case 'c':	//command mode
				mode = 2;
				break;
			case 'i':	//input mode
				mode = 1;
				break;
			case 'o':	//output mode
				mode = 3;
				break;
			case 'm':	//max number of lines
				max = atoi(lineRead + 2);
				break;
			case 'k':;	//send signal
				int num = atoi(lineRead + 2);
				kill(cpid, num);
				break;
			default:
				//error
				printf("Not a valid command\n");
		}
	}else{ //error
		printf("Not a valid command\n");
	}
}

int fncOutput(int fd, timer_t tv){
	int rtnSelect, counter;
	do{
		char * line = fncReadLine(fd);
		counter = counter + 1;
		write(1, line, strlen(line));
		rtnSelect = select(fd, &fds, NULL, NULL, tv);
	}while(rtnSelect && counter < max);

	return rtnSelect; //if there is more 
}

void fncError(){
	//command modes to switch
	char * lRead = fncReadLine(2);

	//its actual input
	write(inputPipefd[1], lRead, strlen(lRead));
}

int main(int argc, char *argv[]){
	//Hint: use select() to determine when reads may execute without blocking.
	
	int stat; //used on exit

	pipe(inputPipefd);
	pipe(outputPipefd);
	pipe(errorPipefd);

	// fork a child
	cpid = fork();//child process
	if(cpid == 0){

		//child process id
		//cpid = getpid();

		//connect stdin to our pipe
		dup2(inputPipefd[0], 0);	//dup read to stin
		close(inputPipefd[1]);		//close write
		close(inputPipefd[0]);		//close read

		//connect stdout to our pipe
		dup2(outputPipefd[1], 1);	//dup write to stdout
		close(outputPipefd[0]);		//close read
		close(outputPipefd[1]);		//close write

		//connect stderr to our pipe
		dup2(errorPipefd[1], 2);	//dup write to stderr
		close(errorPipefd[0]);		//close read
		close(errorPipefd[1]);		//close write

		//setup args for exec
		char * args[100];
		args[0] = argv[1];			//file name
		int i;
		for(i = 1; i < argc-2; i = i + 1){	//args
			args[i] = argv[i+1];
		}
		args[i+1] = NULL;

		//exec program	
		execvp(args[0], args); //returned 0
		
		//exit
		exit(0);
	}
	
	//first time ran mode should be set to command
	mode = 2;
	
	struct timeval tv;
	tv.tv_sec = 0;
	tv.tv_usec = 3; //micro seconds for timout in select

	//set of file desc
	//fd_set fds;

	//parent process id
	//pid_t pid = getpid();

	//close pipe ends for parent
	close(inputPipefd[0]);          //close read
	close(outputPipefd[1]);		//close write
	close(errorPipefd[1]);          //close write

	for(;;){

		switch(mode){
			case 1:
				FD_ZERO(&fds);		//empties set fds of file descriptors
				FD_SET(0, &fds); 	//fd, add to set fds
				//FD_SET(errorPipefd[0], &fds); //inf loop , dies on exit status but still bad
				break;
			case 2:
				FD_ZERO(&fds);		//empties set fds of file descriptors
				FD_SET(0, &fds); 	//fd, add to set fds
				//FD_SET(errorPipefd[0], &fds); //inf loop , dies on exit status but still bad
				break;
			case 3:
				FD_ZERO(&fds);		//empties set fds of file descriptors
				FD_SET(0, &fds); 	//fd, add to set fds
				//FD_SET(errorPipefd[0], &fds); //inf loop , dies on exit status but still bad
				FD_SET(outputPipefd[1], &fds);
				break;
		}

		int rtnWait = waitpid(cpid, &stat, WNOHANG); //look at return val from waitpid, after child teerminate to get val of stat
		//printf("wait: %d\n", rtnWait);
		//printf("errno(%d): %s\n", errno, strerror(errno));
		if(rtnWait == -1){ // child no chang 0, fail -1			****Errno returns 10 No child process on third iteration always****
			int status = WIFEXITED(stat); // true if clean exit
			if(status == 1){ // if its true get code
				int code = WEXITSTATUS(status);
				printf("The child %d has terminated with code %d\n", cpid, code);
				exit(0);
			}
		}

		//printf("selc\n");
		select(errorPipefd[1] + 1, &fds, NULL, NULL, NULL);
		//printf("selc1\n");

		if(FD_ISSET(2, &fds) != 0){ //there is something to read from, gaurd reads from blocking
			printf("error\n");
			fncError();
		}

		//printf("selc2\n");

		//filter program will be in command mode, awaiting input
		switch(mode){	//mode is global var 
			case 1:		//input	
				//printf("no\n");
				if(FD_ISSET(0, &fds) != 0){
					fncInput();
					//Print prompts here
					printf("%d i #\n", cpid);
				}else{
					//error
					printf("Desired file descriptor cannot be read\n");
				}
				break;

			case 2:		//command
				//printf("made\n");
				if(FD_ISSET(0, &fds) != 0){
					fncCommand();
					//Print prompts here
					printf("%d c #\n", cpid);
				}else{
					//error
					printf("Desired file descriptor cannot be read\n");
				}
				break;

			case 3:		//in/out
				if(FD_ISSET(0, &fds) != 0){
					fncInput();
					//Print prompts here
					printf("%d i #\n", cpid);
				}else{
					//error
					printf("Desired file descriptor cannot be read\n");
				}

				if(FD_ISSET(1, &fds) != 0){
					int more = fncOutput(errorPipefd[1] + 1, &tv);
					//Print prompts here
					if(more != 0){
						printf("%d o more #\n", cpid);
						mode = 2; //command mode per spec
					}else{
						printf("%d o #\n", cpid);
					}
				}else{
					//error
					printf("Desired file descriptor cannot be read\n");
				}
				break;
			
			default:
				//error
				printf("Not a valid mode\n");
		}
	}
}			
