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
  int  status, exitValue = 0, writeStatus=0;
  /*Counters*/
  int  counter, tokenCount, pipeCount;
  /*Pointers and Buffer*/
  char readBuf[BUFLEN], *ptrReadBuf, *commandString;
  /*Vectors*/
  char **myVector, **pathVector, **enVector, **pipeVector;
  /*String Constants*/
  char slashString[]="/", exitString[]="exit", pathString[]="PATH", ps1String[]="PS1", pipeString[]="|", cdString[]="cd";


  /*initialization process*/
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

    /*begin loop while exit is false*/
    while (exitValue == 0){                                      // loop through program till exit condition given
      if(writeStatus==0){
	assert(2==write(1,"$ ",2));                                // print the prompt if not supressed
      }

      numBytesRead = read(0, ptrReadBuf, bufferLen);           // read into the buffer
      /*checking for exit conditions*/
      if (numBytesRead==0 || *ptrReadBuf == EOF){
	  exitValue =1;
      }
      else{
	tokenCount = tokenCounter(ptrReadBuf, ' ');
	myVector = mytoc(ptrReadBuf, ' ');                       // call to the mytoc function 
	if (tokenCount == 1){                                    // check for exit string
	  exitValue = strComp(exitString, myVector[0]);    
	}
      }
      /*no exit condition given begin by searching for pipes*/
      if (tokenCount > 0 && exitValue == 0){                   // empty string or exit value bypasses code block
	pipeCount = tokenCounter(ptrReadBuf,'|')-1;
	//pipeCount =0;                                          //WARNING SET TO BYPASS PIPE LOGIC
	if (pipeCount==0){
	  int rc = fork();                                       // attempt to fork
	  if (rc < 0){                                           // fork failed
	    fprintf(stderr, "fork failed\n");
	    exit(1);
	  }
	  else if(rc==0){                                        // fork success
	    childID =  (int) getpid();
	    //printf("making comparison for cd\n");
	    if(strComp(cdString,myVector[0])==1){
	      //printf("cd command recognized, trying to append : %s\n", myVector[1]);
	      //int cdstat =changeDir(myVector[1]);
	      //printf("cd returned with value of %d\n", cdstat);
	      kill(getpid(), SIGUSR1);
	    }
	    else{
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
	  }
	    else{                                                // path followed by parent

	      int waitID = wait(&status);                        // parent awaits return from chid process
	      if(WIFEXITED(status)) {
		if(WEXITSTATUS(status) !=0){                     //check if child returned abnormally, print error code if so
		  fprintf(stderr,"Program terminated with exit code %d.\n", WEXITSTATUS(status));
		}
	      }
	    }
	    clearBuffer(ptrReadBuf, numBytesRead);                 // clear buffer for next read
	    free(myVector);                                        // free the memory allocated by malloc
	}
	else{
	  pipeVector= mytoc(ptrReadBuf, '|');
	  status = pipefunction(pipeVector, pipeCount, envp, pathVector);
	  printf("status returned: %d\n", status);
	  //pipeCount=0;
	  clearBuffer(ptrReadBuf, numBytesRead);                 // clear buffer for next read
	  free(myVector);                                        // free the memory allocated by malloc
	}
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


int pipefunction(char **vectorOfCommands,int  numOfPipes,char **envp, char **pathVector){
  /*create an array with 2 file descriptors for each pipe*/
  int *pipefd, counter, retVal, childID, status, pathCounter;
  char **tokenVector, *commandString;
  printf("current number of pipes brought it :  %d\n", numOfPipes);
  pipefd = (int *) malloc(numOfPipes*2);

  for(counter=0; counter<numOfPipes; counter++){
    if(pipe(pipefd + (counter*2)) <0){
      fprintf(stderr, "Failed to produce pipes\n");
      return -1;
    }
    else{
      printf("Success for pipe %d\n", (counter+1));
      printf("current fd values : %d, %d\n", pipefd[0], pipefd[1]);
    }
  }
  counter =0;
  printf("parent id : %d\n", (int)getpid());
  while (counter <= numOfPipes){
    tokenVector=mytoc(vectorOfCommands[counter], ' ');
    printf("tokens created, counter = %d\n", counter);
    int rc = fork();
    if (rc < 0){
      fprintf(stderr, "fork failed\n");
      return-1;
    }
    
    if (rc ==0){
      printf("Success forking child %d: %d\n",counter+1, (int)getpid());
      printf("Child %d active: %d\n", counter+1, (int)getpid());
      /*first command*/
      printf("%d is counter value", counter);
      if (counter ==0){
	fprintf(stdout,"Setting file descriptors for command %d: %s...", counter-1, tokenVector[counter]);
	close(STDOUT_FILENO);
	if(dup(pipefd[1])<0){
	  fprintf(stderr, "dup process failed");
	  return -1;
	}
	else{
	  printf("fd [1] changed to %d ", pipefd[1]);
	}
	close(pipefd[0]);
	close(pipefd[1]);
      }
      /*final command*/
     
      else if(counter == numOfPipes){
	printf("Setting file descriptors for command %d: %s...", counter-1, tokenVector[counter]);
	close(0);
	if(dup(pipefd[(counter*2)-2])){
  	  fprintf(stderr, "dup process failed");
	  return -1;
	}
	close(pipefd[(counter*2)-2]);
	close(pipefd[(counter*2)-1]);
      }
      /*all other commands*/
      
      else{
	printf("Setting file descriptors for command %d: %s...", counter-1, tokenVector[counter]);
	close(1);
	if(dup(pipefd[counter*2]+1)){
  	  fprintf(stderr, "dup process failed");	  
	}
	close(pipefd[counter*2]);
	close(pipefd[(counter*2)+1]);
	close(0);
	if(dup(pipefd[(counter*2)-2])){
  	  fprintf(stderr, "dup process failed");	  
	}
	close(pipefd[(counter*2)-1]);
	close(pipefd[(counter*2)-2]);
      }
      /*attempt to execute command*/
      printf("attempting to execute command");
      childID =  (int) getpid();
      retVal = execve(tokenVector[0], &tokenVector[0], envp);    // attempt execve full path to program given
      if(retVal!=0){                                                   
	pathCounter =0;                                           
	while (pathVector[pathCounter] != 0){                                  // append the command to all possible paths
	  commandString = strConcat(pathVector[pathCounter], "/");
	  commandString = strConcat(commandString, tokenVector[0]);
	  retVal = execve(commandString, &tokenVector[0], envp);
	  pathCounter++;
	}
	fprintf(stderr, "command not found\n");           //failed to find the command, so kill child process, free memory
	free(commandString);
	kill(getpid(), SIGUSR1);
      }      
    }
    else{
      int waitID = wait(&status);                        // parent awaits return from chid process
      printf("Child %d has returned\n", counter+1);
      printf("current fd values : %d, %d\n", pipefd[0], pipefd[1]);
      if(WIFEXITED(status)) {
	if(WEXITSTATUS(status) !=0){                     //check if child returned abnormally, print error code if so
	  fprintf(stderr,"Program terminated with exit code %d.\n", WEXITSTATUS(status));
	  return -1;
	}
      }
    }
    free(tokenVector);
    counter++;
  }
  free(pipefd);
  return 0;
} 
int changeDir(char *path){
  //printf("changeDir function has been entered\n");
  char directoryBuf[BUFLEN], *dirptr, *returnVal, *tempString, slashString[]="/";
  dirptr= directoryBuf;
  if(getcwd(directoryBuf, sizeof(directoryBuf))== NULL){
    return -1;
  }
  else{
    //printf("the current working directrory is: %s\n", dirptr);
    tempString = strConcat(dirptr,"/");
    returnVal = strConcat(tempString,path);
    //printf("changing directrory to: %s\n", returnVal);
    int ret = chdir(returnVal);
    clearBuffer (dirptr, sizeof(directoryBuf));
    if(getcwd(directoryBuf, sizeof(directoryBuf))== NULL){
      return -1;
    }
    else{
      //  printf("the new working directory should be: %s\n", dirptr);
    }
    return ret;
  }  
}
