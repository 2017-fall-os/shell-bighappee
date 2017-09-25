/*******************************************************
Christopher K. Tarango
9/24/17
CS4375
Shell Lab (Part 1)

shell.c
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

  int numBytesRead, counter, tokenCount, exitValue = 0, bufferLen= BUFLEN, childID, retVal, status;
  char readBuf[BUFLEN], *ptrReadBuf, **myVector, exitString[]="exit", pathString[]="PATH",
    **enVector, **pathVector,slashString[]="/", *commandString;
  ptrReadBuf= readBuf;

  if(argc == 1){                                                 // makes sure no arguments were given when starting program
    for(counter=0; envp[counter] != (char *)0; counter++){
      enVector = mytoc(envp[counter], '=');                      // tokenize environment 
      if(strComp(enVector[0], pathString) ==1){                  // find the path variable
	pathVector = mytoc(enVector[1],':');                     // tokenize path variable
      }
    }
    while (exitValue == 0){                                      // loop through program till exit condition given
      assert(2==write(1,"$ ",2));                                // print the prompt
        numBytesRead = read(0, ptrReadBuf, bufferLen);           // read into the buffer
	//	printf("Bytes Read = %d\n", numBytesRead);
	if (numBytesRead==0|| *ptrReadBuf == EOF){
	  exitValue =1;
	}
	else{
	  tokenCount = tokenCounter(ptrReadBuf, ' ');
	  myVector = mytoc(ptrReadBuf, ' ');                       // call to the mytoc function 
	  if (tokenCount == 1){                                    // check for exit string
	    exitValue = strComp(exitString, myVector[0]);
	  }
	}
	if (tokenCount > 0 && exitValue == 0){                   // empty string or exit value bypasses code block

	  int rc = fork();                                       // attempt to fork
	  if (rc < 0){                                           // fork failed
	    fprintf(stderr, "fork failed\n");
	    exit(1);
	  }
	  else if(rc==0){                                        // fork success
	    childID =  (int) getpid();
	    retVal = execve(myVector[0], &myVector[0], envp);    // attempt execve full path to program given
	      if(retVal!=0){                                                   
		counter =0;                                           
		while (pathVector[counter] != 0){                                  // append the command to all possible paths
		  commandString = strConcat(pathVector[counter], slashString);
		  commandString = strConcat(commandString, myVector[0]);
		  retVal = execve(commandString, &myVector[0], envp);
		  counter++;
		}
		fprintf(stderr, "command not found\n");           //failed to find the command, so kill child process, free memory
		free(commandString);
		kill(getpid(), SIGUSR1);
	      }
	    }
	    else{                                                // path followed by parent

	      int waitID = wait(&status);                        // parent awaits return from chid process
	      if(WIFEXITED(status)) {
		if(WEXITSTATUS(status) !=0){                     //check if child returned abnormally, print error code if so
		  printf("Program terminated with exit code %d.\n", WEXITSTATUS(status));
		}
	      }
	    }
	    clearBuffer(ptrReadBuf, numBytesRead);                 // clear buffer for next read
	    free(myVector);                                        // free the memory allocated by malloc
	}
    }
  }
  else{
    assert(42==write(2,"An illegal number of arguments were given\n", 42));
    return -1;
  }
  return 0;
}
