#ifndef REQUEST_H
#define REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <arpa/inet.h>


struct request {
	int sock;
	struct sockaddr_in client_addr;
	FILE* fp;
	int packets;
	char* mode;
	int block;
	struct request* next;
} requestList;


void initRequestList();
void addRequest(int, struct sockaddr_in, FILE*, int, char*, int);
void removeRequest(int);
void resetRequestList();
int findMaxSocket(int);
struct request* findRequest(int);


#endif
