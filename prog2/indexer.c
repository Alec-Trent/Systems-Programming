#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
/* You must use this struct when constructing records. It is
 * expected that all binary data in the index file will follow
 * this format. Do not modify the struct.
 */
struct record_descriptor
{
	int position; //byte offset relative to the beginning of the data file
	int length;   //length of record in bytes
};

/* main - indexer
 * Develop a single standalone program called indexer which creates an 
 * index file for a text file whose title is supplied as an argument to 
 * the program. This program should read the text file from beginning 
 * till end, find the beginning and ending of each line (ending with 
 * the newline character), create a descriptor and write the descriptor 
 * to the created index file. The program should not modify the text 
 * file which is supplied as an argument.
 * 
 * "make indexer" will compile this source into indexer
 * To run your program: ./indexer <some_text_file>
 */
int main(int argc, char *argv[]){
	/* The index file for record i/o is named by following the 
	 * convention .rinx.<data file name> (i.e., if the data file
	 * is named myfile.txt, its index will be .rinx.myfile.txt). 
	 * This convention conveniently hides the index files while
	 * permitting easy access to them based on the data file name. 
	 */

	struct record_descriptor rd;
	rd.position = 0;
	rd.length = 0;
	char ch;
	int bytesRead;

	char * buffer = malloc(6 + strlen(argv[1]));
	//append file ext and name
	char ext[] = ".rinx.";
	strcat(buffer, ext);
	strcat(buffer, argv[1]);

	//open data file and create index file
	int dataFd = open(argv[1], O_RDONLY, 0600);
	int indexFd = open(buffer, O_CREAT | O_RDWR, 0600);
	
	printf("data %d\n", dataFd);
	printf("index %d\n", indexFd);


	int seek = lseek(dataFd, 0, SEEK_SET);
	int seek1 = lseek(indexFd, 0, SEEK_SET);

	rd.position = 0;
        rd.length = 0;

	do{
		bytesRead = read(dataFd, &ch, 1);
		
		if(bytesRead != 0) {
			rd.length = rd.length + 1;

			if(ch == '\n'){
				printf("length: %d\n", rd.length);
				printf("position: %d\n", rd.position);
				int written = write(indexFd, &rd, sizeof(struct record_descriptor));
				printf("wrote %d\n", written);
				printf("error %d\n", errno);
				rd.position = rd.position + rd.length;
				rd.length = 0;
			}
		}
	}while(bytesRead != 0);
/*	//bytesRead = read(dataFd, &ch, 1);
	
	while(bytesRead != 0){
		bytesRead = read(dataFd, &ch, 1);
		if(bytesRead !=0){
			rd.length = rd.length + 1;

                	if(ch == '\n'){
                		printf("length: %d\n", rd.length);
                        	printf("position: %d\n", rd.position);
                        	int written = write(indexFd, &rd, sizeof(struct record_descriptor));
                        	printf("wrote %d\n", written);
                        	printf("error %d\n", errno);
                        	rd.position = rd.position + rd.length;
                        	rd.length = 0;
                	}
		}
	}*/

	free(buffer);
	close(dataFd);
	close(indexFd);
	return 0;
}
