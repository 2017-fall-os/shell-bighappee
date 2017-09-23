/*******************************************************
Christopher K. Tarango
9/11/17
CS4375
Tokenizer Lab

test.c
*******************************************************/

#include <stdio.h>                                 // This include is needed for input/output
#include <unistd.h>                                // This include is needed for read/write/fork system calls
#include <sys/types.h>		                   // This include is needed for read system call
#include <sys/stat.h>	                           // This include is needed for read system call
#include <fcntl.h>                                 // This include is needed for read system call
#include <assert.h>                                // This include is needed for assert macro
#include <stdlib.h>                                // This include is needed for malloc/calloc/free
#include <sys/wait.h>                              // This include is needed for wait system call
#include "mytoc.h"                                 

#define BUFLEN 8192                                // Buffer length for reader

int main(int argc, char **argv){

  int numBytesRead, counter, printCounter, tokenCount, exitValue = 0, bufferLen= BUFLEN, childID;
  char readBuf[BUFLEN], *ptrReadBuf, *endBuf, **myVector, delimiter = 32;
  ptrReadBuf= readBuf;

  if(argc == 1){                                                 //makes sure no arguments were given when starting program
    assert(75==write(1,"\nThis program will read from input, simply type exit to leave the program\n\n", 75)); 
    while (exitValue == 0){                                      //loop through program till exit condition given
      assert(2==write(1,"$ ",2));                                //print the prompt
      assert(numBytesRead = read(0, ptrReadBuf, bufferLen));{    // read into the buffer
	//ptrReadBuf = readBuf;
	if (numBytesRead == 5){                                  //check for exit string
	  exitValue = checkExit(ptrReadBuf);
	}
	if (exitValue ==0){
	  tokenCount = tokenCounter(ptrReadBuf, delimiter);
	  myVector = mytoc(ptrReadBuf, delimiter);               // call to the mytoc function 
	  //printf("Number of Tokens Read : %d\n", tokenCount);    // print number of tokens read
	  //printf("Address of Vector is %p\n", &myVector);        // print address
	  /* for (counter =0; counter < tokenCount; counter++){
	    printCounter =0;                                     //loop through vector to print tokens
	    printf("Token at index: %d\t", counter);
	    while (myVector[counter][printCounter] !=0){
	      printf("%c", myVector[counter][printCounter]);
	      printCounter++;
	    }
	    printf("%d\n", myVector[counter][printCounter]);     //print the 0 that signifies the end of token
	  }
	  */
	  int rc = fork();
	  if (rc < 0){
	    fprintf(stderr, "fork failed\n");
	    exit(1);
	  }
	  else if(rc==0){
	    childID = (int) getpid();
	    execvp(myVector[0], myVector);
	  }
	  else{
	    assert(childID == wait(NULL));	    
	  }
	  bufferLen = bufferLen - numBytesRead;                  // adjust the read buffer so tokenizer doesn't read past inputs
	  ptrReadBuf += numBytesRead;
	  free(myVector);                                        // free the allocated vector
	}
      }
     
    }
  }
  else{
    assert(42==write(2,"An illegal number of arguments were given\n", 42));
    return -1;
  }
  assert(1==write(1,"\n", 1));                                    // added space for readability
  return 0;
}

/*Function that checks if "exit" has been entered at the prompt so program can be ended
  returns 1 if exit was entered and 0 otherwise */

int checkExit(char *thebuf){
  char *thePointer = thebuf;
  if (*thePointer == 'e'){
    thePointer++;
    if (*thePointer == 'x'){
      thePointer++;
      if(*thePointer == 'i'){
        thePointer++;
        if(*thePointer == 't'){
	  return 1;
        }
      }
    }
  }
  return 0;
}
