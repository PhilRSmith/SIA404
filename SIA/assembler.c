#include <stdio.h>
#include <string.h>
#include <stdlib.h>

char *ltrim(char *s) {
	while (*s == ' ' || *s == '\t') s++;
	return s;
}


char getRegister(char *text) {
	if (*text == 'r' || *text=='R') text++; //advances to number next to R and converts reg num to integer
	return atoi(text);
}

char getImmediate(char *text) {
	return atoi(text);
}

long getAddress(char *text) {
	return atol(text);
}


int assembleLine(char *text, unsigned char* bytes) {
	text = ltrim(text);
	char *keyWord = strtok(text," ");


	//halt, op0 (requires "junk" at end of text line to work)
	if (strcmp("halt",keyWord)==0) {
		bytes[0]=0x00;
		bytes[1]=0x00;
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
		}

	//add, op1
	if (strcmp("add",keyWord) == 0) {
		bytes[0] = 0x10;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 ;
		bytes[1] |= getRegister(strtok(NULL," "));  //the bytes are all in decimal
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
	}

	//and, op2
	if (strcmp("and",keyWord) == 0) {
		bytes[0] = 0x20;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 ;
		bytes[1] |= getRegister(strtok(NULL," "));  //the bytes are all in decimal
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
	}

	//divide, op3
	if (strcmp("divide",keyWord) == 0) {
		bytes[0] = 0x30;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 ;
		bytes[1] |= getRegister(strtok(NULL," "));  //the bytes are all in decimal
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
		}

	//multiply, op4
	if (strcmp("multiply",keyWord) == 0) {
		bytes[0] = 0x40;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 ;
		bytes[1] |= getRegister(strtok(NULL," "));  //the bytes are all in decimal
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
		}

	//subtract, op5
	if (strcmp("subtract",keyWord) == 0) {
		bytes[0] = 0x50;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 ;
		bytes[1] |= getRegister(strtok(NULL," "));  //the bytes are all in decimal
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
		}

	//or, op6
	if (strcmp("or",keyWord) == 0) {
		bytes[0] = 0x60;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getRegister(strtok(NULL," ")) << 4 ;
		bytes[1] |= getRegister(strtok(NULL," "));  //the bytes are all in decimal
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
		}


	//pop,push,return, op7
	if (strcmp("push", keyWord) ==0){
		bytes[0]= 0x70;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = 0x40;
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
	}

	if (strcmp("pop", keyWord) ==0){
		bytes[0]= 0x70;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = 0x80;
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
		}

	if (strcmp("return", keyWord) ==0){ //requires whitespace after "return" statement
		bytes[0]= 0x70;
		bytes[1] = 0x00;
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
		}

	//interrupt, op8
	if (strcmp("interrupt", keyWord) ==0){
		int interrupt= getImmediate(strtok(NULL," "));
		bytes[0] =((interrupt>>8) | 0x80);
		bytes[1] =((interrupt>>0) & 0xFF);
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
		}



	//addimmediate, op9
	if ((strcmp("addImmediate", keyWord) ==0)||(strcmp("addimmediate", keyWord) ==0)){
		bytes[0] = 0x90;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = getImmediate(strtok(NULL, " "));
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
	}

	//branchifequal, op10
	if ((strcmp("branchifequal", keyWord) ==0)||(strcmp("branchIfEqual", keyWord) ==0)){
		bytes[0] = 0xA0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = (getRegister(strtok(NULL," ")))<<4;
		long brAddress = getAddress(strtok(NULL, " "));
		if(brAddress<0){
			bytes[1] |= 0x0F;
			bytes[1] &= (brAddress>>16);
			bytes[2] = ((brAddress>>8) & 0xFF);
			bytes[3] = ((brAddress>>0) & 0xFF);
		}
		else{
			bytes[1] |= (brAddress>>16);
			bytes[2] = ((brAddress>>8) & 0xFF);
			bytes[3] =((brAddress>>0) & 0xFF);
		}
		printf ("Out: %x" , bytes[0]); printf (":%x" , bytes[1]);
		printf (":%x" , bytes[2]); printf (":%x\n" , bytes[3]);
		printf("\n");
		return 4;
	}

	//branchifless, op11
	if ((strcmp("branchifless", keyWord) ==0)||(strcmp("branchIfLess", keyWord) ==0)) {
		bytes[0] = 0xB0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = (getRegister(strtok(NULL," ")))<<4;
		long brAddress = getAddress(strtok(NULL, " "));
		if(brAddress<0){
			bytes[1] |= 0x0F;
			bytes[1] &= (brAddress>>16);
			bytes[2] = ((brAddress>>8) & 0xFF);
			bytes[3] = ((brAddress>>0) & 0xFF);
		}
		else{
			bytes[1] |= (brAddress>>16);
			bytes[2] = ((brAddress>>8) & 0xFF);
			bytes[3] =((brAddress>>0) & 0xFF);
		}
		printf ("Out: %x" , bytes[0]); printf (":%x" , bytes[1]);
		printf (":%x" , bytes[2]); printf (":%x\n" , bytes[3]);
		printf("\n");
		return 4;
	}

	//jump, op12  note: Uses Shift and Mask
	if (strcmp("jump",keyWord) == 0) {
		//max long value this works for is 268435455
		long address = getAddress(strtok(NULL, " "));
		bytes[0]=((address>>24) | 0xC0);
		bytes[1]=((address>>16) & 0xFF);
		bytes[2]=((address>>8) & 0xFF);
		bytes[3]=((address>>0) & 0xFF);
		printf ("Out: %x" , bytes[0]); printf (":%x" , bytes[1]);
		printf (":%x" , bytes[2]); printf (":%x\n" , bytes[3]);
		printf("\n");
		return 4;
		}

	//call, op13
	if (strcmp("call",keyWord) == 0) {
		//max long value this works for is 268435455
		long address = getAddress(strtok(NULL, " "));
		bytes[0]=((address>>24) | 0xD0);
		bytes[1]=((address>>16) & 0xFF);
		bytes[2]=((address>>8) & 0xFF);
		bytes[3]=((address>>0) & 0xFF);
		printf ("Out: %x" , bytes[0]); printf (":%x" , bytes[1]);
		printf (":%x" , bytes[2]); printf (":%x\n" , bytes[3]);
		printf("\n");
		return 4;
		}

	//load, op14
	if (strcmp("load", keyWord) ==0) {
		bytes[0] = 0xE0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = (getRegister(strtok(NULL," ")))<<4;
		int offset = getImmediate(strtok(NULL," ")); //offset should never be greater than 15
		bytes[1] |= offset;
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
	}

	//store, op15
	if (strcmp("store", keyWord) ==0) {
		bytes[0] = 0xF0;
		bytes[0] |= getRegister(strtok(NULL," "));
		bytes[1] = (getRegister(strtok(NULL," ")))<<4;
		int offset = getImmediate(strtok(NULL," ")); //offset should never be greater than 15
		bytes[1] |= offset;
		printf ("Out: %x" , bytes[0]); printf ("%x\n" , bytes[1]);
		printf("\n");
		return 2;
	}

}

int main(int argc, char **argv) {
	FILE *src = fopen(argv[1],"r");
	FILE *dst = fopen(argv[2],"wb");
	while (!feof(src)) {
		unsigned char bytes[4];
		char line[1000];
		if (NULL != fgets(line, sizeof(line), src)) {
			printf ("read: %s",line);
			int byteCount = assembleLine(line,bytes);
			fwrite(bytes,byteCount,1,dst);
		}
	}
	fclose(src);
	fclose(dst);
	return 0;
}
