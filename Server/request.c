#include "request.h"

void initRequestList(){
	requestList.sock = -1;
	requestList.fp = NULL;
	requestList.packets = 0;
	requestList.next = NULL;
}

void addRequest(int sock, struct sockaddr_in client_addr, FILE* fp, int packets,char* mode, int block){
	struct request *r = malloc(sizeof(struct request));
	r->sock = sock;
	r->client_addr = client_addr;
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
void resetRequestList(){
	struct request* r = requestList.next;
	for(; r; r = r->next){
		free(r->next);
	}
 initRequestList();
}
int findMaxSocket(int listener){
	struct request* r = &requestList;
	int max = listener;
	for(; r; r = r->next){
		if(max < r->sock){
			max = r->sock;
		}
	}
   return max;
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
