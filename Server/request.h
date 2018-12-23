#ifndef REQUEST_H
#define REQUEST_H

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>



struct request {
	int sock;
	FILE* fp;
	int packets;
	char* mode;
	int block;
	struct request* next;
} requestList;


void initRequestList();
void addRequest(int, FILE*, int, char*, int);
void removeRequest(int);
struct request* findRequest(int);


#endif
