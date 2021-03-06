/*******************************************************
Christopher K. Tarango
9/24/17
CS4375
Shell Lab (Part 1)

mytoc.h
*******************************************************/

#ifndef MytocIncluded
#define MytocIncluded

char **mytoc(char *str, char delim);
int tokenCounter(char *bufPtr, char delim);
void clearBuffer(char *bufferPointer, int bytesRead);
int stringLength(char *stringPointer);
int strComp(char *str, char *comp);
char *strConcat(char *head, char *tail);

#endif // included
