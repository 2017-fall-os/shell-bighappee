/*******************************************************
Christopher K. Tarango
9/11/17
CS4375
Tokenizer Lab

mytoc.c
*******************************************************/

#include <stdlib.h>                                         // This include is for malloc/calloc/free
#include "mytoc.h"                                          // for consistency


/* function that counts number of tokens that were read into the the buffer*/
int tokenCounter(char *bufPtr, char delim){

  char *readptr = bufPtr, delimiter = delim;
  int counter = 0, tokenOn = 0;

  while(*readptr != 0){                                     // while the pointer has not reached the string terminator
    if(tokenOn == 0){                                         
      if (*readptr == 10){                                  // identifies line feed character which occurs when 
      }                                                     // the string has trailing spaces or no input
      else{
	if (*readptr != delimiter){                         // non-delimiter encountered activate tokenOn
	  tokenOn =1;                                       // and increment the counter
	  counter++;
	}
      }
    }
    else{
      if(*readptr == delimiter){                            // tokenOn already activated so when a delimiter is 
	tokenOn =0;                                         // encounterd de-activate tokenOn
      }
    }
    readptr++;
  }
  return counter;
}


char **mytoc(char *str, char delim){
  /*declarations*/
  int  tokenCount, counter =0, copyCounter, length=0;
  char *ptrReadBuf=str, delimiter=delim, *tokenMarker;

  tokenCount = tokenCounter(ptrReadBuf, delimiter);                              //count the number of tokens in the buffer
  char **tokenVec = (char**)calloc(tokenCount+1, sizeof(char *));                //allocate the vector with an additional
                                                                                 //index for the null pointer in the final index
  if (tokenCount > 0) {
    while (*ptrReadBuf != 0){                                                    //iterate through buffer
      if(*ptrReadBuf == 10){                                                     //skip line feed character
	ptrReadBuf++;
      }
      if(*ptrReadBuf != delim && *ptrReadBuf !=10 && *ptrReadBuf != 0){          //token character recognized  
	tokenMarker = ptrReadBuf;                                                //set marker to the beginning of token
      
	while(*ptrReadBuf != delim && *ptrReadBuf !=10 && *ptrReadBuf != 0){     //iterate through length of the token 
	  length++;                                                              //while incrementing the length
	  ptrReadBuf++;
	}
	tokenVec[counter]=(char *)malloc(length+1);                              //token ends so allocate space to copy into
                                                                                 //additional space given for 0 character
	for(copyCounter=0; copyCounter<length; copyCounter++){                
	  tokenVec[counter][copyCounter]= *tokenMarker;                          //copy the token into vector
	  tokenMarker++;
	}
	tokenVec[counter][length]=0;                                             //add 0 character to final index
	length=0;                                                                //reset length and move to the next index
	counter++;
      }
      else{
	ptrReadBuf++;                                                            //non-token character read so skip to next
      }
      if(counter==tokenCount){                                                   //vector is filled so break out of loop
	break;
      }
    }
  }
  tokenVec[tokenCount] =(char *)0;                                               //point last index of vector to null address space
  return tokenVec;                                                               // return the vector


  /*compares two strings (case insensitive) if the same returns 1 else returns 0*/
int strComp(char *str, char *comp){
  return 0;
}

/*concatenates two strings with '/' character between them*/

char *strConcat(char *head, char *tail){
}

/*clears the given buffer from previous inputs to ready for next series of inputs*/

 void clearBuffer(char *bufferPointer, int bytesRead){
   char *iterator = bufferPointer;
   int counter =0;
   while (counter< bytesRead){
     *iterator = (char *)0;
     counter++;
     iterator++;
   }
 }
