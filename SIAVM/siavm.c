#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

const int MEM_MAX = 1024;
//if one were to increase memory size simply change this variable and the value
//of the size of mem
unsigned char mem[1024];
int reg[16]; //array of registers for emulation 32 bit entries
int PC=0; //program counter 32 bit integer
int currentOp;
int op1; //op1 register 32 bits
int op2; //op2 register 32 bits
int result; //result register

unsigned char currentInstruction[4]; //current instruction buffer 32 bits

const int halt=0, add=1, and=2, divide=3, multiply=4, subtract=5, or=6, stack=7,
interrupt=8, addImmediate=9, branchIfEqual=10, branchIfLess=11, jump=12, call=13,
load=14, store=15;

/**
 * prints out contents of all memory in hex format, 2 byte blocks
 */
void printMemHex()
{
	int index=0;
	printf("INTERRUPT 1: PRINT MEMORY");
	for(int j=0;j<MEM_MAX;j++){
			if(index%16==0){printf("\n");}
			printf("%02x", mem[j]);
			if(j%2!=0){printf(" ");}
			index++;
	}
	printf("\n");
}

/**
 * prints value stored in each register
 */
void printReg()
{
	printf("INTERRUPT 0: PRINT REGISTERS\n");
	for(int j=0;j<16;j++){
		printf("Reg[%i]: ", j);
		printf("%02x \n",reg[j]);
	}
}


/**
 * fetch operation, determines upcoming instructions operation type and prepares the
 * instruction for execution
 */
void fetch()
{
 currentInstruction[0]=mem[PC];
 currentOp= (int)currentInstruction[0];
 currentOp = currentOp/16;

 	//3R FETCH
 	if(currentOp==add||currentOp==and||currentOp==divide||currentOp==halt||
 			currentOp==multiply||currentOp==or||currentOp==subtract)
 	{
 		currentInstruction[1]=mem[PC+1];
 		PC=PC+2;
 	}

 	//AI FETCH
 	if(currentOp==addImmediate)
 	{
 		currentInstruction[1]=mem[PC+1];
 		PC=PC+2;
 	}

 	//BR FETCH
 	if(currentOp==branchIfLess||currentOp==branchIfEqual)
 	{
 			currentInstruction[1]=mem[PC+1];
 			currentInstruction[2]=mem[PC+2];
 			currentInstruction[3]=mem[PC+3];
 			PC=PC+4;
 	}

 	//INT FETCH
 	if(currentOp==interrupt)
 	{
 		currentInstruction[1]=mem[PC+1];
 		PC=PC+2;
 	}

 	//JMP FETCH
 	if(currentOp==jump||currentOp==call)
 	{
 		currentInstruction[1]=mem[PC+1];
 		currentInstruction[2]=mem[PC+2];
 		currentInstruction[3]=mem[PC+3];
 		PC=PC+4;
 	}

 	//LS FETCH
 	if(currentOp==load||currentOp==store)
 	{
 		currentInstruction[1]=mem[PC+1];
 		PC=PC+2;
 	}

 	//STACK FETCH
 	if (currentOp==stack)
 	{
 		currentInstruction[1]=mem[PC+1];
 		PC=PC+2;
 	}

}

/**
 * breaks down current instruction and sets up for execution
 */
void decode()
{
	//3R DECODE
	if(currentOp==add||currentOp==and||currentOp==divide||currentOp==halt||
	 			currentOp==multiply||currentOp==or||currentOp==subtract) //3R TYPE
	{
		op1= (int)currentInstruction[0];
		op1= op1%16;
		op2= (int)currentInstruction[1];
		/**
		 * note that target register is op2%16
		 * note that 2nd register is op2>>4        FOR 3R
		 */
	}

	//AI DECODE
	if(currentOp==addImmediate)//AI TYPE
	{
		op1= (int)currentInstruction[0];
		op1 = op1%16;
		op2= (int)currentInstruction[1];
		//Acquires intended integer value, if larger than 127, flips the sign (2's comp)
		if(op2>127){op2=(op2-256);}
	}

	//INT DECODE
	if(currentOp==interrupt)
	{
		op1=(int)currentInstruction[1];
	}

	//JMP DECODE
	if(currentOp==jump||currentOp==call)
	{
		//store 12 top bits in op 1
		op1=(int)currentInstruction[0];
		op1=op1%16;
		op1=op1<<8;
		op1|=(int)currentInstruction[1];

		//store bottom 16 bits in op2
		op2=(int)currentInstruction[2];
		op2=op2<<8;
		op2|=(int)currentInstruction[3];
	}

	//BR DECODE
	if(currentOp==branchIfLess||currentOp==branchIfEqual)
	{
		//find which registers are needed in op1
		op1 = (int)currentInstruction[0]%16;
		op1 = op1<<4;
		op1 |=(int)currentInstruction[1]/16;

		//store the 20 bits of the address in op2
		if((int)currentInstruction[1]%16>=8)
		{
			op2= -1048576;
			op2 |= (((int)currentInstruction[1]%16)<<16);
			op2 |= ((int)currentInstruction[2])<<8;
			op2 |= (int)currentInstruction[3];


		}
		else
		{
			op2 = (int)currentInstruction[1]%16;
			op2 = op2<<8;
			op2 |= (int)currentInstruction[2];
			op2 = op2<<8;
			op2 |= (int)currentInstruction[3];
		}

	}

	//STACK DECODE
	if (currentOp==stack)
	{	//store the target register of the stack operation
		op1=(int)currentInstruction[0];
		op1=op1%16;
		//store the 00 01 or 10 of the pop instruction
		op2=(int)currentInstruction[1];
		op2=op2>>6;
	}

	//LS DECODE
	if(currentOp==load||currentOp==store)
	{
		op1=(int)currentInstruction[0];
		op1=op1%16;
		op1=op1<<4;

		op1|=((int)currentInstruction[1]/16);
		op2 = ((int)currentInstruction[1]%16);

	}

}

/**
 * executes given instruction
 */
void execute()
{

		if (currentOp==halt)
		{
			return;
		}

		if (currentOp==add)
    	{
			result = (int)reg[op1] + (int)reg[op2>>4];
			//reg[op2%16]=result;
			//printf("3R Result: %i\n" , reg[op2%16]);
    	}

		if (currentOp==and)
    	{
			result = (int)reg[op1] & (int)reg[op2>>4];
			//reg[op2%16]=result;
			//printf("3R Result: %i\n" , reg[op2%16]);
    	}

		if (currentOp==divide)
    	{
			result = (int)reg[op1] / (int)reg[op2>>4];
			//reg[op2%16]=result;
			//printf("3R Result: %i\n" , reg[op2%16]);
    	}

		if (currentOp==multiply)
    	{
			result = (int)reg[op1] * (int)reg[op2>>4];
			//reg[op2%16]=result;
			//printf("3R Result: %i\n" , reg[op2%16]);
    	}

		if (currentOp==subtract)
    	{
			result = (int)reg[op1] - (int)reg[op2>>4];
			//reg[op2%16]=result;
			//printf("3R Result: %i\n" , reg[op2%16]);
    	}

		if (currentOp==or)
    	{
			result = (int)reg[op1] | (int)reg[op2>>4];
			//reg[op2%16]=result;
			//printf("3R Result: %i\n" , reg[op2%16]);
    	}

		if (currentOp==stack)
    	{
			if(op2==0) //stk,return
			{
				result = 0;
				for(int j=0; j<4;j++)
				{
				result|=mem[(int)reg[15]+j];
				if(j<3){result=result<<8;}
				}
				reg[15]=(int)reg[15]+4;
			}
			if(op2==1)//stk,push
			{
				reg[15]=(int)reg[15]-4; //r15 is reserved as stack pointer
				result=(int)reg[op1]; //store value in the register to result register
			}
			if(op2==2)//stk,pop
			{
				result = 0;
				for(int j=0; j<4;j++)
				{
				result|=mem[(int)reg[15]+j];
				if(j<3){result=result<<8;}
				}
				reg[15]=(int)reg[15]+4;
			}
    	}

		if (currentOp==interrupt)
    	{
			if(op1==0){printReg();}
			else if (op1==1){printMemHex();}
			else {printf("UNRECOGNIZED INTERRUPT\n");}
    	}

		if (currentOp==addImmediate)
    	{

			result = (int)reg[op1]+ op2;
			//reg[op1]=result;
			//printf("result: %i\n", reg[op1]);
    	}

		if (currentOp==branchIfEqual)
    	{
			if((int)reg[op1/16]==(int)reg[op1%16])
			{

				result=op2;
				result=result*2;
			}
			else{result=1;}
    	}

		if (currentOp==branchIfLess)
    	{

			if((int)reg[op1/16]<(int)reg[op1%16])
			{

				result=op2;
				result=result*2;
			}
			else{result=1;}
    	}

		if (currentOp==jump)
    	{
			result=op1;
			result=result<<16;
			result|=op2;
			result = result*2;
    	}

		if (currentOp==call)
    	{
			result=op1;
			result=result<<16;
			result|=op2;
			result = result*2;

			reg[15]=(int)reg[15]-4; //move back for push
			int k=0;
			for(int j = 3; j>=0;j--)
			{
				mem[(int)reg[15]+k]=(PC>>(j*8));
				k++;
			}
			k=0;


    	}

		if (currentOp==load)
    	{
			result = 0;
			for(int j=0; j<4;j++)
			{
			result|=mem[((int)reg[(op1%16)]+(2*op2))+j];
			if(j<3){result=result<<8;}
			}
    	}

		if (currentOp==store)
    	{
			result=(int)reg[op1/16];
    	}

}

void save()
{
	//3R operations
	if(currentOp==add||currentOp==and||currentOp==divide||
	 			currentOp==multiply||currentOp==or||currentOp==subtract)
	{
		reg[op2%16]=result;
	}

	//AI operations
	if(currentOp==addImmediate)
	{
		reg[op1]=result;
	}

	//jmp operations
	if (currentOp==jump||currentOp==call)
	{
		PC=result;
	}

	//Stack operations
	if (currentOp==stack)
	{
		if(op2==0) //stk,return
		{
			PC=result;
		}
		if(op2==1)//stk,push
		{
			int k=0;
			for(int j = 3; j>=0;j--)
			{
				mem[(int)reg[15]+k]=(result>>(j*8));
				k++;
			}
			k=0;
		}
		if(op2==2)//stk,pop
		{
			reg[op1]=result;
		}
	}

	//LS operations
	if (currentOp==load)
	{
		reg[op1/16]=result;
	}
	if (currentOp==store)
	{
		int k=0;
		for(int j = 3; j>=0;j--)
		{
			mem[((int)reg[op1%16]+(2*op2))+k]=(result>>(j*8));
			k++;
		}
		k=0;
	}

	//BR OPERATIONS
	if(currentOp==branchIfLess||currentOp==branchIfEqual)
	{
		if(result!=1)
		{
			PC = PC +result;
		}
	}
	//No need for interrupt to store anything

}
/**
 * initiates run loop; takes in assembler output and performs its instructions
 */
int main(int argc, char **argv)
{

	FILE *src = fopen(argv[1],"r");
    char line[2];
		int i, instructionSize=0;

		//Loading in instructions from assembler file
		while (NULL != fgets(line, sizeof(line), src))
		{
		    unsigned char temp;
			for(i=0;i<1;i++){
			temp = line[i];
			mem[instructionSize]=temp;
			instructionSize++;
			}
		}
	fclose(src);

while(PC<1024)
{
   fetch();
   decode();
   execute();
   if(currentOp==halt){printf("HALT");return 0;}
   save();
}
	return 0;
}
