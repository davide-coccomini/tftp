#ifndef CLIENT_H
#define CLIENT_H

#include <sys/types.h>
#include <sys/socket.h>
#include <unistd.h>
#include <pthread.h>
#include <arpa/inet.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

#define CMD_SIZE 50
#define BUFFER_SIZE 1024


void initMessage();
void helpCmd();
void getCmd();
void modeCmd();
void quitCmd();




#endif