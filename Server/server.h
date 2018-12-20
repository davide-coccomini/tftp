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


#define BUFFER_SIZE 1024


void cmd(int, char*, fd_set*);
void getCmd(int, char*);

#endif

