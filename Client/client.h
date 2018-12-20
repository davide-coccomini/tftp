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



void initMessage(int, const char*, const char*);
void helpCmd();
void getCmd(int, char*, char*, struct sockaddr_in);
void modeCmd(char*, char*);
void quitCmd();
void sendCommand(int, char*);




#endif
