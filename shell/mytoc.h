/*******************************************************
Christopher K. Tarango
9/11/17
CS4375
Tokenizer Lab

mytoc.h
*******************************************************/

#ifndef MytocIncluded
#define MytocIncluded

char **mytoc(char *str, char delim);
int tokenCounter(char *bufPtr, char delim);
void clearBuffer(char *bufferPointer, int bytesRead);
int stringLength(char *stringPointer);
int strComp(char *str, char *comp);
#endif // included
