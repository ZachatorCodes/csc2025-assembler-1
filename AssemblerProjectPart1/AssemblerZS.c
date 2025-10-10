// name: Zach Schwartz
// project title: Assembler Project Part 2
// date: 09/08/2025
// description: Code for up to part 2 of the assembler project.

#define _CRT_SECURE_NO_WARNINGS  // lets us use deprecated code

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char ASM_FILE_NAME[] = "AssemblerPart1ZS.asm";

#define MAX 150			// strlen of simulators memory can be changed
#define COL 7			// number of columns for output
#define LINE_SIZE 20	// For c-strings

//OPERAND TYPES, REGISTERS AND OTHER
#define AXREG 0
#define BXREG 1
#define CXREG 2
#define DXREG 3
#define CONSTANT 7
#define ADDRESS 6

//commands
#define HALT 5
#define MOVREG 192
#define MOVMEM 224
#define ADD 160
#define PUT 7

//boolean
#define TRUE 1
#define FALSE 0

enum operType { reg, mem, constant, arrayBx, arrayBxPlus, none };  //list of all types of operand types

//Registers and flag add the stack pointer
struct Registers
{
	int AX;
	int BX;
	int CX;
	int DX;
	int flag;
}regis;

//GLOBAL VARIABLES
typedef short int Memory;  //sets the type of memory to short int 
Memory memory[MAX] = { 0 };   //global variable the memory of the virtual machine
Memory address;     //global variable the current address in the virtual machine

//function prototypes
void runMachineCode();	// Executes the machine code
void splitCommand(char line[], char part1[], char part2[], char part3[]);	// splits line of asm into it's three parts
void convertToMachineCode(FILE* fin);	// Converts a single line of ASM to machine code
void assembler();			// Converts the entire ASM file and stores it in memory
void printMemoryDump();	// Prints memeory with commands represented as integes

// Helper functions prototypes 
int convertToNumber(char line[], int start);	// converts a sub-string to an int
int whichOperand(char operand[]);			// Returns the number of the letter registar
void changeToLowerCase(char line[]);	// Changes each character to lower case
void printMemoryDumpHex();				// Prints memory in hexedecimal
void putValue(int operand, int value);
Memory getValue(Memory operand);

// Function to take in an operand, and return one of the memory registers.
Memory getValue(Memory operand)
{
	int value;
	switch (operand)
	{
	case AXREG:
		return regis.AX;
		break;
	case BXREG:
		return regis.BX;
		break;
	case CXREG:
		return regis.CX;
		break;
	case DXREG:
		return regis.DX;
		break;
	case CONSTANT:
		value = memory[address];
		address++;
		return value;
	}
}

// Function to take in an operand and a value, and store the value in the correct register based on the operand.
void putValue(int operand, int value)
{
	if (operand == AXREG)
	{
		regis.AX = value;
	}
	else if (operand == BXREG)
	{
		regis.BX = value;
	}
	else if (operand == CXREG) {
		regis.CX = value;
	}
	else if (operand == DXREG) {
		regis.DX = value;
	}
	else {
		printf("Error, invalid operand");
	}
}

int main()
{
	//printMemoryDump( );  //displays the starting memory, remove once code works
	assembler();
	runMachineCode();
	/*remove one memory dump, once you decide if you want to see
	   the results in hex or decimal */
	printMemoryDumpHex();  //displays memory with final values in hex 
	printMemoryDump();  //displays memory with final values

	printf("\n");
	system("pause");
	return 0;
}

/********************   assembler   ***********************
changes the assembly code to machine code and places the
commands into the memory.
parameters: none
return value: none
-----------------------------------------------------------*/
void assembler()
{
	address = 0;
	FILE* fin;		// File pointer for reading in the assembly code.
	//recommend changeing so you can type in file name
	fopen_s(&fin, ASM_FILE_NAME, "r");
	if (fin == NULL)
	{
		printf("Error, file didn't open\n\nExiting program...\n\n");
		system("pause");
		exit(1);
	}
	for (int i = 0; i < MAX && !feof(fin); i++)
	{
		convertToMachineCode(fin);
	}
}

/********************   convertToMachineCode   ***********************
Converts a single line of ASM to machine code

Needs work, comment must be corrected
---------------------------------------------------------------------*/
void convertToMachineCode(FILE* fin)
{
	char line[LINE_SIZE];		// full command
	char part1[LINE_SIZE];	// the asm commmand
	char part2[LINE_SIZE] = "";// the two operands, could be empty
	char part3[LINE_SIZE] = "";
	int machineCode = 0;	// One line of converted asm code from the file
	Memory operand3 = 0;  // the second operand, could be empty

	fgets(line, LINE_SIZE, fin);		// Takes one line from the asm file
	changeToLowerCase(line);

	splitCommand(line, part1, part2, part3);

	if (part1[0] == 'h')  //halt
	{
		memory[address] = HALT;
		address++;
	}
	else if (part1[0] == 'm')  //move into a register
	{
		machineCode = MOVREG;
		machineCode = machineCode | (whichOperand(part2) << 3); // bitshifts 3 to the left
		operand3 = whichOperand(part3);
		machineCode = machineCode | operand3;
		memory[address] = machineCode;
		address++;
		//put the command into the first 3 bits of machineCode
		//put the first operand (register) into the next 2 bits (use bitshift)
		//put the second operand into the last 3 bits 
	}
	else if (part1[0] == 'a')  //move into a register
	{
		machineCode = ADD;
		machineCode = machineCode | (whichOperand(part2) << 3); // bitshifts 3 to the left
		operand3 = whichOperand(part3);
		machineCode = machineCode | operand3;
		memory[address] = machineCode;
		address++;
		//put the command into the first 3 bits of machineCode
		//put the first operand (register) into the next 2 bits (use bitshift)
		//put the second operand into the last 3 bits 
	}
	else if (part1[0] == 'p') {
		printf("Output: %d\n", getValue(AXREG));
	}
	if (operand3 == CONSTANT) // if the second operand is a constant
	{
		memory[address] = convertToNumber(part3, 0); // puts the constant value into the next memory address
		address++;
	}
	//output memory, for debugging, comment out when you don't need it. could use printMemoryDumpHex
	printf("\n");
	printMemoryDump();
}


/********************   splitCommand   ***********************
splits a line of asm into it's parts

Takes in a line of assembly code as a character array and splits it into three seperate arrays
part1 - the command
part2 - the first operand
part3 - the second operand

returns nothing
-----------------------------------------------------------*/
void splitCommand(char line[], char part1[], char part2[], char part3[])
{
	int index = 0;           //the character location in the string line
	int index2 = 0;          //character location in new string ie the parts

	//moves the first set of characters from line into instruction
	while (line[index] != ' ' && line[index] != '\0' && line[index] != '\n')
	{
		part1[index2] = line[index];
		index++;
		index2++;
	}
	part1[index2] = '\0';				// add the string stopper

	if (line[index] == '\0')  //no space, command has no other parts
	{
		strcpy(part2, "\0");
		strcpy(part3, "\0");
	}
	else
	{
		printf("\nIndex is: %d", index);  //debugging remove when 

		//checks to make sure it is logical to proceed.  
		//once code is working this code should never be reached.
		if (index < 1 || index > 3)
		{
			printf("\a\a\tnumber not in the range\n");
			system("pause");  //stops the code from running until enter is pushed
			exit(1);	// This is temporary. You must find a way to deal with index out of bounds.
		}

		//What needs to be done.
		//copy the characters from line into the two parts, part2 and part3
		//part2 = "to be done"; //note write one function that works for both
		//part3 = "to be done";
		//put the code here

		index2 = 0; // resets the index to 0 for the next part
		index++;  //skips the space
		// while the next character is not a space or the end of the string (stopper or new line),
		// copy the character from line(index) to part2(index) then increment both indexes.
		// add string stopper when done
		while (line[index] != ' ' && line[index] != '\0' && line[index] != '\n')
		{
			part2[index2] = line[index];
			index++;
			index2++;
		}
		part2[index2] = '\0'; // add the string stopper

		// repeat the process for part3
		index++;
		index2 = 0;
		while (line[index] != ' ' && line[index] != '\0' && line[index] != '\n')
		{
			part3[index2] = line[index];
			index++;
			index2++;
		}
		part3[index2] = '\0'; // add the string stopper

		//these are hard coded temporary values this needs to be deleted when the split is working
		//strcpy( part2, "cx" );  //temporary values so there is something to see
		//strcpy( part3, "654" );
	}
	//for debugging, comment out when you don't need it
	printf("\nCommand = %s %s %s", part1, part2, part3);
}

/********************   runMachineCode   ***********************
Executes the machine code that is in memory, the virtual machine

Needs to be written
-----------------------------------------------------------*/
void runMachineCode()
{
	Memory mask1 = 224;   //111 00 000
	Memory mask2 = 24;    //000 11 000
	Memory mask3 = 7;	  //000 00 111
	Memory part1, part2, part3; //command, operand1, 
	int value1, value2;   //the actual values in the registers or constants

	address = 0;
	Memory fullCommand = memory[address];
	address++;
	while (fullCommand != HALT)
	{
		part1 = fullCommand & mask1;
		part2 = (fullCommand & mask2) >> 3;
		part3 = fullCommand & mask3;
		if (part1 == MOVREG)
		{
			//get the value from part3
			value2 = getValue(part3);
			//put the value into the register specified by part2
			putValue(part2, value2);
		}
		else if (part1 == ADD) {
			// get the values from part2 and part3
			value1 = getValue(part2);
			value2 = getValue(part3);
			// add the values together
			value1 = value1 + value2;
			// put the value into the register specified by part2
			putValue(part2, value1);
		}
		fullCommand = memory[address];  //the next command
		address++;
		//debugging, comment out when you don't need it
		printMemoryDump();
	}
}

/*********************************************************************************
/****************************   HELPER FUNCTIONS   *******************************
/*********************************************************************************

/****************************   printMemoryDump   ********************************
prints memory by number
MAX is the amount of elements in the memory array (Vicki used 100)
COL is the number of columns that are to be displayed (Vicki used 7)
parameters: none
return value: none
---------------------------------------------------------------------------------*/
void printMemoryDump()
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	for (int row = 0; row < numRows; row++)
	{
		location = row;
		for (int column = 0; location < MAX && column < COL; column++)
		{
			if (!(numRows - 1 == row && carryOver - 1 < column))
			{
				printf("%5d.%5d", location, memory[location]);
				location += (numRows - (carryOver - 1 < column));
			}
		}
		printf("\n");
	}
	printf("\nAX:%d\t", regis.AX);
	printf("BX:%d\t", regis.BX);
	printf("CX:%d\t", regis.CX);
	printf("DX:%d\n\n", regis.DX);
	printf("Address: %d\n", address);
	printf("Flag: %d\n\n", regis.flag);
}


/*********************   printMemoryDumpHex   ********************
Prints memory in
*parameters: none
*return value: none
----------------------------------------------------------------*/
void printMemoryDumpHex()
{
	int numRows = MAX / COL + 1;	//number of rows that will print
	int carryOver = MAX % COL;		//number of columns on the bottom row
	int location;   //the current location being called
	for (int row = 0; row < numRows; row++)
	{
		location = row;
		for (int column = 0; location < MAX && column < COL; column++)
		{
			if (!(numRows - 1 == row && carryOver - 1 < column))
			{
				printf("%5d.%3x", location, memory[location]);
				location += (numRows - (carryOver - 1 < column));
			}
		}
		printf("\n");
	}
	printf("\nAX:%d\t", regis.AX);
	printf("BX:%d\t", regis.BX);
	printf("CX:%d\t", regis.CX);
	printf("DX:%d\n", regis.DX);
	printf("Address: %d\n", address);
	printf("Flag: %d\n\n", regis.flag);
}

/*****************************************************************************/
/*********** helper function for converting to machine code ******************/
/*****************************************************************************/

/*********************   whichOperand   ***************************
/* changes the letter of the register to a number parameters:
/* letter - the first letter of the operand, register, number, [
/* return value - the number of the register
/*--------------------------------------------------------------*/
int whichOperand(char operand[LINE_SIZE])
{
	char letter = operand[0];
	if (letter == 'a')
	{
		return AXREG;
	}
	else if (letter == 'b')
	{
		return BXREG;
	}
	else if (letter == 'c')
	{
		return CXREG;
	}
	else if (letter == 'd')
	{
		return DXREG;
	}
	else if (isdigit(letter))
	{
		return CONSTANT;
	}
	return -1;  //something went wrong if -1 is returned
}

/*********************ConvertToNumber ********************/
/*  takes in a line and converts digits to a integer
/*  line - is the string of assembly code to convert
/*  start - is the location where the line is being converted,
/*--------------------------------------------------------------*/
int convertToNumber(char line[], int start)
{
	int value; // is the integer value of the digits in the code
	char number[16];  //just the digits
	int negative = 0;  //negative or positive number

	int i = 0;
	while (line[start] == '[' || line[start] == ' ')
	{
		start++;
	}
	if (line[start] == '-')
	{
		start++;
		negative = 1;
	}
	while (i < 16 && isdigit(line[start]))
	{
		number[i] = line[start];
		i++;
		start++;
	}
	number[i] = '\0';
	value = atoi(number);
	if (negative == 1)
	{
		value = -value;
	}
	return value;
}  //end convertToNumber

/*********************   changeToLowerCase   ********************
Changes each character to lower case
*parameters:
* line - the string that was entered the line is completely changed to lower case
* return value: none
----------------------------------------------------------------*/
void changeToLowerCase(char line[])
{
	int index = 0;
	while (index < strlen(line))
	{
		line[index] = tolower(line[index]);
		index++;
	}
}

// List Of Problems (Head Bangers) - Anything that takes more than a few minutes to figure out
