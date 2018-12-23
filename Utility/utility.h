#ifndef UTILITY_H
#define UTILITY_H

#define BUFFER_SIZE 1024
#define FILE_BUFFER_SIZE 512
#define PACKET_SIZE 516


#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <unistd.h>


struct result {
	int length;
	char* buffer;
	struct sockaddr_in client_addr;
};

void sendBuffer(int, char*, int, struct sockaddr_in);
struct result receiveBuffer(int);
#endif
