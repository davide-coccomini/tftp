#ifndef UTILITY_H
#define UTILITY_H

#define BUFFER_SIZE 1024
#define FILE_BUFFER_SIZE 512


#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>

void sendBuffer(int, char*, int, struct sockaddr_in);
char* receiveBuffer(int);


#endif
