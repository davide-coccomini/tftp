#include "request.h"

void initRequestList(){
	requestList.sock = -1;
	requestList.fp = NULL;
	requestList.packets = 0;
	requestList.next = NULL;
}

void addRequest(int sock, FILE* fp, int packets,char* mode, int block){
	struct request *r = malloc(sizeof(struct request));
	r->sock = sock;
	r->fp = fp;
	r->packets = packets;
	r->mode = malloc(sizeof(mode)+1);
	strcpy(r->mode, mode);
	r->block = block;
	r->next = NULL;
	if(requestList.next == NULL){
		requestList.next = r;
		return;
	}
	struct request* oldRequest = requestList.next;
	struct request* tmp = NULL;
	while(oldRequest != NULL){
		tmp = oldRequest;
		oldRequest = oldRequest->next;
	}

	tmp->next = r;
	return;
}



void removeRequest(int sock){
	struct request *r = requestList.next;
	struct request* prec = NULL;

	for(; r; prec = r, r = r-> next){
		if(sock == r->sock){
			break;
		}			
	}
	if(r){
		if(r == requestList.next){
			requestList.next = r->next;
		}else{
			prec->next = r->next;
		}
	}
}

struct request* findRequest(int sock){

	struct request* r = &requestList;
	for(; r; r = r->next){
		if(r->sock == sock){
			break;
		}
	}
 return r;
}
