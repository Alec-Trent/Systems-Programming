#include "recordio.h"
#include <sys/stat.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
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

/* rio_open
 * Open data file and index file. If create is requested, open both 
 * the data file and the index file with O_CREAT. If O_RDONLY or
 * O_RDWR is requested, make sure that the index file is present and 
 * return an error otherwise. On success the data file's descriptor
 * should be returned and the file descriptor for the index file
 * must be maintained within the abstraction.
 */
int rio_open(const char *pathname, int flags, mode_t mode){
	int dataFd;
	int indexFd;	
	struct record_descriptor rd;

	char * buffer = malloc(6 + strlen(pathname));
	//append file ext and name
        char ext[] = ".rinx.";
	strcat(buffer, ext);
        strcat(buffer, pathname);
	
	if((flags & O_CREAT) == O_CREAT){
	       //create both the data and index file	
		dataFd = open(pathname, O_CREAT, mode);
		indexFd = open(buffer, O_CREAT, mode);

		//check state of each file
		if(indexFd | dataFd == -1){
                         return(-1);
		}
	}else{
		//check to make sure file exists if we did not create one
		int testFd = open(pathname, mode);
        	if(testFd == -1){ //file DNE
			close(testFd);
                	return(-1);
        	}

		//open these existing files
		dataFd = open(pathname, flags, mode);
                indexFd = open(ext, flags, mode);
		//printf("indexfd: %d\n", indexFd);
		//printf("datafd: %d\n", dataFd);
		
		//check to make sure open did not fail
		if(indexFd == -1 | dataFd == -1){
			close(indexFd);
			close(dataFd);
			return(-1);
		}	
        }
	//may need more flag cases
	//free(buffer);
	int returnFd = (dataFd << 16) | indexFd;
        return returnFd; //return file desc
}

/* rio_read
 * Allocate a buffer large enough to hold the requested record, read 
 * the record into the buffer and return the pointer to the allocated 
 * area. The I/O result should be returned through the return_value 
 * argument. On success this will be the number of bytes read into
 * the allocated buffer. If any system call returns an error, this
 * should be communicated to the caller through return_value.
 */
void *rio_read(int fd, int *return_value){
	//bit shift unpack here
        int dataFd = (fd >> 16) & 0xFFFF;
        int indexFd = fd & 0xFFFF;

	struct record_descriptor rd;
//	printf("%d\n", 1);
	//printf("positn %d\n", rd.position);
	//printf("legn %d\n", rd.length);
	
	int ibr = read(indexFd, &rd, sizeof(struct record_descriptor));
	
	//int ibr = read(indexFd, &rd, 1);
	
//	printf("positn %d\n", rd.position);
  //      printf("legn %d\n", rd.length);
	

	int temp = rd.length;
        rd.length = rd. position;
        rd.position = temp;
	
///	printf("positn %d\n", rd.position);
   //     printf("legn %d\n", rd.length);


	//create buffer
	void * dataBuffer = malloc(rd.length + 1);
	if(dataBuffer == NULL){
		return(void *)(-1);
	} 
	
//	printf("%d\n", 2);
	//read data file
	//int ibr = read(indexFd, &rd, 1);
	int bytesRead = read(dataFd, dataBuffer, rd.length);
	//int ibr = read(indexFd, &rd, 1);
	//bytesRead = bytesRead + 1;
//	printf("ibr %d\n", ibr);
//	printf("br %d\n", bytesRead);
//	printf("len %d\n", rd.length);

	if(bytesRead == -1 | ibr == -1){
		return(-1);
	}	

	//check to make sure file states are same
	//if(strlen(bytesRead) != rd.length){
	///	return(void *)(-1);
	///}
	
//	printf("%d\n", 3);
	//pack null terminator
        //dataBuffer[rd.length + 1] = '\0'; //use for testing to printf

	*return_value = bytesRead;
//	printf("%d\n", 4);
	return dataBuffer;
}

/* rio_write
 * Write a new record. If appending to the file, create a record_descriptor 
 * and fill in the values. Write the record_descriptor to the index file and 
 * the supplied data to the data file for the requested length. If updating 
 * an existing record, read the record_descriptor, check to see if the new 
 * record fits in the allocated area and rewrite. Return an error otherwise.
 */
int rio_write(int fd, const void*buf, int count){
	//bit shift unpack here
        int dataFd = (fd >> 16) & 0xFFFF;
        int indexFd = fd & 0xFFFF;
	struct record_descriptor rd;
	char ch;
	int dataWr;
	
	//printf("%d\n", 1);

	int bytesRead = read(indexFd, &rd, sizeof(struct record_descriptor));
	rd.length = bytesRead;
	//printf("len1 %d\n", rd.length);	

	//write to end of the file
	if(bytesRead == 0){ //at the end of the file
		//make new record desctiptor
	//	printf("%d\n", 2);
		int offsetBytes = lseek(dataFd, -1,  SEEK_END); //position to the end of the file
		if(offsetBytes == -1){
			return(-1);
		}else{
	//		printf("%d\n", 3);
			rd.position = offsetBytes;//update rd
                        rd.length = count;
                        int indexWr = write(indexFd, &rd, sizeof(struct record_descriptor));
                        int dataWr = write(dataFd, buf, count);
		}
	}else{ //check lenght to see if it can fit, else error
		//printf("len %d\n", rd.length);
	//	 printf("count %d\n", count);
		if(rd.length > count){
	//		printf("%d\n", 4);
			rd.length = count;
			//int indexOff = lseek(indexFd, sizeof(struct record_descriptor), SEEK_SET);//correct position
			int indexWr = write(indexFd, &rd, sizeof(struct record_descriptor)); //overwrite old data
			//int dataOff = lseek(dataFd, rd.position, SEEK_SET);//correct data file positioning
			int dataWr = write(dataFd, buf, count);// overwrite data
		}else{
	//		printf("%d\n", 5);
			return(-1);
		}
	}
//	printf("%d\n", 6);
	return dataWr; //return from write()
}


/* rio_lseek
 * Seek both files (data and index files) to the beginning of the requested 
 * record so that the next I/O is performed at the requested position. The
 * offset argument is in terms of records not bytes (relative to whence).
 * whence assumes the same values as lseek whence argument.
 *
 */
int rio_lseek(int fd, int offset, int whence){
	//bit shift unpack here
        int dataFd = (fd >> 16) & 0xFFFF;
        int indexFd = fd & 0xFFFF;
	int ch;
	struct record_descriptor rd;

	//seek data at whence provided
	int indexOff = lseek(indexFd, offset*8, whence);
	//printf("indexoff %d\n", indexOff);
	//printf("pos before %d\n", rd.position);
	int indexBr = read(indexFd, &rd, sizeof(struct record_descriptor));
	//printf("pos before %d\n", rd.position);
	//printf("len before %d\n", rd.length);
	//printf("error %d\n", errno);
	//seek index at whence provided

	int temp = rd.length;
	rd.length = rd. position;
	rd.position = temp;
	
	int dataOff = lseek(dataFd, rd.position, SEEK_SET);
	int indexOf = lseek(indexFd, offset*8, whence);
	//printf("doff %d\n", dataOff);
	//printf("pos aft %d\n", rd.position);
        //printf("len aft %d\n", rd.length);

	if(indexBr == -1){
		return(-1);
	}
	
	//printf("posIT %d\n", rd.position);

	//return indexOff;
	return indexOf;
}


/* rio_close
 * Close both files. Even though a single integer is passed as an argument, 
 * your abstraction must close the other file as well. It is suggested 
 * in rio_open that you return the descriptor obtained by opening the data file 
 * to the user and keep the index file descriptor number in the 
 * abstraction and associate them. You may also shift and pack them together 
 * into a single integer.
 */
int rio_close(int fd){
	//bit shift unpack here
	int dataFd = (fd >> 16) & 0xFFFF;
        int indexFd = fd & 0xFFFF;
 	
	close(dataFd);
	close(indexFd);
	return 0; //replace with correct return value
}
