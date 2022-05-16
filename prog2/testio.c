#include "recordio.h"
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
/* Develop a test program called testio which includes recordio.h and 
 * is linked against recordio.o. This program should expect a single 
 * argument which supplies a file name. The program should rio_open 
 * the supplied argument and report any errors to the user. Upon a 
 * successful open it should execute a series of rio_read statements, 
 * read the file one record at a time and write each record to the 
 * standard output as shown below.
 *
 * Data File                          Index file
 * ---------------------------------------------------
 *  Systems                            0, 8
 *  programming is cool.               8, 20
 *
 * "make testio" will compile this souce into testio
 * To execute your program: ./testio <some_record_file_name>
 */
int main(int argc, char *argv[]){	
	printf("Data File                          Index file\n");
	printf("---------------------------------------------------\n");
	//open file
	int fd = rio_open(argv[1], O_RDWR, 0600);
//	printf("fd: %d\n", fd);
	
	if(fd == -1){
		return(-1);
	}

	//seek the file
        int pos = rio_lseek(fd, 0, SEEK_SET);
	printf("seek position: %d\n", pos);
	int temp = pos;

	//read it
        char * bytesRead = rio_read(fd, &pos);
        printf("%s                             %d,%d\n", bytesRead, temp, pos);


	//seek the file
        int posit = rio_lseek(fd, 1, SEEK_SET);
        printf("seek position: %d\n", posit);
	int tempo = posit;


	//read it
        char * byteRead = rio_read(fd, &posit);
        printf("%s                             %d,%d\n", byteRead, tempo, posit);


	//write to the file
  //      int a = rio_write(fd, "System\n", 7);
    //    int b = rio_write(fd, "programming is cool\n", 20);
      //  printf("read1: %d\n", a);
       // printf("read2: %d\n", b);
	
	//close
	int answ = rio_close(fd);
	//printf("close return: %d\n", answ);
	return 0;
}

