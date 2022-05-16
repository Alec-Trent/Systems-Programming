#include "bitsy.h" //This header includes prototypes for the proposed bit abstractions
#include <stdio.h>
/*Include any additional headers you require*/

/*You may use any global variables/structures that you like*/

/* main - czy compression implementation
 * Develop a program called czy which compresses the data stream directed 
 * at its standard input and writes the compressed stream to its standard 
 * output.
 *
 * The compression algorithm reads the input one symbol (i.e., byte) at a 
 * time and compares it with each of the 8 bytes previously seen.  It also 
 * checks to see if the following n characters are the same as the current 
 * symbol.  If the byte has been previously seen, it outputs the position of 
 * the previous byte relative to the current position as well as the number 
 * of times it occurs using the format described below. Otherwise, the symbol 
 * is output as is by prefixing it with a binary one.
 *
 * To compile czy: make czy
 * To execute: ./czy < somefile.txt > somefile.encoded
 */
int main(int argc, char *argv[]){
	//The implementation of your encoder should go here.
	
	//It is recommeded that you implement the bit abstractions in bitsy.c and
	//utilize them to implement your encoder. 
	//If so, do NOT call read/write here. Instead rely exclusively on 
	//readBit, readByte, writeBit, writeByte, and flushWriteBuffer.

	unsigned short recentBuffer[8];
	unsigned short byte; //was char
	unsigned short nextByte;
	int read = 0;

	 byte = readByte();

         if(byte == 268){ // check EOF
         	return 0;
         } else { //else the first iteration is infrequent
                writeBit(1); // <1>
                writeByte(byte);// <symbol>
		recentBuffer[0] = byte;
	 }

	while(byte != 268){ //eof value
		int position = 0;
		if(read == 0){ //check to read if recent non repeating occured
			byte = readByte();
			if(byte == 268){ // check EOF
         			flushWriteBuffer();
				return 0;
         		} 
		}

		read = 0; //reset flag
		//Check for recent
		int recent = 0;
		for(int i = 7; i >= 0; i = i - 1){ //compare byte to recentBuffer for recent match
			if(byte == recentBuffer[i]){ //we have at least one recent
				position = i;
				recent = 1;                        
                        }
                }
		
		if(recent == 0){ //infrequent
			writeBit(1); // <1>
                	writeByte(byte);// <symbol>
			for(int j = 7; j >= 0; j = j - 1){ //Shift recentBuffer
				recentBuffer[j] = recentBuffer[j-1];
			}
			recentBuffer[0] = byte; //update recentBuffer

		} else { //recent
			nextByte = readByte();
			if(nextByte == byte){ //check for matches
				int matches = -1; //was -1
				while(nextByte == byte && matches < 7){ // find number of consecutive matches WAS 7
					nextByte = readByte();
					matches = matches + 1;
				}
				
				writeBit(0); // <0>
				writeBit(1); // <1>
				for(int k = 2; k >= 0; k = k - 1){ //write pos WRONG CURRENTLY
					writeBit((position >> k) & 0x1);
				}
				
				for(int l = 2; l >= 0; l = l - 1){ //write count
					writeBit((matches >> l) & 0x1);
				}

				while(matches >= -1){
					for(int j = 7; j >= 0; j = j - 1){ //Shift recentBuffer
						recentBuffer[j] = recentBuffer[j-1];
					}
					recentBuffer[0] = byte; //update recentBuffer

					matches = matches - 1;
				}

				read = 1; //set the flag not to lose read call
				byte = nextByte; //update byte for next run

			} else { //Recent non repeating
				
				writeBit(0); // <0>
				writeBit(0); // <0>
				for(int m = 2; m >= 0; m = m - 1){ //write pos
					writeBit((position >> m) & 0x1);
				}

				for(int j = 7; j >= 0; j = j - 1){ //Shift recentBuffer
					recentBuffer[j] = recentBuffer[j-1];
				}
				recentBuffer[0] = byte; //update recentBuffer
				read = 1; //set the flag not to lose read call
				byte = nextByte; //update byte for next run
			}
		}
	}
	flushWriteBuffer();
	return 0; //exit status. success=0, error=-1
}
