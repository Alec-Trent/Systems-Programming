#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions
#include <stdio.h>

void pushBuffer(unsigned char c);
/*Include any additional headers you require*/

/*You may use any global variables/structures that you like*/

/* main - dzy de-compression implementation
 * This program decompresses a compressed stream directed at its standard input 
 * and writes decompressed data to its standard output.
 *
 * To compile dzy: make dzy
 * To execute: ./dzy < somefile.encoded > somefile_decoded.txt
 */

unsigned char decoded[8];
int main(int argc, char *argv[]){
	//The implementation of your decoder should go here.
	
	//It is recommeded that you implement the bit abstractions in bitsy.c and
	//utilize them to implement your decoder.
	//If so, do NOT call read/write here. Instead rely exclusively on 
	//readBit, readByte, writeBit, writeByte, and flushWriteBuffer.
	
	unsigned short bit;
	int flag = 0;
	
	bit = readBit();//from input
	flag = 1; 
	if(bit == 268){
		return 268;
	}

	while(bit != 268){ //how to determine EOF? bit can be 0 
		if(flag == 0){
			bit = readBit();//from input
			if(bit == 268){
				return 268;
			}
		}

		flag = 0;
		int value = 0;

		if(bit == 1){ // first bit is 1
			value = readByte();
			if(value == 268){
				return 268;	
			}
			writeByte(value);
			pushBuffer(value);		

		} else { // first bit is 0
			unsigned char bit1 = readBit();
			if(bit1 == 0){ // reapeat bit is 0
				int position = 0;
				position = read3Bit();
				writeByte(decoded[position]);
				pushBuffer(decoded[position]);

			} else { //Repeat Bit is 1
				int position = 0;
				int count = 0;

				position = read3Bit();
				count = read3Bit();

				char currentChar = decoded[position];
				writeByte(currentChar);
				pushBuffer(currentChar);

				while(count >= 0){ //write for each count(# of matches)
					writeByte(currentChar);
					pushBuffer(currentChar);
					count = count - 1;
				}		
			}	
		}
	}
	return 0; //exit status. success=0, error=-1
}


void pushBuffer(unsigned char c){
	for(int j = 7; j >= 0; j = j - 1){ //Shift decoded
		decoded[j] = decoded[j-1];
	}
	decoded[0] = c; //update decoded	
}
