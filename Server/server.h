#ifndef SERVER_H
#define SERVER_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <time.h>
#include "request.h"

#define BUFFER_SIZE 1024


//int receiveCommand(int, char*);
void getCmd(int, char*, struct sockaddr_in);

#endif

