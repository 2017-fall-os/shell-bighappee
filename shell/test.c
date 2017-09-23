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

int main(int argc, char **argv, char **envp){

  int numBytesRead, counter, printCounter, tokenCount, exitValue = 0, bufferLen= BUFLEN, childID;
  char readBuf[BUFLEN], *ptrReadBuf, *endBuf, **myVector, exitString[]="exit", pathString[]="PATH",
    **enVector, **pathVector;
  ptrReadBuf= readBuf;

  if(argc == 1){                                                 // makes sure no arguments were given when starting program
    for(counter=0; envp[counter] != (char *)0; counter++){
      enVector = mytoc(envp[counter], '=');
      if(strComp(enVector[0], pathString) ==1){
	printf("path found\n");
	pathVector = mytoc(enVector[1],':');
	}
      }
    }
    while (exitValue == 0){                                      // loop through program till exit condition given
      assert(2==write(1,"$ ",2));                                // print the prompt
      assert(numBytesRead = read(0, ptrReadBuf, bufferLen));{    // read into the buffer
	tokenCount = tokenCounter(ptrReadBuf, ' ');
	myVector = mytoc(ptrReadBuf, ' ');                       // call to the mytoc function 
	if (tokenCount == 1){                                    // check for exit strin
	  exitValue = strComp(exitString, myVector[0]);
	}
	if (tokenCount >0){
	  if (exitValue ==0){
	    printf("Token Count : %d\n",tokenCount);
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
	      int waitID = wait(NULL);
	      printf("I am the child %d\n", childID);
	      printf("We waited, %d\n", waitID);
	    }
	    clearBuffer(ptrReadBuf, numBytesRead);                 // clear buffer for next read
	    free(myVector);                                        // free the allocated vector
	  }
	}
      }
    }
  }
  else{
    assert(42==write(2,"An illegal number of arguments were given\n", 42));
    return -1;
  }
  return 0;
}
