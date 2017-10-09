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

  int numBytesRead, bufferLen= BUFLEN, childID, retVal;
  /*Status Values*/
  int  status, exitValue = 0, writeStatus=0, cdValue, bgstatus=0;
  /*Counters*/
  int  counter, tokenCount, pipeCount;
  /*Pointers and Buffer*/
  char readBuf[BUFLEN], directoryBuf[BUFLEN], *ptrReadBuf, *commandString, *directoryPtr;
  /*Vectors*/
  char **myVector, **pathVector, **enVector;
  /*String Constants*/
  char slashString[]="/", exitString[]="exit", pathString[]="PATH",
    ps1String[]="PS1", pipeString[]="|", cdString[]="cd", ampString[]="&";


  /*Initialization process*/
  ptrReadBuf= readBuf;
  if(argc == 1){                                                 // makes sure no arguments were given when starting program
    for(counter=0; envp[counter] != (char *)0; counter++){
      enVector = mytoc(envp[counter], '=');                      // tokenize environment 
      if(strComp(enVector[0], ps1String) == 1 && enVector[1]==NULL){
	  writeStatus=1;
      }
      if(strComp(enVector[0], pathString) ==1){                  // find the path variable
	pathVector = mytoc(enVector[1],':');                     // tokenize path variable
      }
    }
    free(enVector);                                              //needed values found from enVector so free it



    /*begin shell logic*/
    while (exitValue == 0){                                      // loop through program till exit condition given
      if(writeStatus==0){
	assert(2==write(1,"$ ",2));                                // print the prompt if not supressed
      }

      numBytesRead = read(0, ptrReadBuf, bufferLen);           // read into the buffer
      if (numBytesRead==0 || *ptrReadBuf == EOF){
	  exitValue =1;
      }
      else{
	tokenCount = tokenCounter(ptrReadBuf, ' ');
	myVector = mytoc(ptrReadBuf, ' ');                       // call to the mytoc function 
	if (tokenCount == 1){                                    // check for exit string
	  exitValue = strComp(exitString, myVector[0]);    
	    
	}
	/*implement logic for cd in parent*/
	if (tokenCount >0){
	  cdValue =strComp(cdString, myVector[0]);
	  if(cdValue==1){
	    directoryPtr = directoryBuf;
	    if(getcwd(directoryBuf, sizeof(directoryBuf))== NULL){
	      return -1;
	    }
	    else{
	      commandString = strConcat(directoryPtr, slashString);
	      commandString = strConcat(commandString, myVector[1]);
	      int cderror =chdir(commandString);
	      if(cderror != 0){
		fprintf(stderr, "chdir has failed with error code : %d\n", cderror);
	      }
	      free(commandString);
	      clearBuffer(directoryPtr, sizeof(directoryBuf));
	    }
	  }
	  bgstatus = strComp(ampString, myVector[tokenCount-1]);  
	  if (bgstatus ==1){
	    myVector[tokenCount-1] =NULL;
	  }
	}	
      }
      if (tokenCount > 0 && exitValue == 0 && cdValue==0){                   // empty string or exit value bypasses code block
	int rc = fork();                                       // attempt to fork
	if (rc < 0){                                           // fork failed
	   fprintf(stderr, "fork failed\n");
	   exit(1);
        }
	  else if(rc==0){                                        // fork success
            /*if child is to run in background, it's disconnect from parent process group*/
	    if(bgstatus ==1){
	      setsid();
	    }
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
	      if(bgstatus==1){
		if(0!=waitpid(0,&status,WNOHANG)){
		  if(WIFEXITED(status)) {
		    if(WEXITSTATUS(status) !=0){                     //check if child returned abnormally, print error code if so
		      fprintf(stderr,"Program terminated with exit code %d.\n", WEXITSTATUS(status));
		    }
		  }
		}
	      }
	      else{
		int waitID = wait(&status);                        // parent awaits return from chid process
		if(WIFEXITED(status)) {
		  if(WEXITSTATUS(status) !=0){                     //check if child returned abnormally, print error code if so
		    fprintf(stderr,"Program terminated with exit code %d.\n", WEXITSTATUS(status));
		  }
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
  free(pathVector);
  return 0;
}
