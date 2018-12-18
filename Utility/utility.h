#ifndef UTILITY_H
#define UTILITY_H

#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

void sendString(int, char*);
char* receiveString(int);

void sendSize(int, unsigned int);
unsigned int receiveSize(int);

void sendFileTxt(int, char*);
char* receiveFileTxt(int);

void sendFileBin(int, FILE*);
FILE* receiveFileBin(int, unsigned int);

void sendACK(int);
unsigned int receiveACK(int);

#endif
