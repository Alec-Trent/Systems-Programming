#include "hmalloc.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

/*You may include any other relevant headers here.*/


/*	main()
 *	Use this function to develop tests for hmalloc. You should not 
 *	implement any of the hmalloc functionality here. That should be
 *	done in hmalloc.c
 *	This file will not be graded. When grading I will replace main 
 *	with my own implementation for testing.*/
int main(int argc, char *argv[]){
   	/*
	
	void * p;    
	p = hmalloc(32);
        //traverse();

	
	void * q;
        q = hmalloc(45);
	//traverse();


        hfree(p);
	//traverse();


	hfree(q);
	//traverse();


	void * r;
        r = hmalloc(64);
	//traverse();


	void * s;
        s = hmalloc(48);
	//traverse();


	hfree(r);
	//traverse();

	hfree(s);
	traverse();

*/

	void * p;
        p = hmalloc(32);

	void * r;
        r = hmalloc(42);

	void * s;
        s = hmalloc(52);

	hfree(p);

	void * t;
	t = hmalloc(20);
	
	hfree(r);
	hfree(s);
	traverse();
	hfree(t);



	/*
	 void * p;
        p = hmalloc(32);
	
	 hfree(p);

	 void * r;
        r = hmalloc(12);

	traverse();
*/
	return 1;
}
