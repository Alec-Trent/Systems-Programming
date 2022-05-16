#include "hmalloc.h"
#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
/*You may include any other relevant headers here.*/


/*Add additional data structures and globals here as needed.*/
void *free_list = NULL;

/* traverse
 * Start at the free list head, visit and print the length of each
 * area in the free pool. Each entry should be printed on a new line.
 */
void traverse(){
   /* Printing format:
	 * "Index: %d, Address: %08x, Length: %d\n"
	 *    -Index is the position in the free list for the current entry. 
	 *     0 for the head and so on
	 *    -Address is the pointer to the beginning of the area.
	 *    -Length is the length in bytes of the free area.
	 */
	if(free_list != NULL){
		int link = 0;	
		void * temp = free_list;
		int Index = 0;
		do{
			Index = Index + 1; //index
			int * Address = temp; //address
                        int Length = *(int *)(temp);//length
			link = *(int *)(temp + 4); //link to next node
			temp = temp + *(int *)(temp + 4); //Next Node
			printf("Index: %d, Address: %08x, Length: %d\n", Index, Address, Length);
		}while(link != 0);
	}
}

/* hmalloc
 * Allocation implementation.
 *    -will not allocate an initial pool from the system and will not 
 *     maintain a bin structure.
 *    -permitted to extend the program break by as many as user 
 *     requested bytes (plus length information).
 *    -keeps a single free list which will be a linked list of 
 *     previously allocated and freed memory areas.
 *    -traverses the free list to see if there is a previously freed
 *     memory area that is at least as big as bytes_to_allocate. If
 *     there is one, it is removed from the free list and returned 
 *     to the user.
 */
void *hmalloc(int bytes_to_allocate){

	void * spaceToAllocate = free_list;
	int link = 0;
	if(free_list == NULL){ //if null allocate space
	void * spaceToAllocate = sbrk(bytes_to_allocate + 8);
                *(int *)(spaceToAllocate) = bytes_to_allocate;
                *(int *)(spaceToAllocate + 4) = 0;
                return spaceToAllocate + 8;
	} else { //else check to see if it fits in a prev malloc or create a new one
		void * temp = NULL;

		do{
			if(bytes_to_allocate <= *(int *)(spaceToAllocate)){ //check to see if it fits in an old malloc
                        	*(int *)(spaceToAllocate + 4) = 0;
				if(temp == NULL){ // If prev node is null then theres only one node
					if(*(int *)(spaceToAllocate + 4) == 0){ 
						free_list = NULL;
					} else {
						free_list = free_list + *(int *)(free_list + 4);
					}
					
					return spaceToAllocate + 8;
				} else { //there is a previous node
					*(int *)(temp + 4) = *(int *)(spaceToAllocate + 4) + *(int *)(temp + 4); // set Prev node link to the sum of the Prev and the Current to reach new node
					return spaceToAllocate + 8;
				}
			} else {
                        	link = *(int *)(spaceToAllocate + 4);// current nodes linker value
				temp = spaceToAllocate;
                        	spaceToAllocate = spaceToAllocate + *(int *)(spaceToAllocate + 4); //Current Node
			}
		}while(link != 0);
		//create space for new malloc
		void * spaceToAllocate = sbrk(bytes_to_allocate + 8);
		*(int *)(spaceToAllocate) = bytes_to_allocate;
                *(int *)(spaceToAllocate + 4) = 0;
                return spaceToAllocate + 8;
	}
}	
	
/* hcalloc
 * Performs the same function as hmalloc but also clears the allocated 
 * area by setting all bytes to 0.
 */
void *hcalloc(int bytes_to_allocate){
	void * result = hmalloc(bytes_to_allocate);
	for(int i = 0; i < bytes_to_allocate; i++){
		*(int *)(result + i) = 0;
	}
	return result; //placeholder to be replaced by proper return value
}

/* hfree
 * Responsible for returning the area (pointed to by ptr) to the free
 * pool.
 *    -simply appends the returned area to the beginning of the single
 *     free list.
 */
void hfree(void *ptr){
	void * temp = free_list;
	if(free_list == NULL){
		free_list = ptr - 8;
		*(int *)(free_list + 4) = 0;
	} else{
		free_list = ptr - 8;
		*(int *)(free_list + 4) = temp - free_list;
	}
}

/* For the bonus credit implement hrealloc. You will need to add a prototype
 * to hmalloc.h for your function.*/

/*You may add additional functions as needed.*/



